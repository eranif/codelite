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
#include "Cxx/CxxTemplateFunction.h"
#include "Cxx/CxxVariableScanner.h"
#include "Cxx/cpp_comment_creator.h"
#include "StdToWX.h"
#include "cl_command_event.h"
#include "code_completion_api.h"
#include "codelite_events.h"
#include "database/tags_storage_sqlite3.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "precompiled_header.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/timer.h>
#include <wx/tokenzr.h>
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

/// Ascending sorting function
struct SAscendingSort {
    bool operator()(const TagEntryPtr& rStart, const TagEntryPtr& rEnd)
    {
        return rEnd->GetName().CmpNoCase(rStart->GetName()) > 0;
    }
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
{
    m_db = std::make_shared<TagsStorageSQLite>();
    m_db->SetSingleSearchLimit(MAX_SEARCH_LIMIT);
}

TagsManager::~TagsManager() {}

void TagsManager::OpenDatabase(const wxFileName& fileName)
{
    m_dbFile = fileName;
    m_db->OpenDatabase(m_dbFile);
    m_db->SetEnableCaseInsensitive(true);
    m_db->SetSingleSearchLimit(m_tagsOptions.GetCcNumberOfDisplayItems());
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
    tree = std::make_shared<TagTree>(wxT("<ROOT>"), root);
    for(size_t i = 0; i < tagsByFile.size(); i++) {
        tree->AddEntry(*(tagsByFile.at(i)));
    }
    return tree;
}

void TagsManager::Delete(const wxFileName& path, const wxString& fileName)
{
    GetDatabase()->DeleteByFileName(path, fileName);
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

//-----------------------------------------------------------------------------
// <<<<<<<<<<<<<<<<<<< Code Completion API END
//-----------------------------------------------------------------------------
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
    const wxArrayString kind = StdToWX::ToArrayString({ wxT("class"), wxT("struct") });

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

void TagsManager::CloseDatabase()
{
    m_dbFile.Clear();
    m_db = nullptr; // Free the current database
    m_db = std::make_shared<TagsStorageSQLite>();
    m_db->SetSingleSearchLimit(m_tagsOptions.GetCcNumberOfDisplayItems());
    m_db->SetUseCache(false);
}

DoxygenComment TagsManager::DoCreateDoxygenComment(TagEntryPtr tag, wxChar keyPrefix)
{
    CppCommentCreator commentCreator(tag, keyPrefix);
    DoxygenComment dc;
    dc.comment = commentCreator.CreateComment();
    dc.name = tag->GetName();
    return dc;
}

void TagsManager::SetCtagsOptions(const TagsOptionsData& options)
{
    m_tagsOptions = options;
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
                                    wxString& scopeTemplateInitList)
{
    return GetLanguage()->ProcessExpression(expr, scopeText, filename, lineno, typeName, typeScope, oper,
                                            scopeTemplateInitList);
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
    const wxArrayString kind = StdToWX::ToArrayString({ wxT("class"), wxT("struct"), wxT("union") });

    GetDatabase()->GetTagsByKind(kind, wxT("name"), ITagsStorage::OrderAsc, tags);
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

void TagsManager::CacheFile(const wxString& fileName)
{
    if(!GetDatabase()) {
        return;
    }

    m_cachedFile = fileName;
    m_cachedFileFunctionsTags.clear();

    const wxArrayString kinds = StdToWX::ToArrayString({ wxT("function"), wxT("prototype") });
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

void TagsManager::FilterNonNeededFilesForRetaging(wxArrayString& strFiles, ITagsStoragePtr db)
{
    std::vector<FileEntryPtr> files_entries;
    db->GetFiles(files_entries);
    std::unordered_set<wxString> files_set;

    for(size_t i = 0; i < strFiles.GetCount(); i++) {
        files_set.insert(strFiles.Item(i));
    }

    for(size_t i = 0; i < files_entries.size(); i++) {
        const FileEntryPtr& fe = files_entries.at(i);

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

bool TagsManager::AreTheSame(const TagEntryPtrVector_t& v1, const TagEntryPtrVector_t& v2) const { return false; }

bool TagsManager::InsertFunctionDecl(const wxString& clsname, const wxString& functionDecl, wxString& sourceContent,
                                     int visibility)
{
    return GetLanguage()->InsertFunctionDecl(clsname, functionDecl, sourceContent, visibility);
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
    words = StdToWX::ToArrayString({ "alignas",      "alignof",
                                     "and",          "and_eq",
                                     "asm",          "auto",
                                     "bitand",       "bitor",
                                     "bool",         "break",
                                     "case",         "catch",
                                     "char",         "char16_t",
                                     "char32_t",     "class",
                                     "compl",        "const",
                                     "constexpr",    "const_cast",
                                     "continue",     "decltype",
                                     "default",      "delete",
                                     "do",           "double",
                                     "dynamic_cast", "else",
                                     "enum",         "explicit",
                                     "export",       "extern",
                                     "false",        "final",
                                     "float",        "for",
                                     "friend",       "goto",
                                     "if",           "inline",
                                     "int",          "long",
                                     "mutable",      "namespace",
                                     "new",          "noexcept",
                                     "not",          "not_eq",
                                     "nullptr",      "operator",
                                     "or",           "or_eq",
                                     "override",     "private",
                                     "protected",    "public",
                                     "register",     "reinterpret_cast",
                                     "return",       "short",
                                     "signed",       "sizeof",
                                     "static",       "static_assert",
                                     "static_cast",  "struct",
                                     "switch",       "template",
                                     "this",         "thread_local",
                                     "throw",        "true",
                                     "try",          "typedef",
                                     "typeid",       "typename",
                                     "union",        "unsigned",
                                     "using",        "virtual",
                                     "void",         "volatile",
                                     "wchar_t",      "while",
                                     "xor",          "xor_eq" });
}

TagEntryPtrVector_t TagsManager::ParseBuffer(const wxString& content, const wxString& filename, const wxString& kinds)
{
    TagEntryPtrVector_t tagsVec;
    CTags::ParseBuffer(filename, content, clStandardPaths::Get().GetBinaryFullPath("codelite-ctags"),
                       GetCtagsOptions().GetTokensWxMap(), tagsVec);
    return tagsVec;
}

void TagsManager::GetTagsByPartialNames(const wxArrayString& partialNames, std::vector<TagEntryPtr>& tags)
{
    GetDatabase()->GetTagsByPartName(partialNames, tags);
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
