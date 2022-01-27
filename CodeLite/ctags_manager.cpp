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
#include "ctags_manager.h"

#include "CTags.hpp"
#include "CxxTemplateFunction.h"
#include "CxxVariable.h"
#include "CxxVariableScanner.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include "cl_standard_paths.h"
#include "code_completion_api.h"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "cpp_comment_creator.h"
#include "cppwordscanner.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
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
#if wxUSE_GUI
#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>
#endif
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

//#define __PERFORMANCE
#include "performance.h"

//---------------------------------------------------------------------------
// Misc

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
    if(gs_TagsManager) {
        delete gs_TagsManager;
    }
    gs_TagsManager = NULL;
}

TagsManager* TagsManagerST::Get()
{
    if(gs_TagsManager == NULL)
        gs_TagsManager = new TagsManager();

    return gs_TagsManager;
}

//------------------------------------------------------------------------------
// CTAGS Manager
//------------------------------------------------------------------------------

TagsManager::TagsManager()
    : wxEvtHandler()
    , m_lang(NULL)
    , m_evtHandler(NULL)
    , m_encoding(wxFONTENCODING_DEFAULT)
{
    m_db = new TagsStorageSQLite();
    m_db->SetSingleSearchLimit(MAX_SEARCH_LIMIT);

    // CPP keywords that are usually followed by open brace '('
    m_CppIgnoreKeyWords.insert(wxT("while"));
    m_CppIgnoreKeyWords.insert(wxT("if"));
    m_CppIgnoreKeyWords.insert(wxT("for"));
    m_CppIgnoreKeyWords.insert(wxT("switch"));
}

TagsManager::~TagsManager() {}

void TagsManager::OpenDatabase(const wxFileName& fileName)
{
    m_dbFile = fileName;
    m_db->OpenDatabase(m_dbFile);
    m_db->SetEnableCaseInsensitive(true);
    m_db->SetSingleSearchLimit(m_tagsOptions.GetCcNumberOfDisplayItems());
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

//---------------------------------------------------------------------
// Parsing
//---------------------------------------------------------------------
TagTreePtr TagsManager::TreeFromTags(const wxArrayString& tags, int& count)
{
    // Load the records and build a language tree
    TagEntry root;
    root.SetName(wxT("<ROOT>"));

    TagTreePtr tree(new TagTree(wxT("<ROOT>"), root));

    for(const wxString& line : tags) {
        TagEntry tag;

        // Construct the tag from the line
        tag.FromLine(line);

        // Add the tag to the tree, locals are not added to the
        // tree
        if(tag.GetKind() != wxT("local")) {
            count++;
            tree->AddEntry(tag);
        }
    }
    return tree;
}

TagTreePtr TagsManager::TreeFromTags(const wxString& tags, int& count)
{
    // Load the records and build a language tree
    wxArrayString lines = ::wxStringTokenize(tags, "\r\n", wxTOKEN_STRTOK);
    return TreeFromTags(lines, count);
}

bool TagsManager::IsValidCtagsFile(const wxFileName& filename) const
{
    wxLogNull PreventMissingFileLogErrorMessages;
    return FileExtManager::IsCxxFile(filename) || FileUtils::WildMatch(m_tagsOptions.GetFileSpec(), filename);
}

//-----------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>> Code Completion API START
//-----------------------------------------------------------------------------

void TagsManager::TagsByScopeAndName(const wxString& scope, const wxString& name, std::vector<TagEntryPtr>& tags,
                                     size_t flags)
{
    std::vector<std::pair<wxString, int>> derivationList;
    // add this scope as well to the derivation list

    wxString _scopeName = DoReplaceMacros(scope);
    derivationList.push_back({ _scopeName, 0 });
    std::unordered_set<wxString> visited;
    GetDerivationList(_scopeName, NULL, derivationList, visited, 1);

    // make enough room for max of 500 elements in the vector
    tags.reserve(500);
    wxArrayString scopes;

    for(size_t i = 0; i < derivationList.size(); i++) {
        // try the worksapce database for match
        scopes.Add(derivationList.at(i).first);
    }

    GetDatabase()->GetTagsByScopeAndName(scopes, name, flags & PartialMatch, tags);

    // and finally sort the results
    std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::TagsByScope(const wxString& scope, std::vector<TagEntryPtr>& tags)
{
    std::vector<std::pair<wxString, int>> derivationList;
    // add this scope as well to the derivation list
    wxString _scopeName = DoReplaceMacros(scope);
    derivationList.push_back({ _scopeName, 0 });
    std::unordered_set<wxString> visited;
    GetDerivationList(_scopeName, NULL, derivationList, visited, 1);

    // make enough room for max of 500 elements in the vector
    tags.reserve(500);

    for(size_t i = 0; i < derivationList.size(); i++) {
        wxString tmpScope(derivationList.at(i).first);
        tmpScope = DoReplaceMacros(tmpScope);

        // try the external database for match
        GetDatabase()->GetTagsByScope(derivationList.at(i).first, tags);
    }

    // and finally sort the results
    std::sort(tags.begin(), tags.end(), SAscendingSort());
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
        if(others.find(iter->first) == others.end()) {
            others[iter->first] = iter->second;
        }
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
            if(localTags.find(t->GetName()) == localTags.end()) {
                localTags[t->GetName()] = t;
            }

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
        if(raw_sig.empty() == false) {
            sig = NormalizeFunctionSig(raw_sig, 0);
        }

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
    tmp.Trim().Trim(false);
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
    } else if(expression == "::") {
        // global scope resolution operator

        GetGlobalTags(word, candidates, ExactMatch);
        TipsFromTags(candidates, word, tips);
    } else {
        wxString typeName, typeScope;
        wxString oper, dummy;
        bool res = ProcessExpression(fileName, lineno, expression, text, typeName, typeScope, oper, dummy);
        if(!res) {
            return;
        }

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

void TagsManager::TryReducingScopes(const wxString& scope, const wxString& word, bool imp,
                                    std::vector<TagEntryPtr>& tags)
{
    if(scope == wxT("<global>") || scope.IsEmpty())
        return;

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
        if(newScope.Len() >= 2) {
            newScope.RemoveLast(2);
        }
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

void TagsManager::DeleteFilesTags(const std::vector<wxFileName>& projectFiles) { wxUnusedVar(projectFiles); }

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
        std::vector<std::pair<wxString, int>> derivationList;
        derivationList.push_back({ scope, 0 });
        std::unordered_set<wxString> visited;
        GetDerivationList(scope, NULL, derivationList, visited, 1);
        wxArrayString paths;
        for(size_t i = 0; i < derivationList.size(); i++) {
            wxString path_;
            path_ << derivationList.at(i).first << wxT("::") << name;
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
    if(iter != m_typeScopeContainerCache.end()) {
        return iter->second;
    }

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
    if(iter != m_typeScopeCache.end()) {
        return iter->second;
    }

    // First try the fast query to save some time
    if(GetDatabase()->IsTypeAndScopeExistLimitOne(typeName, scope)) {
        return true;
    }

    // replace macros:
    // replace the provided typeName and scope with user defined macros as appeared in the PreprocessorMap
    typeName = DoReplaceMacros(typeName);
    scope = DoReplaceMacros(scope);

    return GetDatabase()->IsTypeAndScopeExist(typeName, scope);
}

bool TagsManager::GetDerivationList(const wxString& path, TagEntryPtr derivedClassTag,
                                    std::vector<std::pair<wxString, int>>& derivationList,
                                    std::unordered_set<wxString>& visited, int depth)
{
    bool res = GetDerivationListInternal(path, derivedClassTag, derivationList, visited, depth);
    std::sort(
        derivationList.begin(), derivationList.end(),
        [](const std::pair<wxString, size_t>& a, const std::pair<wxString, size_t>& b) { return a.second < b.second; });
    return res;
}

bool TagsManager::GetDerivationListInternal(const wxString& path, TagEntryPtr derivedClassTag,
                                            std::vector<std::pair<wxString, int>>& derivationList,
                                            std::unordered_set<wxString>& visited, int depth)
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

                    if(possibleScope != wxT("<global>")) {
                        inherits = possibleScope + wxT("::") + inherits;
                    }
                }

                // Make sure that this parent was not scanned already
                if(visited.count(inherits) == 0) {
                    visited.insert(inherits);
                    derivationList.push_back({ inherits, depth });
                    GetDerivationList(inherits, tag, derivationList, visited, depth + 1);
                }
            }
        }
    }
    return true;
}

void TagsManager::TipsFromTags(const std::vector<TagEntryPtr>& tags, const wxString& word, std::vector<wxString>& tips)
{
}

void TagsManager::GetFunctionTipFromTags(const std::vector<TagEntryPtr>& tags, const wxString& word,
                                         std::vector<TagEntryPtr>& tips)
{
    std::map<wxString, TagEntryPtr> tipsMap;
    std::vector<TagEntryPtr> ctor_tags;

    for(size_t i = 0; i < tags.size(); i++) {
        if(tags.at(i)->GetName() != word)
            continue;

        TagEntryPtr t;
        TagEntryPtr curtag = tags.at(i);

        // try to replace the current tag with a macro replacement.
        // we dont temper with 'curtag' content since we dont want
        // to modify cached items
        t = curtag->ReplaceSimpleMacro();
        if(!t) {
            t = curtag;
        }

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
    m_db->SetUseCache(false);
}

DoxygenComment TagsManager::GenerateDoxygenComment(const wxString& file, const int line, wxChar keyPrefix)
{
    if(GetDatabase()->IsOpen()) {
        TagEntryPtr tag = GetDatabase()->GetTagAboveFileAndLine(file, line);
        if(!tag) {
            return DoxygenComment();
        }

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
    m_parseComments = m_tagsOptions.GetFlags() & CC_PARSE_COMMENTS ? true : false;
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
    std::vector<std::pair<wxString, int>> derivationList;
    // add this scope as well to the derivation list
    derivationList.push_back({ scopeName, 0 });
    std::unordered_set<wxString> visited;
    if(includeInherits) {
        GetDerivationList(scopeName, NULL, derivationList, visited, 1);
    }

    // make enough room for max of 500 elements in the vector
    tags.reserve(500);
    wxArrayString kinds, scopes;
    kinds.Add(kind);

    for(size_t i = 0; i < derivationList.size(); i++) {
        scopes.Add(derivationList.at(i).first);
    }

    clDEBUG() << "Reading symbols from scopes:" << scopes << endl;
    if(applyLimit) {
        GetDatabase()->GetTagsByScopesAndKind(scopes, kinds, tags);
    } else {
        GetDatabase()->GetTagsByScopesAndKindNoLimit(scopes, kinds, tags);
    }
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
    if(GetDatabase()) {
        GetDatabase()->GetFiles(partialName, files);
    }
}

void TagsManager::GetFiles(const wxString& partialName, std::vector<wxFileName>& files)
{
    std::vector<FileEntryPtr> f;
    GetFiles(partialName, f);

    for(size_t i = 0; i < f.size(); i++) {
        files.push_back(wxFileName(f.at(i)->GetFile()));
    }
}

TagEntryPtr TagsManager::FunctionFromBufferLine(const wxString& buffer, int lineno, const wxString& file_name)
{
    auto tags = ParseBuffer(buffer, file_name);
    if(tags.empty()) {
        return nullptr;
    }

    TagEntryPtr matched_tag;
    for(TagEntryPtr tag : tags) {
        if(tag->IsMethod() && tag->GetLine() <= lineno) {
            matched_tag = tag;
        } else if(tag->GetLine() > lineno) {
            break;
        }
    }
    return matched_tag;
}

TagEntryPtr TagsManager::FunctionFromFileLine(const wxFileName& fileName, int lineno, bool nextFunction /*false*/)
{
    if(!GetDatabase()) {
        return NULL;
    }

    if(!IsFileCached(fileName.GetFullPath())) {
        CacheFile(fileName.GetFullPath());
    }

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
    return foo;
}

void TagsManager::GetScopesFromFile(const wxFileName& fileName, std::vector<wxString>& scopes)
{
    if(!GetDatabase()) {
        return;
    }

    GetDatabase()->GetScopesFromFileAsc(fileName, scopes);
}

void TagsManager::TagsFromFileAndScope(const wxFileName& fileName, const wxString& scopeName,
                                       std::vector<TagEntryPtr>& tags)
{
    if(!GetDatabase()) {
        return;
    }

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
    if(!GetDatabase()) {
        return NULL;
    }

    std::vector<TagEntryPtr> tags;
    wxArrayString kind;
    kind.Add(wxT("function"));
    GetDatabase()->GetTagsByKindAndFile(kind, fileName.GetFullPath(), wxT("line"), ITagsStorage::OrderAsc, tags);

    if(tags.empty())
        return NULL;
    return tags.at(0);
}

TagEntryPtr TagsManager::FirstScopeOfFile(const wxFileName& fileName)
{
    if(!GetDatabase()) {
        return NULL;
    }
    std::vector<TagEntryPtr> tags;
    wxArrayString kind;
    kind.Add(wxT("struct"));
    kind.Add(wxT("class"));
    kind.Add(wxT("namespace"));
    GetDatabase()->GetTagsByKindAndFile(kind, fileName.GetFullPath(), wxT("line"), ITagsStorage::OrderAsc, tags);

    if(tags.empty())
        return NULL;
    return tags.at(0);
}

wxString TagsManager::FormatFunction(TagEntryPtr tag, size_t flags, const wxString& scope)
{
    clFunction foo;
    if(!GetLanguage()->FunctionFromPattern(tag, foo)) {
        return wxEmptyString;
    }

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
            body << "  typename " << helper.GetList().Item(i) << ", \n";
        }
        if(body.EndsWith(", \n")) {
            body.RemoveLast(3);
        }
        body << ">\n";
    }

    wxString retValue = tag->GetTypename();
    if(retValue.IsEmpty() == false) {
        body << retValue << wxT(" ");
    }

    if(flags & FunctionFormat_Impl) {
        if(scope.IsEmpty()) {
            if(tag->GetScope() != wxT("<global>")) {
                body << tag->GetScope() << wxT("::");
            }
        } else {
            body << scope << wxT("::");
        }
    }

    // Build the flags required by the NormalizeFunctionSig() method
    size_t tmpFlags(0);
    if(flags & FunctionFormat_Impl) {
        tmpFlags |= Normalize_Func_Name;
    } else {
        tmpFlags |= Normalize_Func_Name | Normalize_Func_Default_value;
    }

    if(flags & FunctionFormat_Arg_Per_Line)
        tmpFlags |= Normalize_Func_Arg_Per_Line;

    if(flags & FunctionFormat_Arg_Per_Line)
        body << wxT("\n");

    body << tag->GetName();
    if(tag->GetFlags() & TagEntry::Tag_No_Signature_Format) {
        body << tag->GetSignature();

    } else {
        body << NormalizeFunctionSig(tag->GetSignature(), tmpFlags);
    }

    if(foo.m_isConst) {
        body << wxT(" const");
    }

    if(!foo.m_throws.empty()) {
        body << wxT(" throw (") << wxString(foo.m_throws.c_str(), wxConvUTF8) << wxT(")");
    }

    if(flags & FunctionFormat_Impl) {
        body << wxT("\n{\n}\n");
    } else {
        if(foo.m_isVirtual && (flags & FunctionFormat_WithOverride)) {
            body << wxT(" override");
        }
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
    if(!GetLanguage()->FunctionFromPattern(tag, foo)) {
        return false;
    }
    return foo.m_isPureVirtual;
}

bool TagsManager::IsVirtual(TagEntryPtr tag)
{
    clFunction foo;
    if(!GetLanguage()->FunctionFromPattern(tag, foo)) {
        return false;
    }
    return foo.m_isVirtual;
}

bool TagsManager::GetVirtualProperty(TagEntryPtr tag, bool& isVirtual, bool& isPureVirtual, bool& isFinal)
{
    clFunction foo;
    if(!GetLanguage()->FunctionFromPattern(tag, foo)) {
        return false;
    }
    isVirtual = foo.m_isVirtual;
    isPureVirtual = foo.m_isPureVirtual;
    isFinal = foo.m_isFinal;
    return true;
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
        if(type == 0) {
            break;
        }

        // eat up all tokens until next line
        if(prepLine && scanner.lineno() == curline) {
            continue;
        }

        prepLine = false;

        // Get the current line number, it will help us detect preprocessor lines
        changedLine = (scanner.lineno() > curline);
        if(changedLine) {
            stippedText << wxT("\n");
        }

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
                                           std::vector<std::pair<int, int>>* paramLen)
{
    // FIXME: make the standard configurable
    CxxVariableScanner varScanner(sig, eCxxStandard::kCxx11, {}, true);
    CxxVariable::Vec_t vars = varScanner.ParseFunctionArguments();

    // construct a function signature from the results
    wxString str_output;
    str_output << wxT("(");

    if(paramLen) {
        paramLen->clear();
    }
    if(flags & Normalize_Func_Arg_Per_Line && !vars.empty()) {
        str_output << wxT("\n    ");
    }

    for(const auto& var : vars) {
        int start_offset = str_output.length();

        // FIXME: the standard should be configurable
        size_t toStringFlags = CxxVariable::kToString_None;
        if(flags & Normalize_Func_Name) {
            toStringFlags |= CxxVariable::kToString_Name;
        }
        if(flags & Normalize_Func_Default_value) {
            toStringFlags |= CxxVariable::kToString_DefaultValue;
        }

        str_output << var->ToString(toStringFlags, {});
        // keep the length of this argument
        if(paramLen) {
            paramLen->push_back({ start_offset, str_output.length() - start_offset });
        }

        str_output << ", ";
        if((flags & Normalize_Func_Arg_Per_Line) && !vars.empty()) {
            str_output << wxT("\n    ");
        }
    }

    if(vars.empty() == false) {
        str_output = str_output.BeforeLast(',');
    }

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

        key << NormalizeFunctionSig(tag->GetSignature(), 0);
        protos[key] = tag;
    }

    // std::map<std::string, std::string> ignoreTokens = GetCtagsOptions().GetTokensMap();

    // remove functions with implementation
    for(size_t i = 0; i < vimpl.size(); i++) {
        TagEntryPtr tag = vimpl.at(i);
        wxString key = tag->GetName();
        key << NormalizeFunctionSig(tag->GetSignature(), 0);
        std::map<wxString, TagEntryPtr>::iterator iter = protos.find(key);

        if(iter != protos.end()) {
            protos.erase(iter);
        }
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
    if(!GetDatabase()) {
        return;
    }

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

wxString TagsManager::DoReplaceMacros(const wxString& name)
{
    // replace macros:
    // replace the provided typeName and scope with user defined macros as appeared in the PreprocessorMap
    wxString _name(name);

    const wxStringTable_t& iTokens = GetCtagsOptions().GetTokensWxMap();
    wxStringTable_t::const_iterator it;

    it = iTokens.find(_name);
    if(it != iTokens.end()) {
        if(it->second.empty() == false) {
            _name = it->second;
        }
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
            if(stat(cname.data(), &buff) == 0) {
                modified = (int)buff.st_mtime;
            }

            // if the timestamp from the database < then the actual timestamp, re-tag the file
            if(fe->GetLastRetaggedTimestamp() >= modified) {
                files_set.erase(iter);
            }
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
        if(foo.m_retrunValusConst.empty() == false) {
            return_value << _U(foo.m_retrunValusConst.c_str()) << wxT(" ");
        }

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

        if(!foo.m_returnValue.m_rightSideConst.empty()) {
            return_value << foo.m_returnValue.m_rightSideConst << " ";
        }
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
    if(tags.size() != 1) {
        return;
    }

    TagEntryPtr classTag = tags.at(0);
    if(classTag->GetKind() != wxT("class") && classTag->GetKind() != wxT("struct"))
        return;

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
        if(t->IsDestructor() || t->IsConstructor())
            continue;

        bool isVirtual, isPureVirtual, isFinal;
        if(!GetVirtualProperty(t, isVirtual, isPureVirtual, isFinal))
            continue;

        // Skip final virtual functions
        if(isFinal)
            continue;

        if((!onlyPureVirtual && isVirtual) || isPureVirtual) {
            wxString sig = NormalizeFunctionSig(t->GetSignature(), 0);
            sig.Prepend(t->GetName());
            parentSignature2tag[sig] = t;
        }
    }

    // Step 2:
    // ========
    // Collect a list of function prototypes from the class
    tags.clear();
    GetDatabase()->GetTagsByScopeAndKind(scopeName, kind, tags, false);
    for(size_t i = 0; i < tags.size(); i++) {
        TagEntryPtr t = tags.at(i);
        wxString sig = NormalizeFunctionSig(t->GetSignature(), 0);
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
    std::vector<std::pair<wxString, int>> derivationList;

    // add this scope as well to the derivation list
    wxString _scopeName = DoReplaceMacros(scope);
    derivationList.push_back({ _scopeName, 0 });
    std::unordered_set<wxString> visited;
    GetDerivationList(_scopeName, NULL, derivationList, visited, 1);

    // make enough room for max of 500 elements in the vector
    for(size_t i = 0; i < derivationList.size(); i++) {
        wxString tmpScope(derivationList.at(i).first);
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
    std::vector<std::pair<wxString, int>> derivationList;

    // add this scope as well to the derivation list
    wxString _scopeName = DoReplaceMacros(scope);
    derivationList.push_back({ _scopeName, 0 });
    std::unordered_set<wxString> visited;
    GetDerivationList(_scopeName, NULL, derivationList, visited, 1);

    // make enough room for max of 500 elements in the vector
    for(size_t i = 0; i < derivationList.size(); i++) {
        wxString tmpScope(derivationList.at(i).first);
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
            if(tags[i]->IsFunction()) {
                tag = tags[i];
            }
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

    if(!tag || !states)
        return CppToken();

    // Get the line number of the function
    int funcLine = tag->GetLine() - 1;

    // Convert the line number to offset
    int from = states->LineToPos(funcLine);
    int to = states->FunctionEndPos(from);

    if(to == wxNOT_FOUND)
        return CppToken();

    // get list of variables from the given scope
    CxxVariableScanner varscanner(states->text, eCxxStandard::kCxx11, {}, false);
    CxxVariable::Map_t varsMap = varscanner.GetVariablesMap();

    bool isLocalVar = (varsMap.count(word) != 0);
    if(!isLocalVar)
        return CppToken();

    // search for matches in the given range
    CppTokensMap l;
    scanner.Match(word.mb_str().data(), l, from, to);

    std::vector<CppToken> tokens;
    l.findTokens(word.mb_str().data(), tokens);
    if(tokens.empty())
        return CppToken();

    // return the first match
    return *tokens.begin();
}

void TagsManager::DoParseModifiedText(const wxString& text, std::vector<TagEntryPtr>& tags)
{
    auto _tags = ParseBuffer(text);
    tags.swap(_tags);
}

bool TagsManager::IsBinaryFile(const wxString& filepath, const TagsOptionsData& tod)
{
    // If the file is a C++ file, avoid testing the content return false based on the extension
    FileExtManager::FileType type = FileExtManager::GetType(filepath);
    if(type == FileExtManager::TypeHeader || type == FileExtManager::TypeSourceC ||
       type == FileExtManager::TypeSourceCpp)
        return false;

    // If this file matches any of the c++ patterns defined in the configuration
    // don't consider it as a binary file
    if(FileUtils::WildMatch(tod.GetFileSpec(), filepath)) {
        return false;
    }

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
            if(wrappedString.IsEmpty() == false && wrappedString.Last() != wxT('\n')) {
                wrappedString << wxT("\n");
            }
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
        if(newScope.Len() >= 2) {
            newScope.RemoveLast(2);
        }
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

bool TagsManager::AreTheSame(const TagEntryPtrVector_t& v1, const TagEntryPtrVector_t& v2) const { return false; }

bool TagsManager::InsertFunctionDecl(const wxString& clsname, const wxString& functionDecl, wxString& sourceContent,
                                     int visibility)
{
    return GetLanguage()->InsertFunctionDecl(clsname, functionDecl, sourceContent, visibility);
}

void TagsManager::DoSortByVisibility(TagEntryPtrVector_t& tags)
{
    TagEntryPtrVector_t publicTags;
    TagEntryPtrVector_t protectedTags;
    TagEntryPtrVector_t privateTags;
    TagEntryPtrVector_t locals;
    TagEntryPtrVector_t members;

    for(size_t i = 0; i < tags.size(); ++i) {

        TagEntryPtr tag = tags.at(i);
        wxString access = tag->GetAccess();
        wxString kind = tag->GetKind();

        if(kind == "variable") {
            locals.push_back(tag);

        } else if(kind == "member") {
            members.push_back(tag);

        } else if(access == "private") {
            privateTags.push_back(tag);

        } else if(access == "protected") {
            protectedTags.push_back(tag);

        } else if(access == "public") {
            if(tag->GetName().StartsWith("_")) {
                // methods starting with _ usually are meant to be private
                privateTags.push_back(tag);
            } else {
                publicTags.push_back(tag);
            }
        } else {
            // assume private
            privateTags.push_back(tag);
        }
    }

    std::sort(privateTags.begin(), privateTags.end(), SAscendingSort());
    std::sort(publicTags.begin(), publicTags.end(), SAscendingSort());
    std::sort(protectedTags.begin(), protectedTags.end(), SAscendingSort());
    std::sort(members.begin(), members.end(), SAscendingSort());
    std::sort(locals.begin(), locals.end(), SAscendingSort());
    tags.clear();
    tags.insert(tags.end(), locals.begin(), locals.end());
    tags.insert(tags.end(), publicTags.begin(), publicTags.end());
    tags.insert(tags.end(), protectedTags.begin(), protectedTags.end());
    tags.insert(tags.end(), privateTags.begin(), privateTags.end());
    tags.insert(tags.end(), members.begin(), members.end());
}

void TagsManager::GetScopesByScopeName(const wxString& scopeName, wxArrayString& scopes)
{
    std::vector<std::pair<wxString, int>> derivationList;

    // add this scope as well to the derivation list
    wxString _scopeName = DoReplaceMacros(scopeName);
    derivationList.push_back({ _scopeName, 0 });
    std::unordered_set<wxString> visited;
    GetDerivationList(_scopeName, NULL, derivationList, visited, 1);

    for(size_t i = 0; i < derivationList.size(); i++) {
        wxString tmpScope(derivationList.at(i).first);
        tmpScope = DoReplaceMacros(tmpScope);
        scopes.Add(tmpScope);
    }
}

void TagsManager::InsertForwardDeclaration(const wxString& classname, const wxString& fileContent, wxString& lineToAdd,
                                           int& line, const wxString& impExpMacro)
{
    lineToAdd << "class ";
    if(!impExpMacro.IsEmpty()) {
        lineToAdd << impExpMacro << " ";
    }
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

TagEntryPtrVector_t TagsManager::ParseBuffer(const wxString& content, const wxString& filename, const wxString& kinds)
{
    TagEntryPtrVector_t tagsVec;
    CTags::ParseBuffer(filename, content, clStandardPaths::Get().GetBinaryFullPath("codelite-ctags"),
                       GetCtagsOptions().GetTokensWxMap(), tagsVec);
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
            if(!tag->IsConstructor() && !tag->IsDestructor()) {
                candidatesNoCtorDtor.push_back(tag);
            }
        });
        tags.swap(candidatesNoCtorDtor);
    }
}

void TagsManager::GetTagsByPartialNames(const wxArrayString& partialNames, std::vector<TagEntryPtr>& tags)
{
    GetDatabase()->GetTagsByPartName(partialNames, tags);
}

void TagsManager::DoTagsFromText(const wxString& text, std::vector<TagEntryPtr>& tags)
{
    // Create tags from the string
    wxArrayString tagsLines = wxStringTokenize(text, "\n", wxTOKEN_STRTOK);
    tags.reserve(tagsLines.size());
    for(wxString& line : tagsLines) {
        line.Trim().Trim(false);
        if(line.IsEmpty()) {
            continue;
        }

        TagEntryPtr tag(new TagEntry());
        tag->FromLine(line);
        tags.emplace_back(tag);
    }
}

void TagsManager::ParseWorkspaceIncremental()
{
    // restart ctagsd (this way we ensure that new settings are loaded)
    clLanguageServerEvent stop_event{ wxEVT_LSP_RESTART };
    stop_event.SetLspName("ctagsd");
    EventNotifier::Get()->AddPendingEvent(stop_event);
}

void TagsManager::ParseWorkspaceFull(const wxString& workspace_dir)
{
    // stop ctagsd
    clLanguageServerEvent stop_event{ wxEVT_LSP_STOP };
    stop_event.SetLspName("ctagsd");
    EventNotifier::Get()->ProcessEvent(stop_event);

    // delete the tags.db file
    wxFileName tags_db{ workspace_dir, "tags.db" };
    tags_db.AppendDir(".ctagsd");

    if(tags_db.FileExists()) {
        FileUtils::RemoveFile(tags_db, wxEmptyString);
    }

    // start ctagsd again
    clLanguageServerEvent start_event{ wxEVT_LSP_START };
    start_event.SetLspName("ctagsd");
    EventNotifier::Get()->ProcessEvent(start_event);
}
