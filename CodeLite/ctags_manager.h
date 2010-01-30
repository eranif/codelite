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

#include "wx/event.h"
#include <wx/timer.h>
#include <wx/stopwatch.h>
#include "wx/process.h"
#include "cl_process.h"
#include "tree.h"
#include "entry.h"
#include "tags_storage_sqlite3.h"
#include <wx/thread.h>
#include "singleton.h"
#include "cl_calltip.h"
#include "comment.h"
#include "tags_options_data.h"
#include "setters_getters_data.h"
#include "extdbdata.h"
#include "language.h"
#include <set>

#ifdef USE_TRACE
#include <wx/stopwatch.h>
#endif

/// Forward declaration
class clProcess;
class DirTraverser;
class Language;
class wxTimer;
class Language;


#define TagsGlobal 0
#define TagsGlobalGTK 1

// send this event whenever the a tags file needs to be updated
extern const wxEventType wxEVT_UPDATE_FILETREE_EVENT;
extern const wxEventType wxEVT_TAGS_DB_UPGRADE;

struct DoxygenComment {
	wxString name;
	wxString comment;
};

// By default the NormalizeFunctionSig returns only the variables type
enum NormalizeFuncFlag {
	// variable name
	Normalize_Func_Name          = 0x00000001,
	// variable default value
	Normalize_Func_Default_value = 0x00000002,
};

class ITagsStorage;

/**
 * This class is the interface to ctags and SQLite database.
 * It contains various APIs that allows the caller to parse source file(s),
 * store it into the database and return a symbol tree.
 * TagsManager is also responsible for starting ctags processes.
 *
 * Before you use TagsManager, usually you would like to start ctags,
 * this is easily done by writing something like this:
 *
 * @code
 * // Create ctags processes
 * TagsManagerST::Get()->StartCtagsProcess(TagsGlobal);
 * @endcode
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
class TagsManager : public wxEvtHandler
{
	// Members
	friend class Singleton<TagsManager>;
	friend class DirTraverser;
	friend class Language;

	ITagsStorage *                m_workspaceDatabase;
	wxCriticalSection             m_cs;
	wxCriticalSection             m_crawlerLocker;
	wxFileName                    m_codeliteIndexerPath;
	clProcess*                    m_codeliteIndexerProcess;
	wxString                      m_ctagsCmd;
	wxStopWatch                   m_watch;
	TagsOptionsData               m_tagsOptions;
	std::map<int, clProcess*>     m_processes;
	bool                          m_parseComments;
	bool                          m_canDeleteCtags;
	std::list<clProcess*>         m_gargabeCollector;
	wxTimer*                      m_timer;
	Language*                     m_lang;
	std::vector<TagEntryPtr>      m_cachedFileFunctionsTags;
	wxString                      m_cachedFile;
	bool                          m_enableCaching;
	wxEvtHandler*                 m_evtHandler;
	std::set<wxString>            m_CppIgnoreKeyWords;
public:

	void SetLanguage(Language *lang);
	Language *GetLanguage();
	void      SetEvtHandler(wxEvtHandler *handler){m_evtHandler = handler;}

	wxString GetCTagsCmd();

	/**
	 * @brief return the currently cached file
	 */
	bool IsFileCached(const wxString &fileName) const;

	/**
	 * @brief clear the file cached
	 */
	void ClearCachedFile(const wxString &fileName);

	/**
	 * @brief load fileName into cache, note that this call will clear perivous
	 * cache
	 */
	void CacheFile(const wxString &fileName);

	/**
	 * @brief return the cached file tags
	 */
	const std::vector<TagEntryPtr>& GetCachedFileTags() const {
		return m_cachedFileFunctionsTags;
	}

	/**
	 * Return the CtagsOptions used by the tags manager
	 * @return
	 */
	const TagsOptionsData& GetCtagsOptions() const {
		return m_tagsOptions;
	}

	/**
	 * Set Ctags Options
	 * @param options options to use
	 */
	void SetCtagsOptions(const TagsOptionsData &options);

	/**
	 * Locate symbol by name in database
	 * @param name name to search
	 * @param tags [output] result vector
	 */
	void FindSymbol(const wxString& name, std::vector<TagEntryPtr> &tags);

	/**
	 * ParseTagsFile CTAGS file and construct a TagTree.
	 * User should use ParseSourceFile, which calls to this function internally.
	 * This function throws a std::exception*.
	 * @param fp CTAGS tags file name
	 * @return tag tree
	 */
	TagTreePtr ParseTagsFile(const wxFileName& fp);

	/**
	 * Parse a source file and construct a TagTree.
	 * This function throws a std::exception*.
	 * @param fp Source file name
	 * @param comments if not null, comments will be parsed as well, and will be returned as vector
	 * @return tag tree
	 */
	TagTreePtr ParseSourceFile(const wxFileName& fp, std::vector<CommentPtr> *comments = NULL);
	TagTreePtr ParseSourceFile2(const wxFileName& fp, const wxString &tags, std::vector<CommentPtr> *comments = NULL);

	/**
	 * @brief Set the full path to ctags executable, else TagsManager will use relative path ctags.
	 * So, if for example, ctags is located at: $/home/eran/bin$, you simply call this function
	 * with SetCtagsPath(_T("/home/eran/bin"));
	 * @param path ctags
	 */
	void SetCodeLiteIndexerPath(const wxString& path);

	/**
	 * @brief Store tree of tags into db.
	 * @param tree Tags tree to store
	 * @param path Database file name
	 */
	void Store(TagTreePtr tree, const wxFileName& path = wxFileName());

	/**
	 * load all symbols of fileName from the database and return them
	 * to user as tree
	 * @param path file's symbols
	 * @return tag tree
	 */
	TagTreePtr Load(const wxFileName& fileName);

	/**
	 * Open sqlite database.
	 * @param fileName Database file name
	 */
	void OpenDatabase(const wxFileName& fileName);

	/**
	 * Return a pointer to the underlying databases object.
	 * @return tags database
	 */
	ITagsStorage* GetDatabase() {
		return m_workspaceDatabase;
	}

	/**
	 * Delete all entries from database that are related to file name.
	 * @param path Database name
	 * @param fileName File name
	 */
	void Delete(const wxFileName& path, const wxString& fileName);

	/**
	 * Start a ctags process on a filter mode.
	 * By default, TagsManager will try to launch the ctags process using the following command line:
	 * @code
	 * ctags --fields=aKmSsni --filter=yes --filter-terminator="<<EOF>>\n"
	 * @endcode
	 *
	 * It is possible to add a full path to ctags exectuable by calling the SetCtagsPath() function.
	 */
	clProcess *StartCtagsProcess();

	/**
	 * Restart ctags process.
	 * @param kind
	 * @return
	 */
	void RestartCtagsProcess();

	/**
	 * Test if filename matches the current ctags file spec.
	 * @param filename file name to test
	 * @return true if the file name extension matches the current running ctags file spec
	 */
	bool IsValidCtagsFile(const wxFileName &filename) const;

	/**
	 * Find symbols by name and scope.
	 * @param name symbol name
	 * @param scope full path to symbol. if set to wxEmptyString, the search is performed against the global
	 * @param tags [output] a vector of the results tags
	 * @return true on success false otherwise
	 */
	void FindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags);

	/**
	 * Find tags with given path
	 * @param path path to search
	 * @param tags [output] output tags
	 */
	void FindByPath(const wxString &path, std::vector<TagEntryPtr> &tags);

	/**
	 * Get tags related to a scope.
	 * @param scope scope to search for members
	 * @param tags [output] vector of tags
	 */
	void TagsByScope(const wxString& scope, std::vector<TagEntryPtr> &tags);

	/**
	 *	Get tags related to a scope and name (name can be partial name
	 * @param scope scope to search for members
	 * @param name partial tag name
	 * @param tags [output] vector of tags
	 */
	void TagsByScopeAndName(const wxString& scope, const wxString &name, std::vector<TagEntryPtr> &tags, size_t flags = PartialMatch);

	/**
	 * Return autocompletion candidates based on parsing an expression and retrieving its member from the database.
	 * @param expr Expression to evaluate, can be complex one, such as ((MyClass&)cls).GetName().GetData() ... )
	 * @param text Scope where the expression is located
	 * @param candidates [output] list of TagEntries that can be displayed in Autucompletion box
	 * @return true if candidates.size() is greater than 0
	 */
	bool AutoCompleteCandidates(const wxFileName &fileName, int lineno, const wxString& expr, const wxString& text, std::vector<TagEntryPtr> &candidates);

	/**
	 * Return a word completion candidates. this function is used when user hit Ctrl+Space.
	 * @param expr Expression to evaluate, can be complex one, such as ((MyClass&)cls).GetName().GetData() ... )
	 * @param text Scope where the expression is located
	 * @param &word the partial word entered by user
	 * @param &candidates [output] list of TagEntries that can be displayed in Autucompletion box
	 * @return true if candidates.size() is greater than 0
	 */
	bool WordCompletionCandidates(const wxFileName &fileName, int lineno, const wxString& expr, const wxString& text, const wxString &word, std::vector<TagEntryPtr> &candidates);

	/**
	 * Delete all tags related to these files
	 * @param files list of files, in absolute path
	 */
	void DeleteFilesTags(const std::vector<wxFileName> &files);
	void DeleteFilesTags(const wxArrayString &files);

	/**
	 * @brief delete all entries from tags database which starts with. If the dbFileName is also an active one,
	 * clear any cache entries as well
	 * @param dbfileName database file path
	 * @param filePrefix tag file's prefix
	 */
	void DeleteTagsByFilePrefix(const wxString &dbfileName, const wxString &filePrefix);

	/**
	 * Retag files in the database. 'Retagging' means:
	 * - delete all entries from the database that belongs to one of these files
	 * - parse the files
	 * - update the database again
	 * @param files list of files, in absolute path, to retag
	 */
	void RetagFiles(const std::vector<wxFileName> &files, bool quickRetag);

	/**
	 * Close the workspace database
	 */
	void CloseDatabase();

	/**
	 * Get a hover tip. This function is a wrapper around the Language::GetHoverTip.
	 * @param expr the current expression
	 * @param word the token under the cursor
	 * @param text scope where token was found
	 * @param scopeName scope name
	 * @param isFunc is token is a function
	 * @param tips array of tip strings
	 */
	void GetHoverTip(const wxFileName &fileName, int lineno, const wxString & expr, const wxString &word, const wxString & text, std::vector<wxString> & tips);

	/**
	 * Return a function call tip object
	 * @param expression expression where the function was found
	 * @param text local scope
	 * @param word function name
	 * @return call tip object
	 */
	clCallTipPtr GetFunctionTip(const wxFileName &fileName, int lineno, const wxString &expression, const wxString &text, const wxString &word);

	/**
	 * Return true if comment parsing is enabled, false otherwise
	 */
	bool GetParseComments();

	/**
	 * Generate doxygen based on file & line. The generated doxygen is partial, that is, only the "\param" "\return"
	 * is generated. On top of the comment, there will be the a place holder to be replaced by the application, the place
	 * holder can be one of:
	 * '$(ClassPattern)' or '$(FunctionPattern)'
	 * @param line line number
	 * @param file file name
	 * @param keyPrefix prefix to use for the 'param' & 'return' keyword (can be @ or \ )
	 */
	DoxygenComment GenerateDoxygenComment(const wxString &file, const int line, wxChar keyPrefix);

	/**
	 * Load all types from database. 'Type' is one of:
	 * class, namespace, struct, union, enum, macro, typedef
	 * @param &tags
	 */
	void OpenType(std::vector<TagEntryPtr> &tags);

	/**
	 * return string containing a code section to be inserted into the document. By providing
	 * decl which is not null, this function will split the generated code into two - decl & impl
	 * @param scope the current text from begining of the document up to the cursor pos, this
	 *        string will be parsed by CodeLite to determine the current scope
	 * @param data user's settings for the generation of the getters/setters
	 * @param tags list of members to create setters/getters for them.
	 * @param impl [output] the generated code - implementation, if 'decl' member is null,
	          it will include the declaration as well
	 * @param decl [output] if not null, will contain the declaration part of the functions
	 */
	void GenerateSettersGetters(const wxString &scope, const SettersGettersData &data, const std::vector<TagEntryPtr> &tags, wxString &impl, wxString *decl = NULL);

	/**
	 * return tags belongs to given scope and kind
	 * @param scopeName the scope to search
	 * @param kind tags's kind to return
	 * @param tags [ouput] the result vector
	 * @param inherits set to true if you want inherited members as well members
	 */
	void TagsByScope(const wxString &scopeName, const wxString &kind, std::vector<TagEntryPtr> &tags, bool includeInherits = false, bool onlyWorkspace = false);

	/**
	 * return tags belongs to given scope and kind
	 * @param scopeName the scope to search
	 * @param kind list of tags kind to return
	 * @param tags [ouput] the result vector
	 * @param inherits set to true if you want inherited members as well members
	 * @param include_anon included anonymous members (of Unions/structs/enums)
	 */
	void TagsByScope(const wxString &scopeName, const wxArrayString &kind, std::vector<TagEntryPtr> &tags, bool include_anon = false);

	/**
	 * Find implementation/declaration of symbol
	 * @param expr the current expression
	 * @param word the token under the cursor
	 * @param text scope where token was found
	 * @param gotoImpl set to true, if you wish that CodeLite will find the implementation, false to declaration
	 * @param tags the output
	 */
	void FindImplDecl(const wxFileName &fileName, int lineno, const wxString & expr, const wxString &word,  const wxString &text, std::vector<TagEntryPtr> &tags, bool impl = true, bool workspaceOnly = false);

	/**
	 * @brief get the scope name. CodeLite assumes that the caret is placed at the end of the 'scope'
	 * @param scope the input string
	 * @return scope name or '<global>' if non found
	 */
	wxString GetScopeName(const wxString &scope);

	/**
	 * Pass a source file to ctags process, wait for it to process it and return the output.
	 * @param source Source file name
	 * @param tags String containing the ctags output
	 */
	void SourceToTags(const wxFileName& source, wxString& tags);

	/**
	 * return list of files from the database(s). The returned list is ordered
	 * by name (ascending)
	 * @param partialName part of the file name to act as a filter
	 * @param files [output] array of files
	 */
	void GetFiles(const wxString &partialName, std::vector<FileEntryPtr> &files);
	void GetFiles(const wxString &partialName, std::vector<wxFileName> &files);

	/**
	 * Return function that is close to current line number and matches
	 * file name
	 * @param fileName file to search for
	 * @param lineno the line number
	 * @return pointer to the tage which matches the line number & files
	 */
	TagEntryPtr FunctionFromFileLine(const wxFileName &fileName, int lineno, bool nextFunction = false);

	/**
	 * @brief return the first function of 'fileName'
	 * @param fileName file to scan
	 * @return NULL or valid tag
	 */
	TagEntryPtr FirstFunctionOfFile(const wxFileName &fileName);

	/**
	 * @brief return the first scope of 'fileName'
	 * @param fileName file to scan
	 * @return NULL or valid tag
	 */
	TagEntryPtr FirstScopeOfFile(const wxFileName &fileName);

	/**
	 * @brief return list of scopes from a given file. This function is used by the navigation bar
	 * @param name
	 * @param scopes
	 */
	void GetScopesFromFile(const wxFileName &fileName, std::vector<wxString> &scopes);

	/**
	 * @brief return the scope's member type, for example:
	 * if yout class MyClass::m_str, and m_str is std::string, then calling this function will result with
	 * type=basic_string
	 * typeScope=std
	 * @param scope scope name
	 * @param name the member name
	 * @param type
	 * @param typeScope
	 * @return true on success, false otherwise
	 */
	bool GetMemberType(const wxString &scope, const wxString &name, wxString &type, wxString &typeScope);

	/**
	 * @brief return list of tags by file name & scope
	 * @param fileName
	 * @param scopeName
	 * @param tags
	 */
	void TagsFromFileAndScope(const wxFileName &fileName, const wxString &scopeName, std::vector<TagEntryPtr> &tags);

	/**
	 * @brief return information about the current function based on file & line
	 * @param fileName the current file name
	 * @param lineno the current line number
	 * @param tag [output]
	 * @param func [output]
	 * @return true on success, false otherwise
	 */
	bool GetFunctionDetails(const wxFileName &fileName, int lineno, TagEntryPtr &tag, clFunction &func);

	/**
	 * @brief return list of all classes.
	 * @param tags [output] vector of tags for the classes
	 * @param onlyWorkspace set to true if you wish to accept only classes belongs to the workspace, false if you would like to receive
	 * classes from the external database as well
	 */
	void GetClasses(std::vector< TagEntryPtr > &tags, bool onlyWorkspace = true);

	/**
	 * @brief return list of functions
	 * @param tags
	 * @param fileName
	 */
	void GetFunctions(std::vector< TagEntryPtr > &tags, const wxString &fileName = wxEmptyString, bool onlyWorkspace = true);

	/**
	 * @brief return list of tags by KIND
	 * @param tags [output]
	 * @param kind the kind of the tags to fetch from the database
	 * @param partName name criterion (partial)
	 */
	void GetTagsByKind(std::vector<TagEntryPtr> &tags, const wxArrayString &kind, const wxString &partName = wxEmptyString);
	
	/**
	 * @brief return list of tags by KIND
	 * @param tags [output]
	 * @param kind the kind of the tags to fetch from the database
	 * @param partName name criterion (partial)
	 */
	void GetTagsByKindLimit(std::vector<TagEntryPtr> &tags, const wxArrayString &kind, int limit, const wxString &partName = wxEmptyString);

	/**
	 * @brief generate function body/impl based on a tag
	 * @param tag the input tag which represents the requested tag
	 * @param impl set to true if you need an implementation, false otherwise. Default is set to false
	 * @param scope real function scope to use
	 * @return the function impl/decl
	 */
	wxString FormatFunction(TagEntryPtr tag, bool impl = false, const wxString &scope = wxEmptyString);

	/**
	 * @brief return true of the tag contains a pure virtual function
	 * @param tag
	 */
	bool IsPureVirtual(TagEntryPtr tag);

	/**
	 * @brief return true of the tag contains a virtual function (can be pure)
	 * @param tag
	 */
	bool IsVirtual(TagEntryPtr tag);

	/**
	 * @brief return true if type & scope do exist in the symbols database
	 * @param typeName
	 * @param scope
	 * @return
	 */
	bool IsTypeAndScopeExists(const wxString &typeName, wxString &scope);

	/**
	 * @brief return true if type & scope do exist in the symbols database and is container
	 * @param typeName
	 * @param scope
	 * @return
	 */
	bool IsTypeAndScopeContainer(const wxString &typeName, wxString &scope);

	/**
	 * @brief try to process a given expression and evaluate it into type & typescope
	 * @param expression
	 * @param type
	 * @param typeScope
	 * @return true on success false otherwise
	 */
	bool ProcessExpression(const wxString &expression, wxString &type, wxString &typeScope);

	/**
	 * @brief strip comments from a given text
	 * @param text
	 */
	void StripComments(const wxString &text, wxString &stippedText);

	/**
	 * @brief return space delimited list of all unique string names in the database
	 * @param tagsList
	 */
	void GetAllTagsNames(wxArrayString &tagsList);

	/**
	 * @brief return normalize function signature. This function strips any default values or variable
	 * name from the signature. The return value for signature like this: wxT("int value, const std::string &str = "", void *data = NULL"), is "int, const std::string&, void *"
	 * and by setting the  includeVarNames to true, it will also returns the variables names
	 * @param sig signature
	 * @param includeVarNames set to true if the stripped signature should include the variables names. By default it is set to false
	 * @return stripped functions signature
	 */
	wxString NormalizeFunctionSig(const wxString &sig, size_t flags = Normalize_Func_Name, std::vector<std::pair<int, int> > *paramLen = NULL);

	/**
	 * @brief return map of un-implemented methods of given scope
	 * @param scopeName scope to search
	 * @param protos map of methods prototypes
	 */
	void GetUnImplementedFunctions(const wxString &scopeName, std::map<wxString, TagEntryPtr> &protos);

	/**
	 * @brief get list of virtual functions from the parent which were not override by
	 * the derived class (scopeName)
	 * @param scopeName derived class
	 * @param protos  [output]
	 */
	void GetUnOverridedParentVirtualFunctions(const wxString &scopeName, bool onlyPureVirtual, std::vector<TagEntryPtr> &protos);

	/**
	 * @brief send event to the file tree to mark tags file as bold
	 * @param bold
	 */
	void NotifyFileTree(bool bold);

	/**
	 * @brief update the 'last_retagged' column in the 'files' table for the current timestamp
	 * @param files list of files
	 * @brief db    database to use
	 */
	void UpdateFilesRetagTimestamp(const wxArrayString &files, ITagsStorage *db);

	/**
	 * @brief accept as input ctags pattern of a function and tries to evaluate the
	 * return value of the function
	 * @param pattern ctags pattern of the method
	 * @return return value of the method from the pattern of empty string
	 */
	wxString GetFunctionReturnValueFromPattern(const wxString &pattern);
	/**
	 * @brief fileter a recently tagged files from the strFiles array
	 * @param strFiles
	 * @param db
	 */
	void FilterNonNeededFilesForRetaging(wxArrayString &strFiles, ITagsStorage *db);

	/**
	 * Parse tags from memory and constructs a TagTree.
	 * This function throws a std::exception*.
	 * @param tags wxString containing the tags to parse
	 * @return tag tree, must be freed by caller
	 */
	TagTreePtr TreeFromTags(const wxString& tags, int &count);

	/**
	 * @brief lock/unlock the TagsManager locker
	 */
	void CrawlerLock();
	void CrawlerUnlock();

protected:
	std::map<wxString, bool> m_typeScopeCache;
	std::map<wxString, bool> m_typeScopeContainerCache;

	/**
	 * Handler ctags process termination
	 */
	void OnCtagsEnd    (wxProcessEvent &event);
	void OnTimer       (wxTimerEvent   &event);

	DECLARE_EVENT_TABLE()

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
	 *
	 * @param &path
	 * @param &derivationList
	 * @return
	 */
	bool GetDerivationList(const wxString &path, std::vector<wxString> &derivationList);

protected:
	// provide a default handler for the wxEVT_UPDATE_FILETREE_EVENT event
	void OnUpdateFileTreeEvent(wxCommandEvent &e);

protected:
	void           DoFindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags);
	void           RemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target);
	void           RemoveDuplicatesTips(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target);
	void           GetGlobalTags(const wxString &name, std::vector<TagEntryPtr> &tags, size_t flags = PartialMatch);
	void           GetLocalTags(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags, size_t flags = PartialMatch);
	void           TipsFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<wxString> &tips);
	void           GetFunctionTipFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<TagEntryPtr> &tips);
	DoxygenComment DoCreateDoxygenComment(TagEntryPtr tag, wxChar keyPrefix);
	bool           DoBuildDatabase(const wxArrayString &files, ITagsStorage &db, const wxString *rootPath = NULL);
	bool           ProcessExpression(const wxFileName &filename, int lineno, const wxString &expr, const wxString &scopeText, wxString &typeName, wxString &typeScope, wxString &oper, wxString &scopeTempalteInitiList);
	void           FilterImplementation(const std::vector<TagEntryPtr> &src, std::vector<TagEntryPtr> &tags);
	void           FilterDeclarations(const std::vector<TagEntryPtr> &src, std::vector<TagEntryPtr> &tags);
	wxString       DoReplaceMacros(wxString name);
	void           UpdateFileTree(const std::vector<wxFileName> &files, bool bold);
	void           UpdateFileTree(ITagsStorage *td, bool bold);
	void           DoFilterNonNeededFilesForRetaging(wxArrayString &strFiles, ITagsStorage *db);
	void           DoGetFunctionTipForEmptyExpression(const wxString &word, const wxString &text, std::vector<TagEntryPtr> &tips, bool globalScopeOnly = false);
};

/// create the singleton typedef
typedef Singleton<TagsManager> TagsManagerST;

#endif // CODELITE_CTAGS_MANAGER_H
