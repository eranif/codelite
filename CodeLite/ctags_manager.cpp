//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : ctags_manager.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "CxxTemplateFunction.h"
#include "CxxVariable.h"
#include "CxxVariableScanner.h"
#include "asyncprocess.h"
#include "cl_indexer_reply.h"
#include "cl_indexer_request.h"
#include "cl_standard_paths.h"
#include "clindexerprotocol.h"
#include "code_completion_api.h"
#include "codelite_exports.h"
#include "cpp_comment_creator.h"
#include "cppwordscanner.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "named_pipe_client.h"
#include "parse_thread.h"
#include "precompiled_header.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "tags_options_data.h"
#include "tags_storage_sqlite3.h"
#include "wx/filename.h"
#include "wx/timer.h"
#include "wx/tokenzr.h"
#include "wxStringHash.h"
#include <algorithm>
#include <set>
#include <sstream>
#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/file.h>
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/xrc/xmlres.h>

//#define __PERFORMANCE
#include "performance.h"

#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer_%s"
#else
#define PIPE_NAME "/tmp/codelite_indexer.%s.sock"
#endif

const wxEventType wxEVT_UPDATE_FILETREE_EVENT = XRCID("update_file_tree_event");
const wxEventType wxEVT_TAGS_DB_UPGRADE = XRCID("tags_db_upgraded");
const wxEventType wxEVT_TAGS_DB_UPGRADE_INTER = XRCID("tags_db_upgraded_now");

//---------------------------------------------------------------------------
// Misc

#if 0
static bool isDarkColor(const wxColour& color)
{
    int evg = (color.Red() + color.Green() + color.Blue()) / 3;
    if(evg < 127) return true;
    return false;
}
#endif

// Descending sorting function
struct SDescendingSort {
    bool operator()(const TagEntryPtr& rStart, const TagEntryPtr& rEnd)
    {
        return rStart->GetName().CmpNoCase(rEnd->GetName()) > 0;
    }
};

/// Ascending sorting function
struct SAscendingSort {
    bool operator()(const TagEntryPtr& rStart, const TagEntryPtr& rEnd)
    {
        return rEnd->GetName().CmpNoCase(rStart->GetName()) > 0;
    }
};

struct tagParseResult {
    TagTreePtr tree;
    std::vector<CommentPtr>* comments;
    wxString fileName;
};

//////////////////////////////////////
// Adapter class to TagsManager
//////////////////////////////////////
static TagsManager* gs_TagsManager = NULL;

void TagsManagerST::Free()
{
    if(gs_TagsManager) { delete gs_TagsManager; }
    gs_TagsManager = NULL;
}

TagsManager* TagsManagerST::Get()
{
    if(gs_TagsManager == NULL) gs_TagsManager = new TagsManager();

    return gs_TagsManager;
}

//------------------------------------------------------------------------------
// CTAGS Manager
//------------------------------------------------------------------------------

TagsManager::TagsManager()
    : wxEvtHandler()
    , m_codeliteIndexerPath(wxT("codelite_indexer"))
    , m_codeliteIndexerProcess(NULL)
    , m_canRestartIndexer(true)
    , m_lang(NULL)
    , m_evtHandler(NULL)
    , m_encoding(wxFONTENCODING_DEFAULT)
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &TagsManager::OnIndexerTerminated, this);

    m_db = new TagsStorageSQLite();
    m_db->SetSingleSearchLimit(MAX_SEARCH_LIMIT);

    // Create databases
    m_ctagsCmd = wxT("  --excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ");

    // CPP keywords that are usually followed by open brace '('
    m_CppIgnoreKeyWords.insert(wxT("while"));
    m_CppIgnoreKeyWords.insert(wxT("if"));
    m_CppIgnoreKeyWords.insert(wxT("for"));
    m_CppIgnoreKeyWords.insert(wxT("switch"));
    m_symbolsCache.reset(new clCxxFileCacheSymbols());
}

TagsManager::~TagsManager()
{
    m_symbolsCache.reset(nullptr);
    if(m_codeliteIndexerProcess) {

        // Dont kill the indexer process, just terminate the
        // reader-thread (this is done by deleting the indexer object)
        m_canRestartIndexer = false;

#ifndef __WXMSW__
        m_codeliteIndexerProcess->Terminate();
#endif
        delete m_codeliteIndexerProcess;

#ifndef __WXMSW__
        // Clear the socket file
        std::stringstream s;
        s << wxGetProcessId();

        char channel_name[1024];
        memset(channel_name, 0, sizeof(channel_name));
        sprintf(channel_name, PIPE_NAME, s.str().c_str());
        ::unlink(channel_name);
        ::remove(channel_name);
#endif
    }
}

void TagsManager::OpenDatabase(const wxFileName& fileName)
{
    m_dbFile = fileName;
    ITagsStoragePtr db;
    db = m_db;

    bool retagIsRequired = false;
    if(fileName.FileExists() == false) { retagIsRequired = true; }

    db->OpenDatabase(fileName);
    db->SetEnableCaseInsensitive(!(m_tagsOptions.GetFlags() & CC_IS_CASE_SENSITIVE));
    db->SetSingleSearchLimit(m_tagsOptions.GetCcNumberOfDisplayItems());

    if(db->GetVersion() != db->GetSchemaVersion()) {
        db->RecreateDatabase();

        // Send event to the main frame notifying it about database recreation
        if(m_evtHandler) {
            wxCommandEvent event(wxEVT_TAGS_DB_UPGRADE_INTER);
            event.SetEventObject(this);
            m_evtHandler->ProcessEvent(event);
        }
    }

    if(retagIsRequired && m_evtHandler) {
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
        m_evtHandler->AddPendingEvent(e);
    }
}

TagTreePtr TagsManager::ParseSourceFile(const wxFileName& fp, std::vector<CommentPtr>* comments)
{
    wxString tags;

    if(!m_codeliteIndexerProcess) {
        clWARNING() << "Indexer process is not running..." << clEndl;
        return TagTreePtr(NULL);
    }
    SourceToTags(fp, tags);

    int dummy;
    TagTreePtr ttp = TagTreePtr(TreeFromTags(tags, dummy));

    if(comments && GetParseComments()) {
        // parse comments
        GetLanguage()->ParseComments(fp, comments);
    }
    return ttp;
}

TagTreePtr TagsManager::ParseSourceFile2(const wxFileName& fp, const wxString& tags, std::vector<CommentPtr>* comments)
{
    //	return ParseTagsFile(tags, project);
    int count(0);
    TagTreePtr ttp = TagTreePtr(TreeFromTags(tags, count));

    if(comments && GetParseComments()) {
        // parse comments
        GetLanguage()->ParseComments(fp, comments);
    }
    return ttp;
}

//-----------------------------------------------------------
// Database operations
//-----------------------------------------------------------

void TagsManager::Store(TagTreePtr tree, const wxFileName& path) { GetDatabase()->Store(tree, path); }

TagTreePtr TagsManager::Load(const wxFileName& fileName, TagEntryPtrVector_t* tags)
{
    TagTreePtr tree;
    TagEntryPtrVector_t tagsByFile;

    if(tags) {
        tagsByFile.insert(tagsByFile.end(), tags->begin(), tags->end());

    } else {
        GetDatabase()->SelectTagsByFile(fileName.GetFullPath(), tagsByFile);
    }

    // Load the records and build a language tree
    TagEntry root;
    root.SetName(wxT("<ROOT>"));
    tree.Reset(new TagTree(wxT("<ROOT>"), root));
    for(size_t i = 0; i < tagsByFile.size(); i++) {
        tree->AddEntry(*(tagsByFile.at(i)));
    }
    return tree;
}

void TagsManager::Delete(const wxFileName& path, const wxString& fileName)
{
    GetDatabase()->DeleteByFileName(path, fileName);
}

//--------------------------------------------------------
// Process Handling of CTAGS
//--------------------------------------------------------

void TagsManager::StartCodeLiteIndexer()
{
    if(!m_canRestartIndexer) return;

    // Run ctags process
    wxString cmd;
    wxString ctagsCmd;

    // build the command, we surround ctags name with double quatations
    wxString uid;
    uid << wxGetProcessId();

    if(m_codeliteIndexerPath.FileExists() == false) {
        CL_ERROR(wxT("ERROR: Could not locate indexer: %s"), m_codeliteIndexerPath.GetFullPath().c_str());
        m_codeliteIndexerProcess = NULL;
        return;
    }

    // concatenate the PID to identifies this channel to this instance of codelite
    cmd << wxT("\"") << m_codeliteIndexerPath.GetFullPath() << wxT("\" ") << uid << wxT(" --pid");
    m_codeliteIndexerProcess =
        CreateAsyncProcess(this, cmd, IProcessCreateDefault, clStandardPaths::Get().GetUserDataDir());
}

void TagsManager::RestartCodeLiteIndexer()
{
    if(m_codeliteIndexerProcess) { m_codeliteIndexerProcess->Terminate(); }

    // no need to call StartCodeLiteIndexer(), since it will be called automatically
    // by the termination handler
}

void TagsManager::SetCodeLiteIndexerPath(const wxString& path) { m_codeliteIndexerPath = path; }

void TagsManager::OnIndexerTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    wxDELETE(m_codeliteIndexerProcess);
    StartCodeLiteIndexer();
}

//---------------------------------------------------------------------
// Parsing
//---------------------------------------------------------------------
void TagsManager::SourceToTags(const wxFileName& source, wxString& tags)
{
    std::stringstream s;
    s << wxGetProcessId();

    char channel_name[1024];
    memset(channel_name, 0, sizeof(channel_name));
    sprintf(channel_name, PIPE_NAME, s.str().c_str());

    clNamedPipeClient client(channel_name);

    // Build a request for the indexer
    clIndexerRequest req;
    // set the command
    req.setCmd(clIndexerRequest::CLI_PARSE);

    // prepare list of files to be parsed
    std::vector<std::string> files;
    files.push_back(source.GetFullPath().mb_str(wxConvUTF8).data());
    req.setFiles(files);

    // set ctags options to be used
    wxString ctagsCmd;
    ctagsCmd << wxT(" ") << m_tagsOptions.ToString()
             << wxT(" --excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ");
    req.setCtagOptions(ctagsCmd.mb_str(wxConvUTF8).data());

    clDEBUG1() << "Sending CTAGS command:" << ctagsCmd << clEndl;
    // connect to the indexer
    if(!client.connect()) {
        clWARNING() << "Failed to connect to indexer process. Indexer ID:" << wxGetProcessId() << clEndl;
        return;
    }

    // send the request
    if(!clIndexerProtocol::SendRequest(&client, req)) {
        clWARNING() << "Failed to send request to indexer. Indexer ID:" << wxGetProcessId() << clEndl;
        return;
    }

    // read the reply
    clIndexerReply reply;
    clDEBUG1() << "SourceToTags: reading indexer reply" << clEndl;
    try {
        std::string errmsg;
        if(!clIndexerProtocol::ReadReply(&client, reply, errmsg)) {
            clWARNING() << "Failed to read indexer reply: " << (wxString() << errmsg) << clEndl;
            RestartCodeLiteIndexer();
            return;
        }
    } catch(std::bad_alloc& ex) {
        clWARNING() << "std::bad_alloc exception caught" << clEndl;
        tags.Clear();
        return;
    }

    clDEBUG1() << "SourceToTags: [" << reply.getTags() << "]" << clEndl;

    // convert the data into wxString
    if(m_encoding == wxFONTENCODING_DEFAULT || m_encoding == wxFONTENCODING_SYSTEM)
        tags = wxString(reply.getTags().c_str(), wxConvUTF8);
    else
        tags = wxString(reply.getTags().c_str(), wxCSConv(m_encoding));
    if(tags.empty()) { tags = wxString::From8BitData(reply.getTags().c_str()); }

    clDEBUG1() << "Tags:\n" << tags << clEndl;
}

TagTreePtr TagsManager::TreeFromTags(const wxString& tags, int& count)
{
    // Load the records and build a language tree
    TagEntry root;
    root.SetName(wxT("<ROOT>"));

    TagTreePtr tree(new TagTree(wxT("<ROOT>"), root));

    wxStringTokenizer tkz(tags, wxT("\n"));
    while(tkz.HasMoreTokens()) {
        TagEntry tag;
        wxString line = tkz.NextToken();

        line = line.Trim();
        line = line.Trim(false);
        if(line.IsEmpty()) continue;

        // Construct the tag from the line
        tag.FromLine(line);

        // Add the tag to the tree, locals are not added to the
        // tree
        count++;
        if(tag.GetKind() != wxT("local")) tree->AddEntry(tag);
    }
    return tree;
}

bool TagsManager::IsValidCtagsFile(const wxFileName& filename) const
{
    return FileExtManager::IsCxxFile(filename) || FileUtils::WildMatch(m_tagsOptions.GetFileSpec(), filename);
}

//-----------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>> Code Completion API START
//-----------------------------------------------------------------------------

void TagsManager::TagsByScopeAndName(const wxString& scope, const wxString& name, std::vector<TagEntryPtr>& tags,
                                     size_t flags)
{
    std::vector<wxString> derivationList;
    // add this scope as well to the derivation list

    wxString _scopeName = DoReplaceMacros(scope);
    derivationList.push_back(_scopeName);
    std::set<wxString> scannedInherits;
    GetDerivationList(_scopeName, NULL, derivationList, scannedInherits);

    // make enough room for max of 500 elements in the vector
    tags.reserve(500);
    wxArrayString scopes;

    for(size_t i = 0; i < derivationList.size(); i++) {
        // try the worksapce database for match
        scopes.Add(derivationList.at(i));
    }

    GetDatabase()->GetTagsByScopeAndName(scopes, name, flags & PartialMatch, tags);

    // and finally sort the results
    std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::TagsByScope(const wxString& scope, std::vector<TagEntryPtr>& tags)
{
    std::vector<wxString> derivationList;
    // add this scope as well to the derivation list
    wxString _scopeName = DoReplaceMacros(scope);
    derivationList.push_back(_scopeName);
    std::set<wxString> scannedInherits;
    GetDerivationList(_scopeName, NULL, derivationList, scannedInherits);

    // make enough room for max of 500 elements in the vector
    tags.reserve(500);

    for(size_t i = 0; i < derivationList.size(); i++) {
        wxString tmpScope(derivationList.at(i));
        tmpScope = DoReplaceMacros(tmpScope);

        // try the external database for match
        GetDatabase()->GetTagsByScope(derivationList.at(i), tags);
    }

    // and finally sort the results
    std::sort(tags.begin(), tags.end(), SAscendingSort());
}

bool TagsManager::WordCompletionCandidates(const wxFileName& fileName, int lineno, const wxString& expr,
                                           const wxString& text, const wxString& word,
                                           std::vector<TagEntryPtr>& candidates)
{
    PERF_START("WordCompletionCandidates");

    candidates.clear();
    wxString path, tmp;
    wxString typeName, typeScope;

    // remove the word from the expression
    wxString expression(expr);

    // Trim whitespace from right and left
    static wxString trimString(wxT("!<>=(){};\r\n\t\v "));

    expression.erase(0, expression.find_first_not_of(trimString));
    expression.erase(expression.find_last_not_of(trimString) + 1);
    tmp = expression;
    expression.EndsWith(word, &tmp);
    expression = tmp;

    wxString funcSig;
    std::vector<wxString> additionlScopes; // from 'using namespace XXX;' statements

    wxString scope;
    wxString scopeName = GetLanguage()->GetScopeName(text, &additionlScopes);

    bool showLanguageKeywords = (GetCtagsOptions().GetFlags() & CC_CPP_KEYWORD_ASISST);

    if(GetCtagsOptions().GetFlags() & CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING) {
        // Do a deep scan for 'using namespace'
        GetLanguage()->SetAdditionalScopes(additionlScopes, fileName.GetFullPath());
        additionlScopes = GetLanguage()->GetAdditionalScopes();
    }

    TagEntryPtr funcTag = FunctionFromFileLine(fileName, lineno);
    if(funcTag) { funcSig = funcTag->GetSignature(); }

    wxString oper;
    wxString tmpExp(expression);
    tmpExp.Trim().Trim(false);

    // Keep 3 vectors of the results
    // we keep 3 vectors so the end user will see the matches
    // by their importance order: locals -> scoped -> globals
    TagEntryPtrVector_t locals;
    TagEntryPtrVector_t scoped;
    TagEntryPtrVector_t globals;
    TagEntryPtrVector_t keywords;

    if(tmpExp.IsEmpty()) {
        // Collect all the tags from the current scope, and
        // from the global scope
        wxString curFunctionBody;
        wxString textAfterTokenReplacements;
        int lastFuncLine = funcTag ? funcTag->GetLine() : -1;
        textAfterTokenReplacements = GetLanguage()->ApplyCtagsReplacementTokens(text);
        scope = GetLanguage()->OptimizeScope(textAfterTokenReplacements, lastFuncLine, curFunctionBody);

        // First get the scoped tags
        TagsByScopeAndName(scopeName, word, scoped);
        if(scopeName != wxT("<global>")) {
            // No need to call it twice...
            GetGlobalTags(word, globals);
        }

        if(showLanguageKeywords) {
            // Collect language keywords
            GetKeywordsTagsForLanguage(word, kCxx, keywords);
        }

        // Allways collect the local and the function argument tags
        GetLocalTags(word, scope, locals, PartialMatch | IgnoreCaseSensitive | ReplaceTokens);
        GetLocalTags(word, funcSig, locals, PartialMatch | IgnoreCaseSensitive);

        for(size_t i = 0; i < additionlScopes.size(); i++) {
            TagsByScopeAndName(additionlScopes.at(i), word, scoped);
        }

        // for every vector filter the results
        DoFilterDuplicatesByTagID(locals, locals);
        DoFilterDuplicatesBySignature(locals, locals);

        DoFilterDuplicatesByTagID(scoped, scoped);
        DoFilterDuplicatesBySignature(scoped, scoped);

        DoFilterDuplicatesByTagID(globals, globals);
        DoFilterDuplicatesBySignature(globals, globals);

        // unified the results into a single match
        candidates.insert(candidates.end(), locals.begin(), locals.end());
        candidates.insert(candidates.end(), keywords.begin(), keywords.end());
        candidates.insert(candidates.end(), scoped.begin(), scoped.end());
        candidates.insert(candidates.end(), globals.begin(), globals.end());

    } else if(tmpExp == wxT("::")) {
        // Global scope only
        // e.g.: ::My <CTRL>+<SPACE>
        // Collect all tags from the global scope which starts with 'My' (i.e. 'word')
        std::vector<TagEntryPtr> tmpCandidates;
        GetGlobalTags(word, tmpCandidates);
        DoFilterDuplicatesByTagID(tmpCandidates, candidates);
        DoFilterDuplicatesBySignature(candidates, candidates);

    } else {
        wxString typeName, typeScope, dummy;
        bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
        if(!res) {
            PERF_END();
            return false;
        }

        // get all symbols realted to this scope
        scope = wxT("");
        if(typeScope == wxT("<global>"))
            scope << typeName;
        else
            scope << typeScope << wxT("::") << typeName;

        std::vector<TagEntryPtr> tmpCandidates, tmpCandidates1;
        TagsByScopeAndName(scope, word, tmpCandidates);
        DoFilterCtorDtorIfNeeded(tmpCandidates, oper);

        wxString partialName(word);
        partialName.MakeLower();

        if(partialName.IsEmpty() == false) {
            for(size_t i = 0; i < tmpCandidates.size(); i++) {
                wxString nm = tmpCandidates[i]->GetName();
                nm.MakeLower();
                if(nm.StartsWith(partialName)) { tmpCandidates1.push_back(tmpCandidates.at(i)); }
            }
            DoFilterDuplicatesByTagID(tmpCandidates1, candidates);
            DoFilterDuplicatesBySignature(candidates, candidates);
        } else {
            DoFilterDuplicatesByTagID(tmpCandidates, candidates);
            DoFilterDuplicatesBySignature(candidates, candidates);
        }

        DoSortByVisibility(candidates);
    }

    PERF_END();
    return true;
}

bool TagsManager::AutoCompleteCandidates(const wxFileName& fileName, int lineno, const wxString& expr,
                                         const wxString& text, std::vector<TagEntryPtr>& candidates)
{
    PERF_START("AutoCompleteCandidates");

    candidates.clear();
    wxString path;
    wxString typeName, typeScope;

    wxString expression(expr);
    static wxString trimLeftString(wxT("{};\r\n\t\v "));
    static wxString trimRightString(wxT("({};\r\n\t\v "));
    expression.erase(0, expression.find_first_not_of(trimLeftString));
    expression.erase(expression.find_last_not_of(trimRightString) + 1);
    wxString oper;
    wxString scopeTeamplateInitList;
    bool isGlobalScopeOperator(false);

    if(expression == wxT("::")) {
        // global scope
        isGlobalScopeOperator = true;

    } else {

        PERF_BLOCK("ProcessExpression")
        {
            bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper,
                                         scopeTeamplateInitList);
            if(!res) {
                PERF_END();
                CL_DEBUG(wxT("Failed to resolve %s"), expression.c_str());
                return false;
            }
        }
    }

    // Load all tags from the database that matches typeName & typeScope
    wxString scope;
    if(typeScope == wxT("<global>"))
        scope << typeName;
    else
        scope << typeScope << wxT("::") << typeName;

    // this function will retrieve the ineherited tags as well
    // incase the last operator used was '::', retrieve all kinds of tags. Otherwise (-> , . operators were used)
    // retrieve only the members/prototypes/functions/enums
    wxArrayString filter;

    if(isGlobalScopeOperator) {
        // Fetch all tags from the global scope
        GetDatabase()->GetGlobalFunctions(candidates);

        if(candidates.empty() == false) std::sort(candidates.begin(), candidates.end(), SAscendingSort());

    } else if(oper == wxT("::")) {

        filter.Add(wxT("namespace"));
        filter.Add(wxT("class"));
        filter.Add(wxT("struct"));
        filter.Add(wxT("prototype"));
        filter.Add(wxT("function"));
        filter.Add(wxT("member"));
        filter.Add(wxT("typedef"));
        filter.Add(wxT("enum"));
        filter.Add(wxT("enumerator"));
        filter.Add(wxT("union"));

        PERF_BLOCK("TagsByScope") { TagsByScope(scope, filter, candidates, true); }

        // Let's search in typerefs
        if(candidates.empty()) {
            PERF_BLOCK("TagsByTyperef") { TagsByTyperef(scope, filter, candidates, true); }
        }

    } else {

        filter.Add(wxT("function"));
        filter.Add(wxT("member"));
        filter.Add(wxT("prototype"));
        PERF_BLOCK("TagsByScope") { TagsByScope(scope, filter, candidates, true); }
        DoFilterCtorDtorIfNeeded(candidates, oper);
    }

    PERF_END();

    std::vector<TagEntryPtr> noDupsVec;
    DoFilterDuplicatesBySignature(candidates, noDupsVec);
    noDupsVec.swap(candidates);

    DoSortByVisibility(candidates);
    return candidates.empty() == false;
}

void TagsManager::DoFilterDuplicatesBySignature(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
    // filter out all entries with the same signature (we do keep declaration overa an implenetation
    // since usually the declaration contains more useful information)
    std::map<wxString, TagEntryPtr> others, impls;

    for(size_t i = 0; i < src.size(); i++) {
        const TagEntryPtr& t = src.at(i);
        if(t->IsMethod()) {
            wxString strippedSignature = NormalizeFunctionSig(t->GetSignature(), 0);
            strippedSignature.Prepend(t->GetName());

            if(t->IsPrototype()) {
                // keep declaration in the output map
                others[strippedSignature] = t;
            } else {
                // keep the signature in a different map
                impls[strippedSignature] = t;
            }
        } else {
            // keep all other entries
            others[t->GetName()] = t;
        }
    }

    // unified the two multimaps
    std::map<wxString, TagEntryPtr>::iterator iter = impls.begin();
    for(; iter != impls.end(); iter++) {
        if(others.find(iter->first) == others.end()) { others[iter->first] = iter->second; }
    }

    target.clear();
    // convert the map into vector
    iter = others.begin();
    for(; iter != others.end(); iter++) {
        target.push_back(iter->second);
    }
}

void TagsManager::DoFilterDuplicatesByTagID(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
    std::map<int, TagEntryPtr> mapTags;
    std::map<wxString, TagEntryPtr> localTags;

    for(size_t i = 0; i < src.size(); i++) {
        const TagEntryPtr& t = src.at(i);
        int tagId = t->GetId();
        if(t->GetParent() == wxT("<local>")) {
            if(localTags.find(t->GetName()) == localTags.end()) { localTags[t->GetName()] = t; }

        } else if(mapTags.find(tagId) == mapTags.end()) {
            mapTags[tagId] = t;

        } else {
            tagId = -1;
        }
    }

    // Add the real entries (fetched from the database)
    std::map<int, TagEntryPtr>::iterator iter = mapTags.begin();
    for(; iter != mapTags.end(); iter++) {
        target.push_back(iter->second);
    }

    // Add the locals (collected from the current scope)
    std::map<wxString, TagEntryPtr>::iterator iter2 = localTags.begin();
    for(; iter2 != localTags.end(); iter2++) {
        target.push_back(iter2->second);
    }
}

void TagsManager::RemoveDuplicatesTips(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
    std::map<wxString, TagEntryPtr> unique_tags;

    for(size_t i = 0; i < src.size(); i++) {

        wxString raw_sig(src.at(i)->GetSignature().Trim().Trim(false));
        wxString sig;
        if(raw_sig.empty() == false) { sig = NormalizeFunctionSig(raw_sig, 0); }

        // the signature that we want to keep is one with name & default values, so try and get the maximum out of the
        // function signature
        bool hasDefaultValues = (raw_sig.Find(wxT("=")) != wxNOT_FOUND);

        wxString name = src.at(i)->GetName();
        wxString key = name + sig;

        std::map<wxString, TagEntryPtr>::iterator iter = unique_tags.find(key);
        if(iter == unique_tags.end()) {
            // does not exist
            unique_tags[key] = src.at(i);
        } else {
            // an entry with this key already exist
            if(hasDefaultValues) {
                // this entry has a default values, it means that we probably prefer this signature over the other
                TagEntryPtr t = iter->second;
                t->SetSignature(raw_sig);
                unique_tags[key] = t;
            }
        }
    }

    // conver the map back to std::vector
    std::map<wxString, TagEntryPtr>::iterator iter = unique_tags.begin();
    target.clear();

    for(; iter != unique_tags.end(); iter++) {
        target.push_back(iter->second);
    }
}

void TagsManager::GetGlobalTags(const wxString& name, std::vector<TagEntryPtr>& tags, size_t flags)
{
    // Make enough room for max of 500 elements in the vector
    tags.reserve(500);
    GetDatabase()->GetTagsByScopeAndName(wxT("<global>"), name, flags & PartialMatch, tags);
    std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::GetLocalTags(const wxString& name, const wxString& scope, std::vector<TagEntryPtr>& tags,
                               bool isFuncSignature, size_t flags)
{
    // collect tags from the current scope text
    GetLanguage()->GetLocalVariables(scope, tags, isFuncSignature, name, flags);
}

void TagsManager::GetHoverTip(const wxFileName& fileName, int lineno, const wxString& expr, const wxString& word,
                              const wxString& text, std::vector<wxString>& tips)
{
    wxString path;
    wxString typeName, typeScope, tmp;
    std::vector<TagEntryPtr> tmpCandidates, candidates;

    // remove the word from the expression
    wxString expression(expr);

    // Trim whitespace from right and left
    static wxString trimLeftString(wxT("{};\r\n\t\v "));
    static wxString trimRightString(wxT("({};\r\n\t\v "));
    expression.erase(0, expression.find_first_not_of(trimLeftString));
    expression.erase(expression.find_last_not_of(trimRightString) + 1);

    tmp = expression;
    expression.EndsWith(word, &tmp);
    expression = tmp;

    wxString curFunctionBody;
    wxString scope = GetLanguage()->OptimizeScope(text, -1, curFunctionBody);
    wxString scopeName = GetLanguage()->GetScopeName(scope, NULL);
    if(expression.IsEmpty()) {
        // collect all the tags from the current scope, and
        // from the global scope

        GetGlobalTags(word, tmpCandidates, ExactMatch);
        GetLocalTags(word, scope, tmpCandidates, false, ExactMatch);
        TagsByScopeAndName(scopeName, word, tmpCandidates);
        RemoveDuplicatesTips(tmpCandidates, candidates);

        // we now have a list of tags that matches our word
        TipsFromTags(candidates, word, tips);
    } else {
        wxString typeName, typeScope;
        wxString oper, dummy;
        bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
        if(!res) { return; }

        // get all symbols realted to this scope
        scope = wxT("");
        if(typeScope == wxT("<global>"))
            scope << typeName;
        else
            scope << typeScope << wxT("::") << typeName;

        std::vector<TagEntryPtr> tmpCandidates;
        TagsByScopeAndName(scope, word, tmpCandidates);
        RemoveDuplicatesTips(tmpCandidates, candidates);

        // we now have a list of tags that matches our word
        TipsFromTags(candidates, word, tips);
    }
}

void TagsManager::FindImplDecl(const wxFileName& fileName, int lineno, const wxString& expr, const wxString& word,
                               const wxString& text, std::vector<TagEntryPtr>& tags, bool imp, bool workspaceOnly)
{
    // Don't attempt to parse non valid ctags file
    if(!IsValidCtagsFile(fileName)) { return; }

    wxString path;
    wxString tmp;
    std::vector<TagEntryPtr> tmpCandidates;

    // remove the word from the expression
    wxString expression(expr);

    // Trim whitespace from right and left
    static wxString trimString(wxT("(){};\r\n\t\v "));

    expression.erase(0, expression.find_first_not_of(trimString));
    expression.erase(expression.find_last_not_of(trimString) + 1);
    tmp = expression;
    expression.EndsWith(word, &tmp);
    expression = tmp;
    expression.Trim().Trim(false);

    wxString scope(text);
    std::vector<wxString> visibleScopes;
    wxString scopeName = GetLanguage()->GetScopeName(scope, &visibleScopes);
    if(expression.IsEmpty() || expression == wxT("::")) {
        expression.Clear();

        // add the current scope to the "visibleScopes" to be tested
        if(scopeName != wxT("<global>")) {
            visibleScopes.push_back(scopeName);
            wxArrayString outerScopes = BreakToOuterScopes(scopeName);
            for(size_t i = 0; i < outerScopes.GetCount(); i++)
                visibleScopes.push_back(outerScopes.Item(i));
        }

        // collect tags from all the visible scopes
        for(size_t i = 0; i < visibleScopes.size(); i++)
            TagsByScopeAndName(visibleScopes.at(i), word, tmpCandidates, ExactMatch);

        if(tmpCandidates.empty()) {
            // no match in the given scope, try to collect from global scope as well
            GetGlobalTags(word, tmpCandidates, ExactMatch);
        }

        if(!imp) {
            // collect only implementation
            FilterImplementation(tmpCandidates, tags);

        } else {
            FilterDeclarations(tmpCandidates, tags);
        }

        if(tags.empty()) {
            TryFindImplDeclUsingNS(scopeName, word, imp, visibleScopes, tags);
            if(tags.empty()) TryReducingScopes(scopeName, word, imp, tags);
        }

    } else {
        wxString typeName, typeScope;
        wxString oper, dummy;
        bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
        if(!res) { return; }
        // get all symbols realted to this scope
        scope = wxT("");
        if(typeScope == wxT("<global>"))
            scope << typeName;
        else
            scope << typeScope << wxT("::") << typeName;

        std::vector<TagEntryPtr> tmpCandidates;
        TagsByScopeAndName(scope, word, tmpCandidates, ExactMatch);

        if(!imp) {
            // collect only implementation
            FilterImplementation(tmpCandidates, tags);
        } else {
            FilterDeclarations(tmpCandidates, tags);
        }

        if(tags.empty()) {
            TryFindImplDeclUsingNS(scope, word, imp, visibleScopes, tags);
            if(tags.empty()) TryReducingScopes(scope, word, imp, tags);
        }
    }
}

void TagsManager::TryReducingScopes(const wxString& scope, const wxString& word, bool imp,
                                    std::vector<TagEntryPtr>& tags)
{
    if(scope == wxT("<global>") || scope.IsEmpty()) return;

    // if we are here, it means that the the 'word' was not found in the 'scope'
    // and we already tried the 'TryFindImplDeclUsingNS' method.
    // What is left to be done is to reduce the 'scope' until we find a match.
    // Example:
    // OuterScope::Foo::Bar::Method()
    // However the entry in the database is stored only with as 'Bar::Method()'
    // we will reduce the scope and will try the following scopes:
    // Foo::Bar
    // Bar
    std::vector<wxString> visibleScopes;
    wxArrayString scopes = wxStringTokenize(scope, wxT(":"), wxTOKEN_STRTOK);
    for(size_t i = 1; i < scopes.GetCount(); i++) {
        wxString newScope;
        for(size_t j = i; j < scopes.GetCount(); j++) {
            newScope << scopes.Item(j) << wxT("::");
        }
        if(newScope.Len() >= 2) { newScope.RemoveLast(2); }
        visibleScopes.push_back(newScope);
    }
    std::vector<TagEntryPtr> tmpCandidates;
    if(visibleScopes.empty() == false) {
        for(size_t i = 0; i < visibleScopes.size(); i++) {
            TagsByScopeAndName(visibleScopes.at(i), word, tmpCandidates, ExactMatch);
        }

        if(!imp) {
            // collect only implementation
            FilterImplementation(tmpCandidates, tags);
        } else {
            FilterDeclarations(tmpCandidates, tags);
        }
    }
}

void TagsManager::TryFindImplDeclUsingNS(const wxString& scope, const wxString& word, bool imp,
                                         const std::vector<wxString>& visibleScopes, std::vector<TagEntryPtr>& tags)
{
    std::vector<TagEntryPtr> tmpCandidates;
    // if we got here and the tags.empty() is true,
    // there is another option to try:
    // sometimes people tend to write code similar to:
    // using namespace Foo;
    // void Bar::func(){}
    // this will make the entry in the tags database to have a scope of 'Bar' without
    // the Foo scope, however the ProcessExpression() method does take into consideration
    // the 'using namespace' statement, we attempt to fix this here
    if(visibleScopes.empty() == false) {
        tmpCandidates.clear();
        for(size_t i = 0; i < visibleScopes.size(); i++) {
            wxString newScope(scope);
            if(newScope.StartsWith(visibleScopes.at(i) + wxT("::"))) {
                newScope.Remove(0, visibleScopes.at(i).Len() + 2);
            }
            TagsByScopeAndName(newScope, word, tmpCandidates, ExactMatch);
        }

        if(!imp) {
            // collect only implementation
            FilterImplementation(tmpCandidates, tags);
        } else {
            FilterDeclarations(tmpCandidates, tags);
        }
    }
}

void TagsManager::FilterImplementation(const std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& tags)
{
    // remove all implementations and leave only declarations
    std::map<wxString, TagEntryPtr> tmpMap;
    for(size_t i = 0; i < src.size(); i++) {
        TagEntryPtr tag = src.at(i);
        if(tag->GetKind() != wxT("function")) {
            wxString key;
            key << tag->GetFile() << tag->GetLine();
            tmpMap[key] = tag;
        }
    }

    std::map<wxString, TagEntryPtr>::iterator iter = tmpMap.begin();
    for(; iter != tmpMap.end(); iter++) {
        tags.push_back(iter->second);
    }
}

void TagsManager::FilterDeclarations(const std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& tags)
{
    std::map<wxString, TagEntryPtr> tmpMap;
    for(size_t i = 0; i < src.size(); i++) {
        TagEntryPtr tag = src.at(i);
        if(tag->GetKind() != wxT("prototype")) {
            wxString key;
            key << tag->GetFile() << tag->GetLine();
            tmpMap[key] = tag;
        }
    }
    std::map<wxString, TagEntryPtr>::iterator iter = tmpMap.begin();
    for(; iter != tmpMap.end(); iter++) {
        tags.push_back(iter->second);
    }
}

clCallTipPtr TagsManager::GetFunctionTip(const wxFileName& fileName, int lineno, const wxString& expr,
                                         const wxString& text, const wxString& word)
{
    wxString path;
    wxString typeName, typeScope, tmp;
    std::vector<TagEntryPtr> tips;

    // Skip any C++ keywords
    if(m_CppIgnoreKeyWords.find(word) != m_CppIgnoreKeyWords.end()) { return NULL; }

    // Trim whitespace from right and left
    wxString expression(expr);
    static wxString trimLeftString(wxT("{};\r\n\t\v "));
    static wxString trimRightString(wxT("({};\r\n\t\v "));
    expression.erase(0, expression.find_first_not_of(trimLeftString));
    expression.erase(expression.find_last_not_of(trimRightString) + 1);

    // remove the last token from the expression
    expression.EndsWith(word, &tmp);
    expression = tmp;
    if(word.IsEmpty()) { return NULL; }

    CppScanner scanner;
    scanner.SetText(_C(word));
    if(scanner.yylex() != IDENTIFIER) { return NULL; }

    expression.Trim().Trim(false);
    if(expression.IsEmpty()) {
        DoGetFunctionTipForEmptyExpression(word, text, tips);

        if(tips.empty()) {
            // no luck yet
            // we now try this:
            // Perhaps our "function" is actually a constuctor, e.g.:
            // ClassName cls(
            wxString alteredText(text);
            alteredText.Append(wxT(";"));
            std::vector<TagEntryPtr> tmpCandidates;
            GetLocalTags(word, text, tmpCandidates, false, ExactMatch);
            if(tmpCandidates.size() == 1) {
                TagEntryPtr t = tmpCandidates.at(0);
                DoGetFunctionTipForEmptyExpression(t->GetScope(), text, tips);

            } else {
                // Stil no luck, try this:
                // Assume that the expression is a code-complete expression (i.e. an expression that ends with -> or .
                // and try to resolve it. If we succeed, we collect only the ctors matches from that list
                TagEntryPtrVector_t matches;
                tmpCandidates.clear();
                if(AutoCompleteCandidates(fileName, lineno, expr + ".", text, matches) && !matches.empty()) {
                    std::for_each(matches.begin(), matches.end(), TagEntry::ForEachCopyIfCtor(tmpCandidates));
                    GetFunctionTipFromTags(tmpCandidates, matches.at(0)->GetScopeName(), tips);
                }
            }
        }
    } else if(expression == wxT("::")) {
        // Test the global scope
        DoGetFunctionTipForEmptyExpression(word, text, tips, true);

    } else {
        wxString oper, dummy;
        clDEBUG1() << "Calling ProcessExpression(" << expression << ")" << clEndl;
        bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
        if(!res) {
            clDEBUG1() << "Calling ProcessExpression(" << expression << ") - returned FALSE" << clEndl;
            return NULL;
        }

        // load all tags from the database that matches typeName & typeScope
        wxString scope;
        if(typeScope == wxT("<global>"))
            scope << typeName;
        else
            scope << typeScope << wxT("::") << typeName;

        // this function will retrieve the ineherited tags as well
        std::vector<TagEntryPtr> tmpCandidates;
        TagsByScopeAndName(scope, word, tmpCandidates, ExactMatch);
        clDEBUG1() << "Building Calltip from tags, size:" << tmpCandidates.size() << clEndl;
        GetFunctionTipFromTags(tmpCandidates, word, tips);
    }

    // In case the user requested that the function signature will not be formatted
    // respect it and add the 'Tag_No_Signature_Format' flag
    if(GetCtagsOptions().GetFlags() & CC_KEEP_FUNCTION_SIGNATURE_UNFORMATTED) {
        for(size_t i = 0; i < tips.size(); i++) {
            tips.at(i)->SetFlags(TagEntry::Tag_No_Signature_Format);
        }
    }

    clCallTipPtr ct(new clCallTip(tips));
    return ct;
}

//-----------------------------------------------------------------------------
// <<<<<<<<<<<<<<<<<<< Code Completion API END
//-----------------------------------------------------------------------------
void TagsManager::OpenType(std::vector<TagEntryPtr>& tags)
{
    wxArrayString kinds;
    kinds.Add(wxT("class"));
    kinds.Add(wxT("namespace"));
    kinds.Add(wxT("struct"));
    kinds.Add(wxT("union"));
    kinds.Add(wxT("enum"));
    kinds.Add(wxT("typedef"));

    GetDatabase()->GetTagsByKind(kinds, wxT("name"), ITagsStorage::OrderDesc, tags);
}

void TagsManager::FindSymbol(const wxString& name, std::vector<TagEntryPtr>& tags)
{
    // since we dont get a scope, we better user a search that only uses the
    // name (GetTagsByScopeAndName) is optimized to search the global tags table
    GetDatabase()->GetTagsByName(name, tags, true);
}

void TagsManager::DeleteFilesTags(const wxArrayString& files)
{
    std::vector<wxFileName> files_;
    for(size_t i = 0; i < files.GetCount(); i++) {
        files_.push_back(files.Item(i));
    }
    DeleteFilesTags(files_);
}

void TagsManager::DeleteFilesTags(const std::vector<wxFileName>& projectFiles)
{
    if(projectFiles.empty()) { return; }

    // Put a request to the parsing thread to delete the tags for the 'projectFiles'
    ParseRequest* req = new ParseRequest(ParseThreadST::Get()->GetNotifiedWindow());
    req->setDbFile(GetDatabase()->GetDatabaseFileName().GetFullPath().c_str());
    req->setType(ParseRequest::PR_DELETE_TAGS_OF_FILES);
    req->_workspaceFiles.clear();
    req->_workspaceFiles.reserve(projectFiles.size());
    for(size_t i = 0; i < projectFiles.size(); i++) {
        req->_workspaceFiles.push_back(projectFiles.at(i).GetFullPath().mb_str(wxConvUTF8).data());
    }
    ParseThreadST::Get()->Add(req);
}

void TagsManager::RetagFiles(const std::vector<wxFileName>& files, RetagType type, wxEvtHandler* cb)
{
    wxArrayString strFiles;
    strFiles.Alloc(files.size()); // At most files.size() entries
    // step 1: remove all non-tags files
    for(size_t i = 0; i < files.size(); i++) {
        if(!IsValidCtagsFile(files.at(i).GetFullPath())) { continue; }

        strFiles.Add(files.at(i).GetFullPath());
    }

    // If there are no files to tag - send the 'end' event
    if(strFiles.IsEmpty()) {
        wxFrame* frame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
        if(frame) {
            wxCommandEvent retaggingCompletedEvent(wxEVT_PARSE_THREAD_RETAGGING_COMPLETED);
            frame->GetEventHandler()->AddPendingEvent(retaggingCompletedEvent);
        }
        return;
    }

    // step 2: remove all files which do not need retag
    if(type == Retag_Quick || type == Retag_Quick_No_Scan) DoFilterNonNeededFilesForRetaging(strFiles, GetDatabase());

    // If there are no files to tag - send the 'end' event
    if(strFiles.IsEmpty()) {
        wxFrame* frame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
        if(frame) {
            wxCommandEvent retaggingCompletedEvent(wxEVT_PARSE_THREAD_RETAGGING_COMPLETED);
            frame->GetEventHandler()->AddPendingEvent(retaggingCompletedEvent);
        }
        return;
    }

    // step 4: Remove tags belonging to these files
    DeleteFilesTags(strFiles);

    // step 5: build the database
    ParseRequest* req = new ParseRequest(ParseThreadST::Get()->GetNotifiedWindow());
    if(cb) {
        req->_evtHandler = cb; // Callback window
    }

    req->setDbFile(GetDatabase()->GetDatabaseFileName().GetFullPath().c_str());

    req->setType(type == Retag_Quick_No_Scan ? ParseRequest::PR_PARSE_FILE_NO_INCLUDES
                                             : ParseRequest::PR_PARSE_AND_STORE);
    req->_workspaceFiles.clear();
    req->_workspaceFiles.reserve(strFiles.size());
    for(size_t i = 0; i < strFiles.GetCount(); i++) {
        req->_workspaceFiles.push_back(strFiles[i].mb_str(wxConvUTF8).data());
    }
    ParseThreadST::Get()->Add(req);
}

void TagsManager::FindByNameAndScope(const wxString& name, const wxString& scope, std::vector<TagEntryPtr>& tags)
{
    wxString _name = DoReplaceMacros(name);
    wxString _scope = DoReplaceMacros(scope);
    DoFindByNameAndScope(_name, _scope, tags);

    // Sort the results base on their name
    std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::FindByPath(const wxString& path, std::vector<TagEntryPtr>& tags)
{
    GetDatabase()->GetTagsByPath(path, tags);
}

void TagsManager::DoFindByNameAndScope(const wxString& name, const wxString& scope, std::vector<TagEntryPtr>& tags)
{
    wxString sql;
    if(scope == wxT("<global>")) {
        // try the workspace database for match
        GetDatabase()->GetTagsByNameAndParent(name, wxT("<global>"), tags);
    } else {
        std::vector<wxString> derivationList;
        derivationList.push_back(scope);
        std::set<wxString> scannedInherits;
        GetDerivationList(scope, NULL, derivationList, scannedInherits);
        wxArrayString paths;
        for(size_t i = 0; i < derivationList.size(); i++) {
            wxString path_;
            path_ << derivationList.at(i) << wxT("::") << name;
            paths.Add(path_);
        }

        // try the workspace database for match
        GetDatabase()->GetTagsByPath(paths, tags);
    }
}

bool TagsManager::IsTypeAndScopeContainer(wxString& typeName, wxString& scope)
{
    wxString cacheKey;
    cacheKey << typeName << wxT("@") << scope;

    // we search the cache first, note that the cache
    // is used only for the external database
    std::map<wxString, bool>::iterator iter = m_typeScopeContainerCache.find(cacheKey);
    if(iter != m_typeScopeContainerCache.end()) { return iter->second; }

    // replace macros:
    // replace the provided typeName and scope with user defined macros as appeared in the PreprocessorMap
    wxString _typeName = DoReplaceMacros(typeName);
    wxString _scope = DoReplaceMacros(scope);

    bool res = GetDatabase()->IsTypeAndScopeContainer(_typeName, _scope);
    if(res) {
        typeName = _typeName;
        scope = _scope;
    }
    return res;
}

bool TagsManager::IsTypeAndScopeExists(wxString& typeName, wxString& scope)
{
    wxString cacheKey;
    cacheKey << typeName << wxT("@") << scope;

    // we search the cache first, note that the cache
    // is used only for the external database
    std::map<wxString, bool>::iterator iter = m_typeScopeCache.find(cacheKey);
    if(iter != m_typeScopeCache.end()) { return iter->second; }

    // First try the fast query to save some time
    if(GetDatabase()->IsTypeAndScopeExistLimitOne(typeName, scope)) { return true; }

    // replace macros:
    // replace the provided typeName and scope with user defined macros as appeared in the PreprocessorMap
    typeName = DoReplaceMacros(typeName);
    scope = DoReplaceMacros(scope);

    return GetDatabase()->IsTypeAndScopeExist(typeName, scope);
}

bool TagsManager::GetDerivationList(const wxString& path, TagEntryPtr derivedClassTag,
                                    std::vector<wxString>& derivationList, std::set<wxString>& scannedInherits)
{
    std::vector<TagEntryPtr> tags;
    TagEntryPtr tag;

    wxArrayString kind;
    kind.Add(wxT("class"));
    kind.Add(wxT("struct"));

    GetDatabase()->GetTagsByKindAndPath(kind, path, tags);

    if(tags.size() == 1) {
        tag = tags.at(0);
    } else {
        return false;
    }

    if(tag && tag->IsOk()) {

        // Get the template instantiation list from the child class
        wxArrayString ineheritsList = tag->GetInheritsAsArrayNoTemplates();

        wxString templateInstantiationLine;
        if(derivedClassTag) {
            wxArrayString p_ineheritsListT = derivedClassTag->GetInheritsAsArrayWithTemplates();
            wxArrayString p_ineheritsList = derivedClassTag->GetInheritsAsArrayNoTemplates();

            for(size_t i = 0; i < p_ineheritsList.GetCount(); i++) {
                if(p_ineheritsList.Item(i) == tag->GetName()) {
                    templateInstantiationLine = p_ineheritsListT.Item(i);
                    templateInstantiationLine = templateInstantiationLine.AfterFirst(wxT('<'));
                    templateInstantiationLine.Prepend(wxT("<"));
                    break;
                }
            }
        }

        for(size_t i = 0; i < ineheritsList.GetCount(); i++) {
            wxString inherits = ineheritsList.Item(i);
            wxString tagName = tag->GetName();
            wxString tmpInhr = inherits;

            bool isTempplate = (tag->GetPattern().Find(wxT("template")) != wxNOT_FOUND);
            tagName.MakeLower();
            tmpInhr.MakeLower();

            // Make sure that inherits != the current name or we will end up in an infinite loop
            if(tmpInhr != tagName) {
                wxString possibleScope(wxT("<global>"));

                // if the 'inherits' already contains a scope
                // dont attempt to fix it
                if(inherits.Contains(wxT("::")) == false) {

                    // Correc the type/scope
                    bool testForTemplate = !IsTypeAndScopeExists(inherits, possibleScope);

                    // If the type does not exists, check for templates
                    if(testForTemplate && derivedClassTag && isTempplate) {
                        TemplateHelper th;

                        // e.g. template<typename T> class MyClass
                        wxArrayString templateArgs = GetLanguage()->DoExtractTemplateDeclarationArgs(tag);
                        th.SetTemplateDeclaration(templateArgs);                // <typename T>
                        th.SetTemplateInstantiation(templateInstantiationLine); // e.g. MyClass<wxString>

                        wxString newType = th.Substitute(inherits);

                        // Locate the new type by name in the database
                        // this is done to make sure that the new type is not a macro...
                        if(!newType.IsEmpty() && newType != inherits) {

                            // check the user defined types for a replcement token
                            wxString replacement = DoReplaceMacros(newType);
                            if(replacement == newType) {
                                // No match was found in the user defined replacements
                                // try the database
                                replacement = DoReplaceMacrosFromDatabase(newType);
                            }
                            inherits = replacement;
                        }
                    }

                    if(possibleScope != wxT("<global>")) { inherits = possibleScope + wxT("::") + inherits; }
                }

                // Make sure that this parent was not scanned already
                if(scannedInherits.find(inherits) == scannedInherits.end()) {
                    scannedInherits.insert(inherits);
                    derivationList.push_back(inherits);
                    GetDerivationList(inherits, tag, derivationList, scannedInherits);
                }
            }
        }
    }
    return true;
}

void TagsManager::TipsFromTags(const std::vector<TagEntryPtr>& tags, const wxString& word, std::vector<wxString>& tips)
{
    wxString retValueColour = "\"white\"";
    for(size_t i = 0; i < tags.size(); i++) {
        if(tags.at(i)->GetName() != word) continue;

        wxString tip = tags.at(i)->GetPattern();

        // remove the pattern perfix and suffix
        tip = tip.Trim().Trim(false);
        tip = tip.AfterFirst(wxT('^'));
        if(tip.Find(wxT('$')) != wxNOT_FOUND) {
            tip = tip.BeforeLast(wxT('$'));
        } else {
            if(tip.EndsWith(wxT("/"))) { tip = tip.BeforeLast(wxT('/')); }
        }

        // since the tip is built from the pattern, which is actually a regex expression
        // some characters might be escaped (e.g. '/' will appear as '\/')
        tip.Replace(wxT("\\/"), wxT("/"));

        // Trim whitespace from right and left
        static wxString trimString(wxT("{};\r\n\t\v "));

        tip.erase(0, tip.find_first_not_of(trimString));
        tip.erase(tip.find_last_not_of(trimString) + 1);
        tip.Replace(wxT("\t"), wxT(" "));

        // create a proper tooltip from the stripped pattern
        TagEntryPtr t = tags.at(i);
        if(t->IsMethod()) {

            // add return value
            tip.Clear();

            wxString ret_value = GetFunctionReturnValueFromPattern(t);
            if(ret_value.IsEmpty() == false) {
                tip << "<b>" << ret_value << wxT("</b> ");
            } else {
                wxString retValue = t->GetReturnValue();
                if(retValue.IsEmpty() == false) { tip << "<b>" << retValue << wxT("</b> "); }
            }

            // add the scope
            if(!t->IsScopeGlobal() && !t->IsConstructor() && !t->IsDestructor()) { tip << t->GetScope() << wxT("::"); }

            // name
            tip << "<b><color=\"white\">" << t->GetName() << "</color></b>";

            // method signature
            tip << NormalizeFunctionSig(t->GetSignature(), Normalize_Func_Name | Normalize_Func_Default_value);
        }

        // remove any extra spaces from the tip
        while(tip.Replace(wxT("  "), wxT(" "))) {}

        // BUG#3082954: limit the size of the 'match pattern' to a reasonable size (200 chars)
        tip = WrapLines(tip);

        if(!tips.empty()) { tip.Prepend("\n"); }

        // prepend any comment if exists
        tips.push_back(tip);
    }
}

void TagsManager::GetFunctionTipFromTags(const std::vector<TagEntryPtr>& tags, const wxString& word,
                                         std::vector<TagEntryPtr>& tips)
{
    std::map<wxString, TagEntryPtr> tipsMap;
    std::vector<TagEntryPtr> ctor_tags;

    for(size_t i = 0; i < tags.size(); i++) {
        if(tags.at(i)->GetName() != word) continue;

        TagEntryPtr t;
        TagEntryPtr curtag = tags.at(i);

        // try to replace the current tag with a macro replacement.
        // we dont temper with 'curtag' content since we dont want
        // to modify cached items
        t = curtag->ReplaceSimpleMacro();
        if(!t) { t = curtag; }

        wxString pat = t->GetPattern();

        if(t->IsMethod()) {
            wxString tip;
            tip << wxT("function:") << t->GetSignature();

            // collect each signature only once, we do this by using
            // map
            tipsMap[tip] = t;

        } else if(t->IsClass()) {

            // this tag is a class declaration that matches the word
            // user is probably is typing something like
            // Class *a = new Class(
            // or even Class a = Class(
            // the steps to take from here:
            // - lookup in the tables for tags that matches path of: WordScope::Word::Word and of type
            // prototype/function
            wxString scope;
            if(t->GetScope().IsEmpty() == false && t->GetScope() != wxT("<global>")) {
                scope << t->GetScope() << wxT("::");
            }

            scope << t->GetName();
            ctor_tags.clear();
            TagsByScopeAndName(scope, t->GetName(), ctor_tags, ExactMatch);

            for(size_t i = 0; i < ctor_tags.size(); i++) {
                TagEntryPtr ctor_tag = ctor_tags.at(i);
                if(ctor_tag->IsMethod()) {
                    wxString tip;
                    tip << wxT("function:") << ctor_tag->GetSignature();
                    tipsMap[ctor_tag->GetSignature()] = ctor_tag;
                }
            }

        } else if(t->IsMacro()) {

            wxString tip;
            wxString macroName = t->GetName();
            wxString pattern = t->GetPattern();

            int where = pattern.Find(macroName);
            if(where != wxNOT_FOUND) {
                // remove the #define <name> from the pattern
                pattern = pattern.Mid(where + macroName.Length());
                pattern = pattern.Trim().Trim(false);
                if(pattern.StartsWith(wxT("("))) {
                    // this macro has the form of a function
                    pattern = pattern.BeforeFirst(wxT(')'));
                    pattern.Append(wxT(')'));

                    tip << wxT("macro:") << pattern;

                    // collect each signature only once, we do this by using
                    // map
                    tipsMap[tip] = t;
                }
            }
        }
    }

    for(std::map<wxString, TagEntryPtr>::iterator iter = tipsMap.begin(); iter != tipsMap.end(); iter++) {
        tips.push_back(iter->second);
    }
}

void TagsManager::CloseDatabase()
{
    m_dbFile.Clear();
    m_db = NULL; // Free the current database
    m_db = new TagsStorageSQLite();
    m_db->SetSingleSearchLimit(m_tagsOptions.GetCcNumberOfDisplayItems());
    m_db->SetUseCache(true);
}

DoxygenComment TagsManager::GenerateDoxygenComment(const wxString& file, const int line, wxChar keyPrefix)
{
    if(GetDatabase()->IsOpen()) {
        TagEntryPtr tag = GetDatabase()->GetTagAboveFileAndLine(file, line);
        if(!tag) { return DoxygenComment(); }

        // create a doxygen comment from the tag
        return DoCreateDoxygenComment(tag, keyPrefix);
    }
    return DoxygenComment();
}

DoxygenComment TagsManager::DoCreateDoxygenComment(TagEntryPtr tag, wxChar keyPrefix)
{
    CppCommentCreator commentCreator(tag, keyPrefix);
    DoxygenComment dc;
    dc.comment = commentCreator.CreateComment();
    dc.name = tag->GetName();
    return dc;
}

bool TagsManager::GetParseComments() { return m_parseComments; }

void TagsManager::SetCtagsOptions(const TagsOptionsData& options)
{
    m_tagsOptions = options;
    RestartCodeLiteIndexer();
    m_parseComments = m_tagsOptions.GetFlags() & CC_PARSE_COMMENTS ? true : false;
    ITagsStoragePtr db = GetDatabase();
    if(db) { db->SetSingleSearchLimit(m_tagsOptions.GetCcNumberOfDisplayItems()); }
}

void TagsManager::GenerateSettersGetters(const wxString& scope, const SettersGettersData& data,
                                         const std::vector<TagEntryPtr>& tags, wxString& impl, wxString* decl)
{
    wxUnusedVar(scope);
    wxUnusedVar(data);
    wxUnusedVar(tags);
    wxUnusedVar(impl);
    wxUnusedVar(decl);
}

void TagsManager::TagsByScope(const wxString& scopeName, const wxString& kind, std::vector<TagEntryPtr>& tags,
                              bool includeInherits, bool applyLimit)
{
    wxString sql;
    std::vector<wxString> derivationList;
    // add this scope as well to the derivation list
    derivationList.push_back(scopeName);
    std::set<wxString> scannedInherits;
    if(includeInherits) { GetDerivationList(scopeName, NULL, derivationList, scannedInherits); }

    // make enough room for max of 500 elements in the vector
    tags.reserve(500);
    wxArrayString kinds, scopes;
    kinds.Add(kind);

    for(size_t i = 0; i < derivationList.size(); i++) {
        scopes.Add(derivationList.at(i));
    }

    if(applyLimit)
        GetDatabase()->GetTagsByScopesAndKind(scopes, kinds, tags);
    else
        GetDatabase()->GetTagsByScopesAndKindNoLimit(scopes, kinds, tags);
}

wxString TagsManager::GetScopeName(const wxString& scope)
{
    Language* lang = GetLanguage();
    return lang->GetScopeName(scope, NULL);
}

bool TagsManager::ProcessExpression(const wxFileName& filename, int lineno, const wxString& expr,
                                    const wxString& scopeText, wxString& typeName, wxString& typeScope, wxString& oper,
                                    wxString& scopeTempalteInitiList)
{
    return GetLanguage()->ProcessExpression(expr, scopeText, filename, lineno, typeName, typeScope, oper,
                                            scopeTempalteInitiList);
}

bool TagsManager::GetMemberType(const wxString& scope, const wxString& name, wxString& type, wxString& typeScope)
{
    wxString expression(scope);
    expression << wxT("::") << name << wxT(".");
    wxString dummy;
    return GetLanguage()->ProcessExpression(expression, wxEmptyString, wxFileName(), wxNOT_FOUND, type, typeScope,
                                            dummy, dummy);
}

void TagsManager::GetFiles(const wxString& partialName, std::vector<FileEntryPtr>& files)
{
    if(GetDatabase()) { GetDatabase()->GetFiles(partialName, files); }
}

void TagsManager::GetFiles(const wxString& partialName, std::vector<wxFileName>& files)
{
    std::vector<FileEntryPtr> f;
    GetFiles(partialName, f);

    for(size_t i = 0; i < f.size(); i++) {
        files.push_back(wxFileName(f.at(i)->GetFile()));
    }
}

TagEntryPtr TagsManager::FunctionFromFileLine(const wxFileName& fileName, int lineno, bool nextFunction /*false*/)
{
    if(!GetDatabase()) { return NULL; }

    if(!IsFileCached(fileName.GetFullPath())) { CacheFile(fileName.GetFullPath()); }

    TagEntryPtr foo = NULL;
    for(size_t i = 0; i < m_cachedFileFunctionsTags.size(); i++) {
        TagEntryPtr t = m_cachedFileFunctionsTags.at(i);

        if(nextFunction && t->GetLine() > lineno) {
            // keep the last non matched method
            foo = t;
        } else if(t->GetLine() <= lineno) {
            if(nextFunction) {
                return foo;
            } else {
                return t;
            }
        }
    }
    return NULL;
}

void TagsManager::GetScopesFromFile(const wxFileName& fileName, std::vector<wxString>& scopes)
{
    if(!GetDatabase()) { return; }

    GetDatabase()->GetScopesFromFileAsc(fileName, scopes);
}

void TagsManager::TagsFromFileAndScope(const wxFileName& fileName, const wxString& scopeName,
                                       std::vector<TagEntryPtr>& tags)
{
    if(!GetDatabase()) { return; }

    wxArrayString kind;
    kind.Add(wxT("function"));
    kind.Add(wxT("prototype"));
    kind.Add(wxT("enum"));

    GetDatabase()->GetTagsByFileScopeAndKind(fileName, scopeName, kind, tags);
    std::sort(tags.begin(), tags.end(), SAscendingSort());
}

bool TagsManager::GetFunctionDetails(const wxFileName& fileName, int lineno, TagEntryPtr& tag, clFunction& func)
{
    tag = FunctionFromFileLine(fileName, lineno);
    if(tag) {
        GetLanguage()->FunctionFromPattern(tag, func);
        return true;
    }
    return false;
}

TagEntryPtr TagsManager::FirstFunctionOfFile(const wxFileName& fileName)
{
    if(!GetDatabase()) { return NULL; }

    std::vector<TagEntryPtr> tags;
    wxArrayString kind;
    kind.Add(wxT("function"));
    GetDatabase()->GetTagsByKindAndFile(kind, fileName.GetFullPath(), wxT("line"), ITagsStorage::OrderAsc, tags);

    if(tags.empty()) return NULL;
    return tags.at(0);
}

TagEntryPtr TagsManager::FirstScopeOfFile(const wxFileName& fileName)
{
    if(!GetDatabase()) { return NULL; }
    std::vector<TagEntryPtr> tags;
    wxArrayString kind;
    kind.Add(wxT("struct"));
    kind.Add(wxT("class"));
    kind.Add(wxT("namespace"));
    GetDatabase()->GetTagsByKindAndFile(kind, fileName.GetFullPath(), wxT("line"), ITagsStorage::OrderAsc, tags);

    if(tags.empty()) return NULL;
    return tags.at(0);
}

wxString TagsManager::FormatFunction(TagEntryPtr tag, size_t flags, const wxString& scope)
{
    clFunction foo;
    if(!GetLanguage()->FunctionFromPattern(tag, foo)) { return wxEmptyString; }

    wxString body;
    // add virtual keyword to declarations only && if the flags is set
    if(foo.m_isVirtual && (flags & FunctionFormat_WithVirtual) && !(flags & FunctionFormat_Impl)) {
        body << wxT("virtual\n");
    }

    if(tag->IsTemplateFunction()) {
        // a template function, add the template definition
        body << "template <";
        CxxTemplateFunction helper(tag);
        helper.ParseDefinitionList();
        for(size_t i = 0; i < helper.GetList().GetCount(); ++i) {
            body << "typename " << helper.GetList().Item(i) << ", \n";
        }
        if(body.EndsWith(", \n")) { body.RemoveLast(3); }
        body << ">\n";
    }

    wxString ret_value = GetFunctionReturnValueFromPattern(tag);
    if(ret_value.IsEmpty() == false) {
        body << ret_value << wxT(" ");

    } else {
        wxString retValue = tag->GetReturnValue();
        if(retValue.IsEmpty() == false) { body << retValue << wxT(" "); }
    }

    if(flags & FunctionFormat_Impl) {
        if(scope.IsEmpty()) {
            if(tag->GetScope() != wxT("<global>")) { body << tag->GetScope() << wxT("::"); }
        } else {
            body << scope << wxT("::");
        }
    }

    // Build the flags required by the NormalizeFunctionSig() method
    size_t tmpFlags(0);
    if(flags & FunctionFormat_Impl) {
        tmpFlags |= Normalize_Func_Name | Normalize_Func_Reverse_Macro;
    } else {
        tmpFlags |= Normalize_Func_Name | Normalize_Func_Reverse_Macro | Normalize_Func_Default_value;
    }

    if(flags & FunctionFormat_Arg_Per_Line) tmpFlags |= Normalize_Func_Arg_Per_Line;

    if(flags & FunctionFormat_Arg_Per_Line) body << wxT("\n");

    body << tag->GetName();
    if(tag->GetFlags() & TagEntry::Tag_No_Signature_Format) {
        body << tag->GetSignature();

    } else {
        body << NormalizeFunctionSig(tag->GetSignature(), tmpFlags);
    }

    if(foo.m_isConst) { body << wxT(" const"); }

    if(!foo.m_throws.empty()) { body << wxT(" throw (") << wxString(foo.m_throws.c_str(), wxConvUTF8) << wxT(")"); }

    if(flags & FunctionFormat_Impl) {
        body << wxT("\n{\n}\n");
    } else {
        body << wxT(";\n");
    }

    // convert \t to spaces
    body.Replace(wxT("\t"), wxT(" "));

    // remove any extra spaces from the tip
    while(body.Replace(wxT("  "), wxT(" "))) {}
    return body;
}

bool TagsManager::IsPureVirtual(TagEntryPtr tag)
{
    clFunction foo;
    if(!GetLanguage()->FunctionFromPattern(tag, foo)) { return false; }
    return foo.m_isPureVirtual;
}

bool TagsManager::IsVirtual(TagEntryPtr tag)
{
    clFunction foo;
    if(!GetLanguage()->FunctionFromPattern(tag, foo)) { return false; }
    return foo.m_isVirtual;
}
void TagsManager::SetLanguage(Language* lang) { m_lang = lang; }

Language* TagsManager::GetLanguage()
{
    if(!m_lang) {
        // for backward compatibility allows access to the tags manager using
        // the singleton call
        return LanguageST::Get();
    } else {
        return m_lang;
    }
}

bool TagsManager::ProcessExpression(const wxString& expression, wxString& type, wxString& typeScope)
{
    wxString oper, dummy;
    return ProcessExpression(wxFileName(), wxNOT_FOUND, expression, wxEmptyString, type, typeScope, oper, dummy);
}

void TagsManager::GetClasses(std::vector<TagEntryPtr>& tags, bool onlyWorkspace)
{
    wxArrayString kind;
    kind.Add(wxT("class"));
    kind.Add(wxT("struct"));
    kind.Add(wxT("union"));

    GetDatabase()->GetTagsByKind(kind, wxT("name"), ITagsStorage::OrderAsc, tags);
}

void TagsManager::StripComments(const wxString& text, wxString& stippedText)
{
    CppScanner scanner;
    scanner.SetText(_C(text));

    bool changedLine = false;
    bool prepLine = false;
    int curline = 0;

    while(true) {
        int type = scanner.yylex();
        if(type == 0) { break; }

        // eat up all tokens until next line
        if(prepLine && scanner.lineno() == curline) { continue; }

        prepLine = false;

        // Get the current line number, it will help us detect preprocessor lines
        changedLine = (scanner.lineno() > curline);
        if(changedLine) { stippedText << wxT("\n"); }

        curline = scanner.lineno();
        if(type == '#') {
            if(changedLine) {
                // We are at the start of a new line
                // consume everything until new line is found or end of text
                prepLine = true;
                continue;
            }
        }
        stippedText << _U(scanner.YYText()) << wxT(" ");
    }
}

void TagsManager::GetFunctions(std::vector<TagEntryPtr>& tags, const wxString& fileName, bool onlyWorkspace)
{
    wxArrayString kind;
    kind.Add(wxT("function"));
    kind.Add(wxT("prototype"));
    GetDatabase()->GetTagsByKindAndFile(kind, fileName, wxT("name"), ITagsStorage::OrderAsc, tags);
}

void TagsManager::GetAllTagsNames(wxArrayString& tagsList)
{
    size_t kind = GetCtagsOptions().GetCcColourFlags();
    if(kind == CC_COLOUR_ALL) {
        GetDatabase()->GetAllTagsNames(tagsList);
        return;
    }

    wxArrayString kindArr;

    if(kind & CC_COLOUR_CLASS) { kindArr.Add(wxT("class")); }
    if(kind & CC_COLOUR_ENUM) { kindArr.Add(wxT("enum")); }
    if(kind & CC_COLOUR_FUNCTION) { kindArr.Add(wxT("function")); }
    if(kind & CC_COLOUR_MACRO) { kindArr.Add(wxT("macro")); }
    if(kind & CC_COLOUR_NAMESPACE) { kindArr.Add(wxT("namespace")); }
    if(kind & CC_COLOUR_PROTOTYPE) { kindArr.Add(wxT("prototype")); }
    if(kind & CC_COLOUR_STRUCT) { kindArr.Add(wxT("struct")); }
    if(kind & CC_COLOUR_TYPEDEF) { kindArr.Add(wxT("typedef")); }
    if(kind & CC_COLOUR_UNION) { kindArr.Add(wxT("union")); }
    if(kind & CC_COLOUR_ENUMERATOR) { kindArr.Add(wxT("enumerator")); }
    if(kind & CC_COLOUR_VARIABLE) { kindArr.Add(wxT("variable")); }
    if(kind & CC_COLOUR_MEMBER) { kindArr.Add(wxT("member")); }

    if(kindArr.IsEmpty()) { return; }

    GetDatabase()->GetTagsNames(kindArr, tagsList);
}

void TagsManager::TagsByScope(const wxString& scopeName, const wxArrayString& kind, std::vector<TagEntryPtr>& tags,
                              bool include_anon)
{
    wxUnusedVar(include_anon);

    wxArrayString scopes;
    GetScopesByScopeName(scopeName, scopes);
    // make enough room for max of 500 elements in the vector
    tags.reserve(500);
    GetDatabase()->GetTagsByScopesAndKind(scopes, kind, tags);

    // and finally sort the results
    std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::TagsByTyperef(const wxString& scopeName, const wxArrayString& kind, std::vector<TagEntryPtr>& tags,
                                bool include_anon)
{
    wxUnusedVar(include_anon);

    wxArrayString scopes;
    GetScopesByScopeName(scopeName, scopes);
    // make enough room for max of 500 elements in the vector
    tags.reserve(500);

    GetDatabase()->GetTagsByTyperefAndKind(scopes, kind, tags);

    // and finally sort the results
    std::sort(tags.begin(), tags.end(), SAscendingSort());
}

wxString TagsManager::NormalizeFunctionSig(const wxString& sig, size_t flags,
                                           std::vector<std::pair<int, int> >* paramLen)
{
    // FIXME: make the standard configurable
    CxxVariableScanner varScanner(sig, eCxxStandard::kCxx03, wxStringTable_t(), true);
    CxxVariable::Vec_t vars = varScanner.ParseFunctionArguments();

    // construct a function signature from the results
    wxString str_output;
    str_output << wxT("(");

    if(paramLen) { paramLen->clear(); }
    if(flags & Normalize_Func_Arg_Per_Line && !vars.empty()) { str_output << wxT("\n    "); }

    const wxStringTable_t& macrosTable = GetCtagsOptions().GetTokensReversedWxMap();
    std::for_each(vars.begin(), vars.end(), [&](CxxVariable::Ptr_t var) {
        int start_offset = str_output.length();

        // FIXME: the standard should be configurable
        size_t toStringFlags = CxxVariable::kToString_None;
        if(flags & Normalize_Func_Name) { toStringFlags |= CxxVariable::kToString_Name; }
        if(flags & Normalize_Func_Default_value) { toStringFlags |= CxxVariable::kToString_DefaultValue; }

        str_output << var->ToString(toStringFlags,
                                    (flags & Normalize_Func_Reverse_Macro) ? macrosTable : wxStringTable_t());
        // keep the length of this argument
        if(paramLen) { paramLen->push_back(std::make_pair(start_offset, str_output.length() - start_offset)); }
        str_output << ", ";
        if((flags & Normalize_Func_Arg_Per_Line) && !vars.empty()) { str_output << wxT("\n    "); }
    });

    if(vars.empty() == false) { str_output = str_output.BeforeLast(','); }

    str_output << ")";
    return str_output;
}

void TagsManager::GetUnImplementedFunctions(const wxString& scopeName, std::map<wxString, TagEntryPtr>& protos)
{
    // get list of all prototype functions from the database
    std::vector<TagEntryPtr> vproto;
    std::vector<TagEntryPtr> vimpl;

    // currently we want to add implementation only for workspace classes
    TagsByScope(scopeName, wxT("prototype"), vproto, false, false);
    TagsByScope(scopeName, wxT("function"), vimpl, false, false);

    // filter out functions which already has implementation
    for(size_t i = 0; i < vproto.size(); i++) {
        TagEntryPtr tag = vproto.at(i);
        wxString key = tag->GetName();

        // override the scope to be our scope...
        tag->SetScope(scopeName);

        key << NormalizeFunctionSig(tag->GetSignature(), Normalize_Func_Reverse_Macro);
        protos[key] = tag;
    }

    // std::map<std::string, std::string> ignoreTokens = GetCtagsOptions().GetTokensMap();

    // remove functions with implementation
    for(size_t i = 0; i < vimpl.size(); i++) {
        TagEntryPtr tag = vimpl.at(i);
        wxString key = tag->GetName();
        key << NormalizeFunctionSig(tag->GetSignature(), Normalize_Func_Reverse_Macro);
        std::map<wxString, TagEntryPtr>::iterator iter = protos.find(key);

        if(iter != protos.end()) { protos.erase(iter); }
    }

    std::map<wxString, TagEntryPtr> tmpMap(protos);
    std::map<wxString, TagEntryPtr>::iterator it = tmpMap.begin();
    protos.clear();

    // collect only non-pure virtual methods
    for(; it != tmpMap.end(); it++) {
        TagEntryPtr tag = it->second;
        clFunction f;
        if(GetLanguage()->FunctionFromPattern(tag, f)) {
            if(!f.m_isPureVirtual) {
                // incude this function
                protos[it->first] = it->second;
            }
        } else {
            // parsing failed
            protos[it->first] = it->second;
        }
    }
}

void TagsManager::CacheFile(const wxString& fileName)
{
    if(!GetDatabase()) { return; }

    m_cachedFile = fileName;
    m_cachedFileFunctionsTags.clear();

    wxArrayString kinds;
    kinds.Add(wxT("function"));
    kinds.Add(wxT("prototype"));
    // disable the cache
    GetDatabase()->SetUseCache(false);
    GetDatabase()->GetTagsByKindAndFile(kinds, fileName, wxT("line"), ITagsStorage::OrderDesc,
                                        m_cachedFileFunctionsTags);
    // re-enable it
    GetDatabase()->SetUseCache(true);
}

void TagsManager::ClearCachedFile(const wxString& fileName)
{
    if(fileName == m_cachedFile) {
        m_cachedFile.Clear();
        m_cachedFileFunctionsTags.clear();
    }
}

bool TagsManager::IsFileCached(const wxString& fileName) const { return fileName == m_cachedFile; }

wxString TagsManager::GetCTagsCmd()
{
    wxString cmd;
    wxString ctagsCmd;
    ctagsCmd << m_tagsOptions.ToString() << m_ctagsCmd;

    // build the command, we surround ctags name with double quatations
    cmd << wxT("\"") << m_codeliteIndexerPath.GetFullPath() << wxT("\"") << ctagsCmd;

    return cmd;
}

wxString TagsManager::DoReplaceMacros(const wxString& name)
{
    // replace macros:
    // replace the provided typeName and scope with user defined macros as appeared in the PreprocessorMap
    wxString _name(name);

    const wxStringTable_t& iTokens = GetCtagsOptions().GetTokensWxMap();
    wxStringTable_t::const_iterator it;

    it = iTokens.find(_name);
    if(it != iTokens.end()) {
        if(it->second.empty() == false) { _name = it->second; }
    }
    return _name;
}

void TagsManager::DeleteTagsByFilePrefix(const wxString& dbfileName, const wxString& filePrefix)
{
    ITagsStorage* db = new TagsStorageSQLite();
    db->OpenDatabase(wxFileName(dbfileName));
    db->Begin();

    // delete the tags
    db->DeleteByFilePrefix(db->GetDatabaseFileName(), filePrefix);

    // deelete the FILES entries
    db->DeleteFromFilesByPrefix(db->GetDatabaseFileName(), filePrefix);
    db->Commit();

    delete db;
}

void TagsManager::UpdateFilesRetagTimestamp(const wxArrayString& files, ITagsStoragePtr db)
{
    db->Begin();
    for(size_t i = 0; i < files.GetCount(); i++) {
        db->InsertFileEntry(files.Item(i), (int)time(NULL));
    }
    db->Commit();
}

void TagsManager::FilterNonNeededFilesForRetaging(wxArrayString& strFiles, ITagsStoragePtr db)
{
    std::vector<FileEntryPtr> files_entries;
    db->GetFiles(files_entries);
    std::unordered_set<wxString> files_set;

    for(size_t i = 0; i < strFiles.GetCount(); i++) {
        files_set.insert(strFiles.Item(i));
    }

    for(size_t i = 0; i < files_entries.size(); i++) {
        FileEntryPtr fe = files_entries.at(i);

        // does the file exist in both lists?
        std::unordered_set<wxString>::iterator iter = files_set.find(fe->GetFile());
        if(iter != files_set.end()) {
            // get the actual modifiaction time of the file from the disk
            struct stat buff;
            int modified(0);

            const wxCharBuffer cname = _C((*iter));
            if(stat(cname.data(), &buff) == 0) { modified = (int)buff.st_mtime; }

            // if the timestamp from the database < then the actual timestamp, re-tag the file
            if(fe->GetLastRetaggedTimestamp() >= modified) { files_set.erase(iter); }
        }
    }

    // copy back the files to the array
    std::unordered_set<wxString>::iterator iter = files_set.begin();
    strFiles.Clear();
    strFiles.Alloc(files_set.size());
    for(; iter != files_set.end(); iter++) {
        strFiles.Add(*iter);
    }
}

void TagsManager::DoFilterNonNeededFilesForRetaging(wxArrayString& strFiles, ITagsStoragePtr db)
{
    FilterNonNeededFilesForRetaging(strFiles, db);
}

wxString TagsManager::GetFunctionReturnValueFromPattern(TagEntryPtr tag)
{
    // evaluate the return value of the tag
    clFunction foo;
    wxString return_value;
    if(GetLanguage()->FunctionFromPattern(tag, foo)) {
        if(foo.m_retrunValusConst.empty() == false) { return_value << _U(foo.m_retrunValusConst.c_str()) << wxT(" "); }

        if(foo.m_returnValue.m_typeScope.empty() == false) {
            return_value << _U(foo.m_returnValue.m_typeScope.c_str()) << wxT("::");
        }

        if(foo.m_returnValue.m_type.empty() == false) {
            return_value << _U(foo.m_returnValue.m_type.c_str());
            if(foo.m_returnValue.m_templateDecl.empty() == false) {
                return_value << wxT("<") << _U(foo.m_returnValue.m_templateDecl.c_str()) << wxT(">");
            }
            return_value << _U(foo.m_returnValue.m_starAmp.c_str());
            return_value << wxT(" ");
        }

        if(!foo.m_returnValue.m_rightSideConst.empty()) { return_value << foo.m_returnValue.m_rightSideConst << " "; }
    }
    return return_value;
}

void TagsManager::GetTagsByKind(std::vector<TagEntryPtr>& tags, const wxArrayString& kind, const wxString& partName)
{
    wxUnusedVar(partName);
    GetDatabase()->GetTagsByKind(kind, wxEmptyString, ITagsStorage::OrderNone, tags);
}

void TagsManager::GetTagsByKindLimit(std::vector<TagEntryPtr>& tags, const wxArrayString& kind, int limit,
                                     const wxString& partName)
{
    GetDatabase()->GetTagsByKindLimit(kind, wxEmptyString, ITagsStorage::OrderNone, limit, partName, tags);
}

void TagsManager::DoGetFunctionTipForEmptyExpression(const wxString& word, const wxString& text,
                                                     std::vector<TagEntryPtr>& tips, bool globalScopeOnly /* = false*/)
{
    std::vector<TagEntryPtr> candidates;
    std::vector<wxString> additionlScopes;

    // we are probably examining a global function, or a scope function
    GetGlobalTags(word, candidates, ExactMatch);

    if(!globalScopeOnly) {
        wxString scopeName = GetLanguage()->GetScopeName(text, &additionlScopes);
        TagsByScopeAndName(scopeName, word, candidates);
        for(size_t i = 0; i < additionlScopes.size(); i++) {
            TagsByScopeAndName(additionlScopes.at(i), word, candidates);
        }
    }
    GetFunctionTipFromTags(candidates, word, tips);
}

void TagsManager::GetUnOverridedParentVirtualFunctions(const wxString& scopeName, bool onlyPureVirtual,
                                                       std::vector<TagEntryPtr>& protos)
{
    std::vector<TagEntryPtr> tags;
    std::map<wxString, TagEntryPtr> parentSignature2tag;
    std::map<wxString, TagEntryPtr> classSignature2tag;

    GetDatabase()->GetTagsByPath(scopeName, tags);
    if(tags.size() != 1) { return; }

    TagEntryPtr classTag = tags.at(0);
    if(classTag->GetKind() != wxT("class") && classTag->GetKind() != wxT("struct")) return;

    // Step 1:
    // ========
    // Compoze a list of all virtual functions from the direct parent(s)
    // class (there could be a multiple inheritance...)
    wxArrayString parents = classTag->GetInheritsAsArrayNoTemplates();
    wxArrayString kind;

    tags.clear();
    kind.Add(wxT("prototype"));
    kind.Add(wxT("function"));
    for(wxArrayString::size_type i = 0; i < parents.GetCount(); i++) {
        GetDatabase()->GetTagsByScopeAndKind(parents.Item(i), kind, tags, false);
    }

    for(wxArrayString::size_type i = 0; i < tags.size(); i++) {
        TagEntryPtr t = tags.at(i);

        // Skip c-tors/d-tors
        if(t->IsDestructor() || t->IsConstructor()) continue;

        if(onlyPureVirtual) {

            // Collect only pure virtual methods
            if(IsPureVirtual(t)) {
                TagEntryPtr t = tags.at(i);
                wxString sig = NormalizeFunctionSig(t->GetSignature(), Normalize_Func_Reverse_Macro);
                sig.Prepend(t->GetName());
                parentSignature2tag[sig] = tags.at(i);
            }

        } else {

            // Collect both virtual and pure virtual
            if(IsVirtual(tags.at(i)) || IsPureVirtual(tags.at(i))) {
                wxString sig = NormalizeFunctionSig(t->GetSignature(), Normalize_Func_Reverse_Macro);
                sig.Prepend(t->GetName());
                parentSignature2tag[sig] = tags.at(i);
            }
        }
    }

    // Step 2:
    // ========
    // Collect a list of function prototypes from the class
    tags.clear();
    GetDatabase()->GetTagsByScopeAndKind(scopeName, kind, tags, false);
    for(size_t i = 0; i < tags.size(); i++) {
        TagEntryPtr t = tags.at(i);
        wxString sig = NormalizeFunctionSig(t->GetSignature(), Normalize_Func_Reverse_Macro);
        sig.Prepend(t->GetName());
        classSignature2tag[sig] = t;
    }

    // Step 3:
    // =======
    // remove any entry from the parent tags which exists in the child tags
    std::map<wxString, TagEntryPtr>::iterator iter = classSignature2tag.begin();
    for(; iter != classSignature2tag.end(); iter++) {
        if(parentSignature2tag.find(iter->first) != parentSignature2tag.end()) {
            // the current signature exists both in the child and the parent,
            // remove it
            parentSignature2tag.erase(iter->first);
        }
    }

    // Step 4:
    // =======
    // parentSignature2tag now contains map of signature/tags of virtual functions which exists
    // in the parent but could not be found in the child
    iter = parentSignature2tag.begin();
    for(; iter != parentSignature2tag.end(); iter++) {
        protos.push_back(iter->second);
    }
}

void TagsManager::ClearTagsCache() { GetDatabase()->ClearCache(); }

void TagsManager::SetProjectPaths(const wxArrayString& paths)
{
    m_projectPaths.Clear();
    m_projectPaths = paths;
}

void TagsManager::GetDereferenceOperator(const wxString& scope, std::vector<TagEntryPtr>& tags)
{
    std::vector<wxString> derivationList;

    // add this scope as well to the derivation list
    wxString _scopeName = DoReplaceMacros(scope);
    derivationList.push_back(_scopeName);
    std::set<wxString> scannedInherits;
    GetDerivationList(_scopeName, NULL, derivationList, scannedInherits);

    // make enough room for max of 500 elements in the vector
    for(size_t i = 0; i < derivationList.size(); i++) {
        wxString tmpScope(derivationList.at(i));
        tmpScope = DoReplaceMacros(tmpScope);

        GetDatabase()->GetDereferenceOperator(tmpScope, tags);
        if(!tags.empty()) {

            // No need to further check
            break;
        }
    }
}

void TagsManager::GetSubscriptOperator(const wxString& scope, std::vector<TagEntryPtr>& tags)
{
    std::vector<wxString> derivationList;

    // add this scope as well to the derivation list
    wxString _scopeName = DoReplaceMacros(scope);
    derivationList.push_back(_scopeName);
    std::set<wxString> scannedInherits;
    GetDerivationList(_scopeName, NULL, derivationList, scannedInherits);

    // make enough room for max of 500 elements in the vector
    for(size_t i = 0; i < derivationList.size(); i++) {
        wxString tmpScope(derivationList.at(i));
        tmpScope = DoReplaceMacros(tmpScope);

        GetDatabase()->GetSubscriptOperator(scope, tags);
        if(!tags.empty()) {

            // No need to further check
            break;
        }
    }
}

void TagsManager::ClearAllCaches()
{
    m_cachedFile.Clear();
    m_cachedFileFunctionsTags.clear();
    GetDatabase()->ClearCache();
}

CppToken TagsManager::FindLocalVariable(const wxFileName& fileName, int pos, int lineNumber, const wxString& word,
                                        const wxString& modifiedText)
{
    // Load the file and get a state map + the text from the scanner
    TagEntryPtr tag(NULL);
    TextStatesPtr states(NULL);
    CppWordScanner scanner;

    if(modifiedText.empty() == false) {
        // Parse the modified text
        std::vector<TagEntryPtr> tags;
        DoParseModifiedText(modifiedText, tags);

        // It is safe to assume that the tags are sorted by line number
        // Loop over the tree and search for the a function closest to the given line number
        for(size_t i = 0; i < tags.size() && tags[i]->GetLine() <= lineNumber; i++) {
            if(tags[i]->IsFunction()) { tag = tags[i]; }
        }

        // Construct a scanner based on the modified text
        scanner = CppWordScanner(fileName.GetFullPath(), modifiedText.mb_str().data(), 0);
        states = scanner.states();

    } else {
        // get the local by scanning from the current function's
        tag = FunctionFromFileLine(fileName, lineNumber + 1);
        scanner = CppWordScanner(fileName.GetFullPath().mb_str().data());
        states = scanner.states();
    }

    if(!tag || !states) return CppToken();

    // Get the line number of the function
    int funcLine = tag->GetLine() - 1;

    // Convert the line number to offset
    int from = states->LineToPos(funcLine);
    int to = states->FunctionEndPos(from);

    if(to == wxNOT_FOUND) return CppToken();

    // get list of variables from the given scope
    CxxVariableScanner varscanner(states->text, eCxxStandard::kCxx11, GetCtagsOptions().GetTokensWxMap(), false);
    CxxVariable::Map_t varsMap = varscanner.GetVariablesMap();

    bool isLocalVar = (varsMap.count(word) != 0);
    if(!isLocalVar) return CppToken();

    // search for matches in the given range
    CppTokensMap l;
    scanner.Match(word.mb_str().data(), l, from, to);

    std::vector<CppToken> tokens;
    l.findTokens(word.mb_str().data(), tokens);
    if(tokens.empty()) return CppToken();

    // return the first match
    return *tokens.begin();
}

void TagsManager::DoParseModifiedText(const wxString& text, std::vector<TagEntryPtr>& tags)
{
    wxFFile fp;
    wxString fileName = wxFileName::CreateTempFileName(wxT("codelite_mod_file_"), &fp);
    if(fp.IsOpened()) {
        fp.Write(text);
        fp.Close();
        wxString tagsStr;
        SourceToTags(wxFileName(fileName), tagsStr);

        // Create tags from the string
        wxArrayString tagsLines = wxStringTokenize(tagsStr, wxT("\n"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < tagsLines.GetCount(); i++) {
            wxString line = tagsLines.Item(i).Trim().Trim(false);
            if(line.IsEmpty()) continue;

            TagEntryPtr tag(new TagEntry());
            tag->FromLine(line);

            tags.push_back(tag);
        }
        // Delete the modified file
        wxRemoveFile(fileName);
    }
}

bool TagsManager::IsBinaryFile(const wxString& filepath)
{
    // If the file is a C++ file, avoid testing the content return false based on the extension
    FileExtManager::FileType type = FileExtManager::GetType(filepath);
    if(type == FileExtManager::TypeHeader || type == FileExtManager::TypeSourceC ||
       type == FileExtManager::TypeSourceCpp)
        return false;

    // examine the file based on the content of the first 4K (max) bytes
    FILE* fp = fopen(filepath.To8BitData(), "rb");
    if(fp) {

        char buffer[1];
        int textLen(0);
        const int maxTextToExamine(4096);

        // examine up to maxTextToExamine first chars in the file and search for '\0'
        while(fread(buffer, sizeof(char), sizeof(buffer), fp) == 1 && textLen < maxTextToExamine) {
            textLen++;
            // if we found a NULL, return true
            if(buffer[0] == 0) {
                fclose(fp);
                return true;
            }
        }

        fclose(fp);
        return false;
    }

    // if we could not open it, return true
    return true;
}

wxString TagsManager::WrapLines(const wxString& str)
{
    wxString wrappedString;

    int curLineBytes(0);
    wxString::const_iterator iter = str.begin();
    for(; iter != str.end(); iter++) {
        if(*iter == wxT('\t')) {
            wrappedString << wxT(" ");

        } else if(*iter == wxT('\n')) {
            wrappedString << wxT("\n");
            curLineBytes = 0;

        } else if(*iter == wxT('\r')) {
            // Skip it

        } else {
            wrappedString << *iter;
        }
        curLineBytes++;

        if(curLineBytes == MAX_TIP_LINE_SIZE) {

            // Wrap the lines
            if(wrappedString.IsEmpty() == false && wrappedString.Last() != wxT('\n')) { wrappedString << wxT("\n"); }
            curLineBytes = 0;
        }
    }
    return wrappedString;
}

void TagsManager::SetEncoding(const wxFontEncoding& encoding) { m_encoding = encoding; }

wxArrayString TagsManager::BreakToOuterScopes(const wxString& scope)
{
    wxArrayString outerScopes;
    wxArrayString scopes = wxStringTokenize(scope, wxT(":"), wxTOKEN_STRTOK);
    for(size_t i = 1; i < scopes.GetCount(); i++) {
        wxString newScope;
        for(size_t j = 0; j < i; j++) {
            newScope << scopes.Item(j) << wxT("::");
        }
        if(newScope.Len() >= 2) { newScope.RemoveLast(2); }
        outerScopes.Add(newScope);
    }
    return outerScopes;
}

ITagsStoragePtr TagsManager::GetDatabase() { return m_db; }

void TagsManager::GetTagsByName(const wxString& prefix, std::vector<TagEntryPtr>& tags)
{
    GetDatabase()->GetTagsByName(prefix, tags);
}

wxString TagsManager::DoReplaceMacrosFromDatabase(const wxString& name)
{
    std::set<wxString> scannedMacros;
    wxString newName = name;
    while(true) {
        TagEntryPtr matchedTag = GetDatabase()->GetTagsByNameLimitOne(newName);
        if(matchedTag && matchedTag->IsMacro() && scannedMacros.find(matchedTag->GetName()) == scannedMacros.end()) {
            TagEntryPtr realTag = matchedTag->ReplaceSimpleMacro();
            if(realTag) {

                newName = realTag->GetName();
                scannedMacros.insert(newName);
                continue;

            } else {
                break;
            }
        } else {
            break;
        }
    }
    return newName;
}

void TagsManager::GetTagsByPartialName(const wxString& partialName, std::vector<TagEntryPtr>& tags)
{
    GetDatabase()->GetTagsByPartName(partialName, tags);
}

bool TagsManager::AreTheSame(const TagEntryPtrVector_t& v1, const TagEntryPtrVector_t& v2) const
{
    // Assuming that v1 and v2 are sorted!
    if(v1.size() != v2.size()) return false;
    for(size_t i = 0; i < v1.size(); i++) {
        if(v1.at(i)->CompareDisplayString(v2.at(i)) != 0) return false;
    }
    return true;
}

bool TagsManager::InsertFunctionDecl(const wxString& clsname, const wxString& functionDecl, wxString& sourceContent,
                                     int visibility)
{
    return GetLanguage()->InsertFunctionDecl(clsname, functionDecl, sourceContent, visibility);
}

void TagsManager::InsertFunctionImpl(const wxString& clsname, const wxString& functionImpl, const wxString& filename,
                                     wxString& sourceContent, int& insertedLine)
{
    return GetLanguage()->InsertFunctionImpl(clsname, functionImpl, filename, sourceContent, insertedLine);
}

void TagsManager::DoSortByVisibility(TagEntryPtrVector_t& tags)
{
    TagEntryPtrVector_t publicTags, privateTags, protectedTags;
    for(size_t i = 0; i < tags.size(); ++i) {

        TagEntryPtr tag = tags.at(i);
        wxString access = tag->GetAccess();

        if(access == "private") {
            privateTags.push_back(tag);

        } else if(access == "protected") {
            protectedTags.push_back(tag);

        } else if(access == "public") {
            publicTags.push_back(tag);

        } else {
            // assume private
            privateTags.push_back(tag);
        }
    }

    std::sort(privateTags.begin(), privateTags.end(), SAscendingSort());
    std::sort(publicTags.begin(), publicTags.end(), SAscendingSort());
    std::sort(protectedTags.begin(), protectedTags.end(), SAscendingSort());
    tags.clear();
    tags.insert(tags.end(), publicTags.begin(), publicTags.end());
    tags.insert(tags.end(), protectedTags.begin(), protectedTags.end());
    tags.insert(tags.end(), privateTags.begin(), privateTags.end());
}

void TagsManager::GetScopesByScopeName(const wxString& scopeName, wxArrayString& scopes)
{
    std::vector<wxString> derivationList;

    // add this scope as well to the derivation list
    wxString _scopeName = DoReplaceMacros(scopeName);
    derivationList.push_back(_scopeName);
    std::set<wxString> scannedInherits;
    GetDerivationList(_scopeName, NULL, derivationList, scannedInherits);

    for(size_t i = 0; i < derivationList.size(); i++) {
        wxString tmpScope(derivationList.at(i));
        tmpScope = DoReplaceMacros(tmpScope);
        scopes.Add(tmpScope);
    }
}

void TagsManager::InsertForwardDeclaration(const wxString& classname, const wxString& fileContent, wxString& lineToAdd,
                                           int& line, const wxString& impExpMacro)
{
    lineToAdd << "class ";
    if(!impExpMacro.IsEmpty()) { lineToAdd << impExpMacro << " "; }
    lineToAdd << classname << ";";
    line = GetLanguage()->GetBestLineForForwardDecl(fileContent);
}

void TagsManager::GetFilesForCC(const wxString& userTyped, wxArrayString& matches)
{
    GetDatabase()->GetFilesForCC(userTyped, matches);
}

void TagsManager::GetCXXKeywords(wxStringSet_t& words)
{
    wxArrayString arr;
    GetCXXKeywords(arr);
    words.clear();
    words.insert(arr.begin(), arr.end());
}

void TagsManager::GetCXXKeywords(wxArrayString& words)
{
    words.Clear();
    words.Add("alignas");
    words.Add("alignof");
    words.Add("and");
    words.Add("and_eq");
    words.Add("asm");
    words.Add("auto");
    words.Add("bitand");
    words.Add("bitor");
    words.Add("bool");
    words.Add("break");
    words.Add("case");
    words.Add("catch");
    words.Add("char");
    words.Add("char16_t");
    words.Add("char32_t");
    words.Add("class");
    words.Add("compl");
    words.Add("const");
    words.Add("constexpr");
    words.Add("const_cast");
    words.Add("continue");
    words.Add("decltype");
    words.Add("default");
    words.Add("delete");
    words.Add("do");
    words.Add("double");
    words.Add("dynamic_cast");
    words.Add("else");
    words.Add("enum");
    words.Add("explicit");
    words.Add("export");
    words.Add("extern");
    words.Add("false");
    words.Add("final");
    words.Add("float");
    words.Add("for");
    words.Add("friend");
    words.Add("goto");
    words.Add("if");
    words.Add("inline");
    words.Add("int");
    words.Add("long");
    words.Add("mutable");
    words.Add("namespace");
    words.Add("new");
    words.Add("noexcept");
    words.Add("not");
    words.Add("not_eq");
    words.Add("nullptr");
    words.Add("operator");
    words.Add("or");
    words.Add("or_eq");
    words.Add("override");
    words.Add("private");
    words.Add("protected");
    words.Add("public");
    words.Add("register");
    words.Add("reinterpret_cast");
    words.Add("return");
    words.Add("short");
    words.Add("signed");
    words.Add("sizeof");
    words.Add("static");
    words.Add("static_assert");
    words.Add("static_cast");
    words.Add("struct");
    words.Add("switch");
    words.Add("template");
    words.Add("this");
    words.Add("thread_local");
    words.Add("throw");
    words.Add("true");
    words.Add("try");
    words.Add("typedef");
    words.Add("typeid");
    words.Add("typename");
    words.Add("union");
    words.Add("unsigned");
    words.Add("using");
    words.Add("virtual");
    words.Add("void");
    words.Add("volatile");
    words.Add("wchar_t");
    words.Add("while");
    words.Add("xor");
    words.Add("xor_eq");
}

TagEntryPtrVector_t TagsManager::ParseBuffer(const wxString& content, const wxString& filename)
{
    if(!m_codeliteIndexerProcess) { return TagEntryPtrVector_t(); }

    // Write the content into temporary file
    wxString tmpfilename = wxFileName::CreateTempFileName("ctagstemp");
    wxFFile fp(tmpfilename, "w+b");
    if(!fp.IsOpened()) return TagEntryPtrVector_t();
    fp.Write(content, wxConvUTF8);
    fp.Close();

    wxString tags;
    SourceToTags(tmpfilename, tags);

    {
        wxLogNull noLog;
        ::wxRemoveFile(tmpfilename);
    }

    TagEntryPtrVector_t tagsVec;
    wxArrayString lines = ::wxStringTokenize(tags, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        wxString& line = lines.Item(i);
        line.Trim().Trim(false);
        if(line.IsEmpty()) continue;

        TagEntryPtr tag(new TagEntry());
        tag->FromLine(line);

        // If the caller provided a filename, set it
        if(!filename.IsEmpty()) { tag->SetFile(filename); }

        if(tag->GetKind() != "local") { tagsVec.push_back(tag); }
    }
    return tagsVec;
}

void TagsManager::GetKeywordsTagsForLanguage(const wxString& filter, eLanguage lang, std::vector<TagEntryPtr>& tags)
{
    wxString keywords;
    if(lang == kCxx) {
        keywords = wxT(" alignas"
                       " alignof"
                       " and"
                       " and_eq"
                       " asm"
                       " auto"
                       " bitand"
                       " bitor"
                       " bool"
                       " break"
                       " case"
                       " catch"
                       " char"
                       " char16_t"
                       " char32_t"
                       " class"
                       " compl"
                       " concept"
                       " const"
                       " constexpr"
                       " const_cast"
                       " continue"
                       " decltype"
                       " default"
                       " delete"
                       " do"
                       " double"
                       " dynamic_cast"
                       " else"
                       " enum"
                       " explicit"
                       " export"
                       " extern"
                       " false"
                       " final"
                       " float"
                       " for"
                       " friend"
                       " goto"
                       " if"
                       " inline"
                       " int"
                       " long"
                       " mutable"
                       " namespace"
                       " new"
                       " noexcept"
                       " not"
                       " not_eq"
                       " nullptr"
                       " once"
                       " operator"
                       " or"
                       " or_eq"
                       " override"
                       " private"
                       " protected"
                       " public"
                       " register"
                       " reinterpret_cast"
                       " requires"
                       " return"
                       " short"
                       " signed"
                       " sizeof"
                       " static"
                       " static_assert"
                       " static_cast"
                       " struct"
                       " switch"
                       " template"
                       " this"
                       " thread_local"
                       " throw"
                       " true"
                       " try"
                       " typedef"
                       " typeid"
                       " typename"
                       " union"
                       " unsigned"
                       " using"
                       " virtual"
                       " void"
                       " volatile"
                       " wchar_t"
                       " while"
                       " xor"
                       " xor_eq");
    } else if(lang == kJavaScript) {
        keywords = "abstract boolean break byte case catch char class "
                   "const continue debugger default delete do double else enum export extends "
                   "final finally float for function goto if implements import in instanceof "
                   "int interface long native new package private protected public "
                   "return short static super switch synchronized this throw throws "
                   "transient try typeof var void volatile while with";
    }

    std::set<wxString> uniqueWords;
    wxArrayString wordsArr = wxStringTokenize(keywords, wxT(" \r\t\n"));
    uniqueWords.insert(wordsArr.begin(), wordsArr.end());
    std::set<wxString>::iterator iter = uniqueWords.begin();
    for(; iter != uniqueWords.end(); ++iter) {
        if(iter->Contains(filter)) {
            TagEntryPtr tag(new TagEntry());
            tag->SetName(*iter);
            tag->SetKind(wxT("cpp_keyword"));
            tags.push_back(tag);
        }
    }
}

void TagsManager::DoFilterCtorDtorIfNeeded(std::vector<TagEntryPtr>& tags, const wxString& oper)
{
    if((oper == "->") || (oper == ".")) {
        // filter out the constructors / destructors
        std::vector<TagEntryPtr> candidatesNoCtorDtor;
        candidatesNoCtorDtor.reserve(tags.size());
        std::for_each(tags.begin(), tags.end(), [&](TagEntryPtr tag) {
            if(!tag->IsConstructor() && !tag->IsDestructor()) { candidatesNoCtorDtor.push_back(tag); }
        });
        tags.swap(candidatesNoCtorDtor);
    }
}

void TagsManager::GetTagsByPartialNames(const wxArrayString& partialNames, std::vector<TagEntryPtr>& tags)
{
    GetDatabase()->GetTagsByPartName(partialNames, tags);
}
