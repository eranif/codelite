//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : ctags_manager.h
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

#ifndef CODELITE_CTAGS_MANAGER_H
#define CODELITE_CTAGS_MANAGER_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "database/istorage.h"
#include "language.h"
#include "macros.h"
#include "tags_options_data.h"
#include "wxStringHash.h"

#include <wx/event.h>

#define USE_TAGS_SQLITE3 1

struct DoxygenComment {
    wxString name;
    wxString comment;
};

// By default the NormalizeFunctionSig returns only the variables type
enum NormalizeFuncFlag {
    // variable name
    Normalize_Func_Name = (1 << 0),
    // variable default value
    Normalize_Func_Default_value = (1 << 1),
    // Each argument is placed on a separate line
    Normalize_Func_Arg_Per_Line = (1 << 2),
};

enum FunctionFormatFlag {
    FunctionFormat_WithVirtual = (1 << 0),
    FunctionFormat_Impl = (1 << 1),
    FunctionFormat_Arg_Per_Line = (1 << 2),
    FunctionFormat_WithOverride = (1 << 3),
};

/**
 * This class is the interface to ctags and SQLite database.
 * It contains various APIs that allows the caller to parse source file(s),
 * store it into the database and return a symbol tree.
 * TagsManager is also responsible for starting ctags processes.
 *
 * Before you use TagsManager, usually you would like to start ctags,
 * this is easily done by writing something like this:
 *
 * In the destructor of your main frame it is recommended to call Free() to avoid memory leaks:
 *
 * @code
 * // kill the TagsManager object first it will do the process termination and cleanup
 * TagsManager::Free();
 * @endcode
 *
 * @ingroup CodeLite
 * @version 1.0
 * first version
 *
 * @date 09-01-2006
 *
 * @author Eran
 *
 */
class WXDLLIMPEXP_CL TagsManager : public wxEvtHandler
{
    friend class TagsManagerST;
    friend class Language;

public:
    enum RetagType { Retag_Full, Retag_Quick, Retag_Quick_No_Scan };
    enum eLanguage { kCxx, kJavaScript };

private:
    TagsOptionsData m_tagsOptions;
    Language* m_lang;
    std::vector<TagEntryPtr> m_cachedFileFunctionsTags;
    wxString m_cachedFile;
    wxFileName m_dbFile;

#if USE_TAGS_SQLITE3
    ITagsStoragePtr m_db;
#endif
    wxString m_indexer_path;

public:
    /**
     * @brief return a set of CXX keywords
     */
    static void GetCXXKeywords(wxStringSet_t& words);

    /**
     * @brief return an array of CXX keywords
     */
    static void GetCXXKeywords(wxArrayString& words);

    void SetIndexerPath(const wxString& indexer_path) { m_indexer_path = indexer_path; }
    const wxString& GetIndexerPath() const { return m_indexer_path; }

    void SetLanguage(Language* lang);
    Language* GetLanguage();

    /**
     * @brief return the currently cached file
     */
    bool IsFileCached(const wxString& fileName) const;

    /**
     * @brief clear the file cached
     */
    void ClearCachedFile(const wxString& fileName);

    /**
     4* @brief clear all the cached tags information stored in this class
     */
    void ClearAllCaches();

    /**
     * @brief load fileName into cache, note that this call will clear perivous
     * cache
     */
    void CacheFile(const wxString& fileName);

    /**
     * @brief return the cached file tags
     */
    const std::vector<TagEntryPtr>& GetCachedFileTags() const { return m_cachedFileFunctionsTags; }

    /**
     * Return the CtagsOptions used by the tags manager
     * @return
     */
    const TagsOptionsData& GetCtagsOptions() const { return m_tagsOptions; }

    /**
     * Set Ctags Options
     * @param options options to use
     */
    void SetCtagsOptions(const TagsOptionsData& options);

    void SetEncoding(const wxFontEncoding& encoding);

    /**
     * Locate symbol by name in database
     * @param name name to search
     * @param tags [output] result vector
     */
    void FindSymbol(const wxString& name, std::vector<TagEntryPtr>& tags);

    /**
     * @brief parse source file (from memory) and return list of tags
     * If "filename" is passed, each returned TagEntryPtr will have it as its "File" attribute
     */
    TagEntryPtrVector_t ParseBuffer(const wxString& content, const wxString& filename = wxEmptyString,
                                    const wxString& kinds = "cdefgmnpstuv");

    /**
     * load all symbols of fileName from the database and return them
     * to user as tree
     * @param path file's symbols
     * @param tags if tags is set to non NULL, the tags are converted to TagTreePtr
     * @return tag tree
     */
    TagTreePtr Load(const wxFileName& fileName, TagEntryPtrVector_t* tags = NULL);

    /**
     * Open sqlite database.
     * @param fileName Database file name
     */
    void OpenDatabase(const wxFileName& fileName);

    /**
     * Return a pointer to the underlying databases object.
     * @return tags database
     */
    ITagsStoragePtr GetDatabase();

    /**
     * Delete all entries from database that are related to file name.
     * @param path Database name
     * @param fileName File name
     */
    void Delete(const wxFileName& path, const wxString& fileName);

    /**
     * Find symbols by name and scope.
     * @param name symbol name
     * @param scope full path to symbol. if set to wxEmptyString, the search is performed against the global
     * @param tags [output] a vector of the results tags
     * @return true on success false otherwise
     */
    void FindByNameAndScope(const wxString& name, const wxString& scope, std::vector<TagEntryPtr>& tags);

    /**
     * Find tags with given path
     * @param path path to search
     * @param tags [output] output tags
     */
    void FindByPath(const wxString& path, std::vector<TagEntryPtr>& tags);

    /**
     * Get tags related to a scope.
     * @param scope scope to search for members
     * @param tags [output] vector of tags
     */
    void TagsByScope(const wxString& scope, std::vector<TagEntryPtr>& tags);

    /**
     *	Get tags related to a scope and name (name can be partial name
     * @param scope scope to search for members
     * @param name partial tag name
     * @param tags [output] vector of tags
     */
    void TagsByScopeAndName(const wxString& scope, const wxString& name, std::vector<TagEntryPtr>& tags,
                            size_t flags = PartialMatch);

    /**
     * Delete all tags related to these files
     * @param files list of files, in absolute path
     */
    void DeleteFilesTags(const std::vector<wxFileName>& files);
    void DeleteFilesTags(const wxArrayString& files);

    /**
     * Close the workspace database
     */
    void CloseDatabase();

    /**
     * @brief trigger a complete parsing of the workspace
     */
    void ParseWorkspaceFull(const wxString& workspace_dir);

    /**
     * @brief trigger an incremental workspace parsing
     */
    void ParseWorkspaceIncremental();

    /**
     * Get a hover tip. This function is a wrapper around the Language::GetHoverTip.
     * @param expr the current expression
     * @param word the token under the cursor
     * @param text scope where token was found
     * @param scopeName scope name
     * @param isFunc is token is a function
     * @param tips array of tip strings
     */
    void GetHoverTip(const wxFileName& fileName, int lineno, const wxString& expr, const wxString& word,
                     const wxString& text, std::vector<wxString>& tips);

    /**
     * return tags belongs to given scope and kind
     * @param scopeName the scope to search
     * @param kind tags's kind to return
     * @param tags [output] the result vector
     * @param inherits set to true if you want inherited members as well members
     */
    void TagsByScope(const wxString& scopeName, const wxString& kind, std::vector<TagEntryPtr>& tags,
                     bool includeInherits = false, bool applyLimit = true);

    /**
     * return tags belongs to given scope and kind
     * @param scopeName the scope to search
     * @param kind list of tags kind to return
     * @param tags [output] the result vector
     * @param inherits set to true if you want inherited members as well members
     * @param include_anon included anonymous members (of Unions/structs/enums)
     */
    void TagsByScope(const wxString& scopeName, const wxArrayString& kind, std::vector<TagEntryPtr>& tags,
                     bool include_anon = false);

    /**
     * @brief get the scope name. CodeLite assumes that the caret is placed at the end of the 'scope'
     * @param scope the input string
     * @return scope name or '<global>' if non found
     */
    wxString GetScopeName(const wxString& scope);

    /**
     * return list of files from the database(s). The returned list is ordered
     * by name (ascending)
     * @param partialName part of the file name to act as a filter
     * @param files [output] array of files
     */
    void GetFiles(const wxString& partialName, std::vector<FileEntryPtr>& files);
    void GetFiles(const wxString& partialName, std::vector<wxFileName>& files);
    /**
     * @brief this function is for supporting CC inside an include statement
     * line
     */
    void GetFilesForCC(const wxString& userTyped, wxArrayString& matches);

    /**
     * Return function that is close to current line number and matches
     * file name
     * @param fileName file to search for
     * @param lineno the line number
     * @return pointer to the tage which matches the line number & files
     */
    TagEntryPtr FunctionFromFileLine(const wxFileName& fileName, int lineno, bool nextFunction = false);

    /**
     * @brief Return function that is close to current line number
     * this is done by parsing `buffer`
     * @param buffer input source file as string
     * @param lineno the end buffer line number
     * @param file_name the returned tag `GetFile()` value
     */
    TagEntryPtr FunctionFromBufferLine(const wxString& buffer, int lineno, const wxString& file_name);

    /**
     * @brief
     * @param scope
     * @param tags
     */
    virtual void GetSubscriptOperator(const wxString& scope, std::vector<TagEntryPtr>& tags);
    /**
     * @brief
     * @param scope
     * @param tags
     */
    virtual void GetDereferenceOperator(const wxString& scope, std::vector<TagEntryPtr>& tags);

    /**
     * @brief return list of all classes.
     * @param tags [output] vector of tags for the classes
     * @param onlyWorkspace set to true if you wish to accept only classes belongs to the workspace, false if you would
     * like to receive
     * classes from the external database as well
     */
    void GetClasses(std::vector<TagEntryPtr>& tags, bool onlyWorkspace = true);

    /**
     * @brief return list of tags by KIND
     * @param tags [output]
     * @param kind the kind of the tags to fetch from the database
     * @param partName name criterion (partial)
     */
    void GetTagsByKind(std::vector<TagEntryPtr>& tags, const wxArrayString& kind,
                       const wxString& partName = wxEmptyString);

    /**
     * @brief return list of tags by name
     * @param prefix
     * @param tags
     */
    void GetTagsByName(const wxString& prefix, std::vector<TagEntryPtr>& tags);

    /**
     * @brief return list of tags by their partial names
     */
    void GetTagsByPartialNames(const wxArrayString& partialNames, std::vector<TagEntryPtr>& tags);

    /**
     * @brief return list of tags by KIND
     * @param tags [output]
     * @param kind the kind of the tags to fetch from the database
     * @param partName name criterion (partial)
     */
    void GetTagsByKindLimit(std::vector<TagEntryPtr>& tags, const wxArrayString& kind, int limit,
                            const wxString& partName = wxEmptyString);

    /**
     * @brief generate function body/impl based on a tag
     * @param tag the input tag which represents the requested tag
     * @param impl set to true if you need an implementation, false otherwise. Default is set to false
     * @param scope real function scope to use
     * @return the function impl/decl
     */
    wxString FormatFunction(TagEntryPtr tag, size_t flags = FunctionFormat_WithVirtual,
                            const wxString& scope = wxEmptyString);

    /**
     * @brief return true if type & scope do exist in the symbols database
     * @param typeName
     * @param scope
     * @return
     */
    bool IsTypeAndScopeExists(wxString& typeName, wxString& scope);

    /**
     * @brief try to process a given expression and evaluate it into type & typescope
     * @param expression
     * @param type
     * @param typeScope
     * @return true on success false otherwise
     */
    bool ProcessExpression(const wxString& expression, wxString& type, wxString& typeScope);

    /**
     * @brief return normalize function signature. This function strips any default values or variable
     * name from the signature. The return value for signature like this: wxT("int value, const std::string &str = "",
     * void *data = NULL"), is "int, const std::string&, void *"
     * and by setting the  includeVarNames to true, it will also returns the variables names
     * @param sig signature
     * @param includeVarNames set to true if the stripped signature should include the variables names. By default it is
     * set to false
     * @return stripped functions signature
     */
    wxString NormalizeFunctionSig(const wxString& sig, size_t flags = Normalize_Func_Name,
                                  std::vector<std::pair<int, int>>* paramLen = NULL);

    /**
     * @brief accept as input ctags pattern of a function and tries to evaluate the
     * return value of the function
     * @param pattern ctags pattern of the method
     * @return return value of the method from the pattern of empty string
     */
    wxString GetFunctionReturnValueFromPattern(TagEntryPtr tag);
    /**
     * @brief fileter a recently tagged files from the strFiles array
     * @param strFiles
     * @param db
     */
    void FilterNonNeededFilesForRetaging(wxArrayString& strFiles, ITagsStoragePtr db);

    /**
     * @brief return true of v1 cotnains the same tags as v2
     */
    bool AreTheSame(const TagEntryPtrVector_t& v1, const TagEntryPtrVector_t& v2) const;

    /**
     * @brief insert functionBody into clsname. This function will search for best location
     * to place the function body. set visibility to 0 for 'pubilc' function, 1 for 'protected' and 2 for private
     * return true if this function succeeded, false otherwise
     */
    bool InsertFunctionDecl(const wxString& clsname, const wxString& functionDecl, wxString& sourceContent,
                            int visibility = 0);

    /**
     * @brief insert forward declaration statement at the top of the file
     * @param classname the class name to add
     * @param fileContent [input] the file content
     * @param lineToAdd [output] the line that should be added
     * @param line [output] line number where to add the forward declaration
     * @param impExpMacro [optional/input] Windows DLL Imp/Exp macro
     */
    void InsertForwardDeclaration(const wxString& classname, const wxString& fileContent, wxString& lineToAdd,
                                  int& line, const wxString& impExpMacro = "");

protected:
    std::map<wxString, bool> m_typeScopeCache;

private:
    /**
     * Construct a TagsManager object, for internal use
     */
    TagsManager();

    /**
     * Destructor
     */
    virtual ~TagsManager();

    /**
     * @param path
     * @param parentTag
     * @param derivationList
     * @param scannedInherits
     */
    bool GetDerivationList(const wxString& path, TagEntryPtr parentTag,
                           std::vector<std::pair<wxString, int>>& derivationList, std::unordered_set<wxString>& visited,
                           int depth);

    bool GetDerivationListInternal(const wxString& path, TagEntryPtr parentTag,
                                   std::vector<std::pair<wxString, int>>& derivationList,
                                   std::unordered_set<wxString>& visited, int depth);

public:
    /**
     * @brief return true if the file is binary (by searching for NULL chars)
     * @param filepath file to examine
     * @return return true if the file is binary
     */
    bool IsBinaryFile(const wxString& filepath, const TagsOptionsData& tod);

    /**
     * @brief create doxygen comment from a tag
     * @param tag
     * @param keyPrefix should we use @ or \\ to prefix doxy keywords?
     */
    DoxygenComment DoCreateDoxygenComment(TagEntryPtr tag, wxChar keyPrefix);

protected:
    void DoFindByNameAndScope(const wxString& name, const wxString& scope, std::vector<TagEntryPtr>& tags);
    void RemoveDuplicatesTips(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target);
    void GetGlobalTags(const wxString& name, std::vector<TagEntryPtr>& tags, size_t flags = PartialMatch);
    void GetLocalTags(const wxString& name, const wxString& scope, std::vector<TagEntryPtr>& tags, bool isFuncSignature,
                      size_t flags = PartialMatch);
    void TipsFromTags(const std::vector<TagEntryPtr>& tags, const wxString& word, std::vector<wxString>& tips);
    bool ProcessExpression(const wxFileName& filename, int lineno, const wxString& expr, const wxString& scopeText,
                           wxString& typeName, wxString& typeScope, wxString& oper, wxString& scopeTemplateInitList);
    wxString DoReplaceMacros(const wxString& name);
    wxArrayString BreakToOuterScopes(const wxString& scope);
    wxString DoReplaceMacrosFromDatabase(const wxString& name);
    void GetScopesByScopeName(const wxString& scopeName, wxArrayString& scopes);
};

/// create the singleton typedef
class WXDLLIMPEXP_CL TagsManagerST
{
public:
    static TagsManager* Get();
    static void Free();
};

#endif // CODELITE_CTAGS_MANAGER_H
