//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_database.h
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
#ifndef CODELITE_TAGS_DATABASE_H
#define CODELITE_TAGS_DATABASE_H

#include "codelite_exports.h"
#include "entry.h"
#include "fileentry.h"
#include "istorage.h"

#include <unordered_map>
#include <wx/filename.h>
#include <wx/wxsqlite3.h>

/**
 * TagsDatabase is a wrapper around wxSQLite3 database with tags specific functions.
 * It allows caller to query and populate the SQLite database for tags with a set of convenient functions.
 * this class is responsible for creating the schema for the CodeLite library.
 *
 * The tables are automatically created once a database is created
 *
 * Database tables:
 *
 * Table Name: TAGS
 *
 * || Column Name || Type || Description
 * |id            | Number | ID
 * |Name          | String | Tag name is appears in ctags output file
 * |File          | String | File that this tag was found in
 * |Line          | Number | Line number
 * |Kind          | String | Tag kind, can be one of: function, prototype, class, namespace, variable, member, enum,
 * enumerator, macro, project, union, typedef
 * |Access        | String | Can be one of public, private protected
 * |Signature     | String | For functions, this column holds the function signature
 * |Pattern       | String | pattern that can be used to located this tag in the file
 * |Parent        | String | tag direct parent, can be its class parent (for members or functions), namespace or the
 * literal "<global>"
 * |Inherits      | String | If this class/struct inherits from other class, it will contain the name of the base class
 * |Path          | String | full name including path, (e.g. Project::ClassName::FunctionName
 * |Typeref       | String | Special type of tag, that points to other Tag (i.e. typedef)
 *
 * Table Name: TAGS_VERSION
 *
 * || Column Name || Type || Description
 * | Version      | String | contains the current database schema
 *
 * Table Name: FILES
 *
 * || Column Name || Type || Description
 * | id           | Number | ID
 * | file         | String | Full path of the file
 * | last_retagged| Number | Timestamp for the last time this file was retagged
 *
 * Table Name: MACROS
 *
 * || Column Name   || Type || Description
 * | id             | Number | ID
 * | File           | String | The file where this macro was found
 * | Line           | int    | The line number where this macro was found
 * | Name           | String | The macro name
 * | IsFunctionLike | int    | Contains 1 if this entry is "function" like macro, 0 otherwise
 * | Replacement    | String | the replacement string (processed. i.e. %0..%N as placeholders for the arguments)
 * | Signature      | String | For macro of type 'IsFunctionLike', contains the signature in the form of (%0,...%N)
 *
 * @date 08-22-2006
 * @author Eran
 * @ingroup CodeLite
 */

class TagsStorageSQLiteCache
{
    std::unordered_map<wxString, std::vector<TagEntryPtr>> m_cache;

protected:
    bool DoGet(const wxString& key, std::vector<TagEntryPtr>& tags);
    void DoStore(const wxString& key, const std::vector<TagEntryPtr>& tags);

public:
    TagsStorageSQLiteCache() = default;
    virtual ~TagsStorageSQLiteCache();

    bool Get(const wxString& sql, std::vector<TagEntryPtr>& tags);
    bool Get(const wxString& sql, const wxArrayString& kind, std::vector<TagEntryPtr>& tags);
    void Store(const wxString& sql, const std::vector<TagEntryPtr>& tags);
    void Store(const wxString& sql, const wxArrayString& kind, const std::vector<TagEntryPtr>& tags);
    void Clear();
};

class WXDLLIMPEXP_CL clSqliteDB : public wxSQLite3Database
{
    std::unordered_map<wxString, wxSQLite3Statement> m_statements;

public:
    clSqliteDB()
        : wxSQLite3Database()
    {
    }

    void Close()
    {
        if (IsOpen())
            wxSQLite3Database::Close();

        m_statements.clear();
    }

    wxSQLite3Statement GetPrepareStatement(const wxString& sql) { return wxSQLite3Database::PrepareStatement(sql); }
};

class WXDLLIMPEXP_CL TagsStorageSQLite : public ITagsStorage
{
    clSqliteDB* m_db;
    TagsStorageSQLiteCache m_cache;

private:
    /**
     * @brief fetch tags from the database
     * @param sql
     * @param tags
     */
    void DoFetchTags(const wxString& sql, std::vector<TagEntryPtr>& tags);

    /**
     * @brief
     * @param sql
     * @param tags
     */
    void DoFetchTags(const wxString& sql, std::vector<TagEntryPtr>& tags, const wxArrayString& kinds);

    void DoAddNamePartToQuery(wxString& sql, const wxString& name, bool partial, bool prependAnd);
    void DoAddLimitPartToQuery(wxString& sql, const std::vector<TagEntryPtr>& tags);

public:
    static TagEntry* FromSQLite3ResultSet(wxSQLite3ResultSet& rs);
    static void PPTokenFromSQlite3ResultSet(wxSQLite3ResultSet& rs, PPToken& token);

public:
    /**
     * Execute a query sql and return result set.
     * @param sql Select statement
     * @param path Database file to use
     * @return result set
     */
    wxSQLite3ResultSet Query(const wxString& sql, const wxFileName& path = wxFileName());

    /**
     * Construct a tags database.
     */
    TagsStorageSQLite();

    /**
     *
     * Destructor
     */
    virtual ~TagsStorageSQLite();

    virtual void SetUseCache(bool useCache);

    /**
     * Return the currently opened database.
     * @return Currently open database
     */
    const wxFileName& GetDatabaseFileName() const { return m_fileName; }

    /**
     * Open sqlite database.
     * @param fileName Database file name
     */
    void OpenDatabase(const wxFileName& fileName);

    /**
     * @brief reopen the database. If it is already opened - close it before
     */
    void ReOpenDatabase();

    /**
     * Create database if not existed already.
     */
    void CreateSchema();

    /**
     * Return a result set of tags according to file name.
     * @param file Source file name
     * @param path Database file name
     * @return result set
     */
    virtual void
    SelectTagsByFile(const wxString& file, std::vector<TagEntryPtr>& tags, const wxFileName& path = wxFileName());

    /**
     * Delete all entries from database that are related to filename.
     * @param path Database name
     * @param fileName File name
     * @param autoCommit handle the Delete operation inside a transaction or let the user handle it
     */
    void DeleteByFileName(const wxFileName& path, const wxString& fileName, bool autoCommit = true);

    /**
     * @brief determine the current scope based on file name and line number
     */
    virtual TagEntryPtr GetScope(const wxString& filename, int line_number);

    /**
     * Begin transaction.
     */
    void Begin()
    {
        try {
            m_db->Begin();
        } catch (const wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
    }

    /**
     * Commit transaction.
     */
    void Commit()
    {
        try {
            m_db->Commit();
        } catch (const wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
    }

    /**
     * Rollback transaction.
     */
    void Rollback() { return m_db->Rollback(); }

    /**
     * Test whether the database is opened
     * @return true if database is attached to a file
     */
    bool IsOpen() const;

    /**
     * Return SQLite3 prepare statement object
     * @param sql sql
     * @return wxSQLite3ResultSet object
     */
    wxSQLite3Statement PrepareStatement(const wxString& sql) { return m_db->PrepareStatement(sql); }

    /**
     * Execute query
     * @param sql sql to execute
     */
    void ExecuteUpdate(const wxString& sql);

    /**
     * return list of files from the database. The returned list is ordered
     * by name (ascending)
     * @param partialName part of the file name to act as a filter
     * @param files [output] array of files
     */
    void GetFiles(const wxString& partialName, std::vector<FileEntryPtr>& files);

    /**
     * @brief return list of files from the database
     * @param files vector of database record
     */
    void GetFiles(std::vector<FileEntryPtr>& files);

    //----------------------------------------------------------
    //----------------------------------------------------------
    //----------------------------------------------------------
    //       Implementation of the IStorage methods
    //----------------------------------------------------------
    //----------------------------------------------------------
    //----------------------------------------------------------

    //----------------------------- Meta Data ---------------------------------------

    /**
     * Return the current version of the database library .
     * @return current version of the database library
     */
    const wxString& GetVersion() const;

    /**
     * Schema version as appears in TAGS_VERSION table
     * @return schema's version
     */
    wxString GetSchemaVersion() const;

    // --------------------------------------------------------------------------------------------

    /**
     * @brief return array of tags by kind.
     * @param kinds array of kinds
     * @param orderingColumn the column that the output should be ordered by (leave empty for no sorting)
     * @param order OrderAsc, OrderDesc or use OrderNone for no ordering the results
     * @param tags [output]
     */
    virtual void GetTagsByKind(const wxArrayString& kinds,
                               const wxString& orderingColumn,
                               int order,
                               std::vector<TagEntryPtr>& tags);

    /**
     * @brief return array of items by path
     * @param path
     * @param tags
     */
    virtual void GetTagsByPath(const wxArrayString& path, std::vector<TagEntryPtr>& tags);
    /**
     * @brief return array of items by name and parent
     * @param path
     * @param tags
     */
    virtual void GetTagsByNameAndParent(const wxString& name, const wxString& parent, std::vector<TagEntryPtr>& tags);

    /**
     * @brief return array of tags by kind and path
     * @param kinds array of kind
     * @param path
     * @param tags  [output]
     */
    virtual void GetTagsByKindAndPath(const wxArrayString& kinds, const wxString& path, std::vector<TagEntryPtr>& tags);

    /**
     * @brief return list of tags by scopes and kinds
     * @param scopes array of possible scopes
     * @param kinds array of possible kinds
     * @param tags [output]
     */
    virtual void
    GetTagsByScopesAndKind(const wxArrayString& scopes, const wxArrayString& kinds, std::vector<TagEntryPtr>& tags);

    /**
     * @brief get list of tags by kind and file
     * @param kind
     * @param orderingColumn the column that the output should be ordered by (leave empty for no sorting)
     * @param order OrderAsc, OrderDesc or use OrderNone for no ordering the results
     * @param tags
     */
    virtual void GetTagsByKindAndFile(const wxArrayString& kind,
                                      const wxString& fileName,
                                      const wxString& orderingColumn,
                                      int order,
                                      std::vector<TagEntryPtr>& tags);

    /**
     * @brief delete an entry by file name
     * @param filename
     * @return
     */
    virtual int DeleteFileEntry(const wxString& filename);

    /**
     * @brief
     * @param typeName
     * @param scope
     * @return
     */
    virtual bool IsTypeAndScopeExistLimitOne(const wxString& typeName, const wxString& scope);

    /**
     * @brief return true if type & scope do exist in the symbols database and is container
     * @param typeName
     * @param scope
     * @return
     */
    virtual bool IsTypeAndScopeExist(wxString& typeName, wxString& scope);

    /**
     * @brief
     */
    virtual void ClearCache();

    /**
     * @brief
     * @param name
     * @return
     */
    virtual PPToken GetMacro(const wxString& name);

    /**
     * @brief search for a single match in the database for an entry with a given name
     */
    TagEntryPtr GetTagsByNameLimitOne(const wxString& name);
};

#endif // CODELITE_TAGS_DATABASE_H
