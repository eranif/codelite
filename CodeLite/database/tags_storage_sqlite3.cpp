//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_database.cpp
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
#include "precompiled_header.h"

#include "tags_storage_sqlite3.h"

#include "file_logger.h"
#include "fileutils.h"
#include "macros.h"

#include <algorithm>
#include <unordered_set>
#include <wx/longlong.h>
#include <wx/tokenzr.h>

//-------------------------------------------------
// Tags database class implementation
//-------------------------------------------------
TagsStorageSQLite::TagsStorageSQLite()
    : ITagsStorage()
{
    m_db = new clSqliteDB();
    SetUseCache(true);
}

TagsStorageSQLite::~TagsStorageSQLite()
{
    if (m_db) {
        m_db->Close();
        delete m_db;
        m_db = NULL;
    }
}

void TagsStorageSQLite::OpenDatabase(const wxFileName& fileName)
{
    if (m_fileName.GetFullPath() == fileName.GetFullPath())
        return;

    // Did we get a file name to use?
    if (!fileName.IsOk() && !m_fileName.IsOk())
        return;

    // We did not get any file name to use BUT we
    // do have an open database, so we will use it
    if (!fileName.IsOk())
        return;

    try {
        if (!m_fileName.IsOk()) {
            // First time we open the db
            m_db->Open(fileName.GetFullPath());
            m_db->SetBusyTimeout(10);
            CreateSchema();
            m_fileName = fileName;

        } else {
            // We have both fileName & m_fileName and they
            // are different, Close previous db
            m_db->Close();
            m_db->Open(fileName.GetFullPath());
            m_db->SetBusyTimeout(10);
            CreateSchema();
            m_fileName = fileName;
        }

    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "Failed to open file:" << m_fileName.GetFullPath() << "." << e.GetMessage();
    }
}

void TagsStorageSQLite::CreateSchema()
{
    wxString sql;

    // improve performance by using pragma command:
    // (this needs to be done before the creation of the
    // tables and indices)
    try {
        sql = wxT("PRAGMA journal_mode= OFF;");
        m_db->ExecuteUpdate(sql);

        sql = wxT("PRAGMA synchronous = OFF;");
        m_db->ExecuteUpdate(sql);

        sql = wxT("PRAGMA temp_store = MEMORY;");
        m_db->ExecuteUpdate(sql);

        sql = wxT("PRAGMA case_sensitive_like = 0;");
        m_db->ExecuteUpdate(sql);

        sql = wxT("create  table if not exists tags (ID INTEGER PRIMARY KEY AUTOINCREMENT, name string, file string, "
                  "line integer, kind string, access string, signature string, pattern string, parent string, inherits "
                  "string, path string, typeref string, scope string, template_definition string, tag_properties "
                  "string, macrodef string);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("create  table if not exists global_tags (ID INTEGER PRIMARY KEY AUTOINCREMENT, name string, tag_id "
                  "integer)");
        m_db->ExecuteUpdate(sql);

        sql = wxT("create  table if not exists FILES (ID INTEGER PRIMARY KEY AUTOINCREMENT, file string, last_retagged "
                  "integer);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("create  table if not exists MACROS (ID INTEGER PRIMARY KEY AUTOINCREMENT, file string, line "
                  "integer, name string, is_function_like int, replacement string, signature string);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("create  table if not exists SIMPLE_MACROS (ID INTEGER PRIMARY KEY AUTOINCREMENT, file string, name "
                  "string);");
        m_db->ExecuteUpdate(sql);

        // create unique index on Files' file column
        sql = wxT("CREATE UNIQUE INDEX IF NOT EXISTS FILES_NAME on FILES(file)");
        m_db->ExecuteUpdate(sql);

        // Create a trigger that makes sure that whenever a record is deleted
        // from the TAGS table, the corresponded entry is also deleted from the
        // global_tags
        wxString trigger1 = wxT("CREATE TRIGGER IF NOT EXISTS tags_delete AFTER DELETE ON tags ") wxT("FOR EACH ROW ")
            wxT("BEGIN ") wxT("    DELETE FROM global_tags WHERE global_tags.tag_id = OLD.id;") wxT("END;");

        m_db->ExecuteUpdate(trigger1);

        wxString trigger2 = wxT("CREATE TRIGGER IF NOT EXISTS tags_insert AFTER INSERT ON tags ")
            wxT("FOR EACH ROW WHEN NEW.scope = '<global>' ") wxT("BEGIN ")
                wxT("    INSERT INTO global_tags (id, name, tag_id) VALUES (NULL, NEW.name, NEW.id);") wxT("END;");
        m_db->ExecuteUpdate(trigger2);

        // Create unique index on tags table
        sql = wxT("CREATE UNIQUE INDEX IF NOT EXISTS TAGS_UNIQ on tags(file, kind, path, signature, typeref, "
                  "template_definition);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS KIND_IDX on tags(kind);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS FILE_IDX on tags(file);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE UNIQUE INDEX IF NOT EXISTS MACROS_UNIQ on MACROS(name);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS global_tags_idx_1 on global_tags(name);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS global_tags_idx_2 on global_tags(tag_id);");
        m_db->ExecuteUpdate(sql);

        // Create search indexes
        sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_NAME on tags(name);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_SCOPE on tags(scope);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_PATH on tags(path);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_PARENT on tags(parent);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS TAGS_TYPEREF on tags(typeref);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS MACROS_NAME on MACROS(name);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("CREATE INDEX IF NOT EXISTS SIMPLE_MACROS_FILE on SIMPLE_MACROS(file);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("create table if not exists tags_version (version string primary key);");
        m_db->ExecuteUpdate(sql);

        sql = wxT("create unique index if not exists tags_version_uniq on tags_version(version);");
        m_db->ExecuteUpdate(sql);

        sql = wxString(wxT("replace into tags_version values ('")) << GetVersion() << wxT("');");
        m_db->ExecuteUpdate(sql);

    } catch (const wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

wxString TagsStorageSQLite::GetSchemaVersion() const
{
    // return the current schema version
    try {
        wxString sql;
        wxString version;
        sql = wxT("SELECT * FROM TAGS_VERSION");
        wxSQLite3ResultSet rs = m_db->ExecuteQuery(sql);

        if (rs.NextRow())
            version = rs.GetString(0);
        return version;
    } catch (const wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return wxEmptyString;
}

#define SAFE_ROLLBACK_IF_NEEDED(Auto_Commit) \
    try {                                    \
        if (Auto_Commit)                     \
            m_db->Rollback();                \
    } catch (const wxSQLite3Exception&) {    \
    }

void TagsStorageSQLite::SelectTagsByFile(const wxString& file, std::vector<TagEntryPtr>& tags, const wxFileName& path)
{
    // Incase empty file path is provided, use the current file name
    wxFileName databaseFileName(path);
    path.IsOk() == false ? databaseFileName = m_fileName : databaseFileName = path;
    OpenDatabase(databaseFileName);

    wxString query;
    query << wxT("select * from tags where file='") << file << "' ";
    // #ifdef __WXMSW__
    //     // Under Windows, the file-crawler changes the file path
    //     // to lowercase. However, the database matches the file name
    //     // by case-sensitive
    //     query << "COLLATE NOCASE ";
    // #endif
    query << wxT("order by line asc");
    DoFetchTags(query, tags);
}

void TagsStorageSQLite::DeleteByFileName(const wxFileName& path, const wxString& fileName, bool autoCommit)
{
    // make sure database is open
    try {
        OpenDatabase(path);

        if (autoCommit) {
            m_db->Begin();
        }

        wxString sql;
        sql << "delete from tags where File='" << fileName << "'";
        m_db->ExecuteUpdate(sql);
        if (autoCommit)
            m_db->Commit();
    } catch (const wxSQLite3Exception& e) {
        wxUnusedVar(e);
        if (autoCommit) {
            m_db->Rollback();
        }
    }
    // also remove the file entry associated with this file
    DeleteFileEntry(fileName);
}

wxSQLite3ResultSet TagsStorageSQLite::Query(const wxString& sql, const wxFileName& path)
{
    // make sure database is open
    try {
        OpenDatabase(path);
        return m_db->ExecuteQuery(sql);
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "Query error:" << sql << "." << e.GetMessage();
        if (e.GetMessage().Contains("disk I/O error")) {
            ReOpenDatabase();
        }
    }
    return wxSQLite3ResultSet();
}

void TagsStorageSQLite::ExecuteUpdate(const wxString& sql)
{
    try {
        m_db->ExecuteUpdate(sql);
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "ExecuteUpdate error:" << sql << "." << e.GetMessage();
    }
}

bool TagsStorageSQLite::IsOpen() const { return m_db->IsOpen(); }

void TagsStorageSQLite::GetFiles(const wxString& partialName, std::vector<FileEntryPtr>& files)
{
    try {
        bool match_path = (!partialName.IsEmpty() && partialName.Last() == wxFileName::GetPathSeparator());

        wxString query;
        wxString tmpName(partialName);
        tmpName.Replace(wxT("_"), wxT("^_"));
        query << wxT("select * from files where file like '%%") << tmpName << wxT("%%' ESCAPE '^' ")
              << wxT("order by file");

        wxSQLite3ResultSet res = m_db->ExecuteQuery(query);
        while (res.NextRow()) {

            FileEntryPtr fe(new FileEntry());
            fe->SetId(res.GetInt(0));
            fe->SetFile(res.GetString(1));
            fe->SetLastRetaggedTimestamp(res.GetInt(2));

            wxFileName fileName(fe->GetFile());
            wxString match = match_path ? fileName.GetFullPath() : fileName.GetFullName();

// Under Windows, all files are stored as lower case in the
// database (see fc_fileopener.cpp normalize_path method
#ifdef __WXMSW__
            wxString lowerCasePartialName(partialName);
            lowerCasePartialName.MakeLower();
            match.MakeLower();
#else
            wxString lowerCasePartialName(partialName);
#endif
            if (match.StartsWith(lowerCasePartialName)) {
                files.emplace_back(std::move(fe));
            }
        }
    } catch (const wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void TagsStorageSQLite::GetFiles(std::vector<FileEntryPtr>& files)
{
    try {
        wxString query(wxT("select * from files order by file"));
        wxSQLite3ResultSet res = m_db->ExecuteQuery(query);

        // Pre allocate a reasonable amount of entries
        files.reserve(5000);

        while (res.NextRow()) {
            FileEntryPtr fe(new FileEntry());
            fe->SetId(res.GetInt(0));
            fe->SetFile(res.GetString(1));
            fe->SetLastRetaggedTimestamp(res.GetInt(2));

            files.push_back(std::move(fe));
        }
        // release unneeded memory
        files.shrink_to_fit();

    } catch (const wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void TagsStorageSQLite::PPTokenFromSQlite3ResultSet(wxSQLite3ResultSet& rs, PPToken& token)
{
    // set the name
    token.name = rs.GetString(3);

    bool isFunctionLike = rs.GetInt(4) == 0 ? false : true;

    // set the flags
    token.flags = PPToken::IsValid;
    if (isFunctionLike)
        token.flags |= PPToken::IsFunctionLike;

    token.line = rs.GetInt(2);
    token.replacement = rs.GetString(5);

    wxString sig = rs.GetString(6);
    sig.Replace(wxT("("), wxT(""));
    sig.Replace(wxT(")"), wxT(""));
    token.args = wxStringTokenize(sig, wxT(","), wxTOKEN_STRTOK);
}

TagEntry* TagsStorageSQLite::FromSQLite3ResultSet(wxSQLite3ResultSet& rs)
{
    TagEntry* entry = new TagEntry();
    entry->SetId(rs.GetInt(0));
    entry->SetName(rs.GetString(1));
    entry->SetFile(rs.GetString(2));
    entry->SetLine(rs.GetInt(3));
    entry->SetKind(rs.GetString(4));
    entry->SetAccess(rs.GetString(5));
    entry->SetSignature(rs.GetString(6));
    entry->SetPattern(rs.GetString(7));
    entry->SetParent(rs.GetString(8));
    entry->SetInherits(rs.GetString(9));
    entry->SetPath(rs.GetString(10));
    entry->SetTypename(rs.GetString(11));
    entry->SetScope(rs.GetString(12));
    entry->SetTemplateDefinition(rs.GetString(13));
    entry->SetTagProperties(rs.GetString(14));
    entry->SetMacrodef(rs.GetString(15));
    return entry;
}

void TagsStorageSQLite::DoFetchTags(const wxString& sql, std::vector<TagEntryPtr>& tags)
{
    if (GetUseCache() && m_cache.Get(sql, tags)) {
        return;
    }

    LOG_IF_TRACE { clDEBUG1() << "Fetching from disk:" << sql << clEndl; }
    tags.reserve(1000);

    try {
        wxSQLite3ResultSet ex_rs;
        ex_rs = Query(sql);

        // add results from external database to the workspace database
        while (ex_rs.NextRow()) {
            // Construct a TagEntry from the record set
            TagEntryPtr tag(FromSQLite3ResultSet(ex_rs));
            // convert the path to be real path
            tags.push_back(tag);
        }
        ex_rs.Finalize();
    } catch (const wxSQLite3Exception& e) {
        LOG_IF_DEBUG
        {
            clDEBUG() << "SQLite exception!" << endl;
            clDEBUG() << e.GetMessage() << endl;
        }
    }

    LOG_IF_TRACE { clDEBUG1() << "Fetching from disk...done" << tags.size() << "matches found" << clEndl; }
    if (GetUseCache()) {
        m_cache.Store(sql, tags);
    }
}

void TagsStorageSQLite::DoFetchTags(const wxString& sql, std::vector<TagEntryPtr>& tags, const wxArrayString& kinds)
{
    if (GetUseCache() && m_cache.Get(sql, kinds, tags))
        return;

    wxStringSet_t set_kinds;
    set_kinds.insert(kinds.begin(), kinds.end());
    tags.reserve(1000);

    LOG_IF_TRACE { clDEBUG1() << "Fetching from disk:" << sql << endl; }
    try {
        wxSQLite3ResultSet ex_rs;
        ex_rs = Query(sql);

        // add results from external database to the workspace database
        while (ex_rs.NextRow()) {
            // check if this kind is acceptable
            if (set_kinds.count(ex_rs.GetString(4))) {

                // Construct a TagEntry from the record set
                TagEntryPtr tag(FromSQLite3ResultSet(ex_rs));

                // convert the path to be real path
                tags.push_back(tag);
            }
        }
        ex_rs.Finalize();

    } catch (const wxSQLite3Exception& e) {
        LOG_IF_DEBUG
        {
            clDEBUG() << e.GetMessage() << endl;
            clDEBUG() << "SQLite exception!" << endl;
        }
    }
    LOG_IF_TRACE { clDEBUG1() << "Fetching from disk...done" << tags.size() << "matches found" << endl; }
    if (GetUseCache()) {
        m_cache.Store(sql, kinds, tags);
    }
}

void TagsStorageSQLite::GetTagsByKind(const wxArrayString& kinds,
                                      const wxString& orderingColumn,
                                      int order,
                                      std::vector<TagEntryPtr>& tags)
{
    wxString sql;
    sql << wxT("select * from tags where kind in (");
    for (size_t i = 0; i < kinds.GetCount(); i++) {
        sql << wxT("'") << kinds.Item(i) << wxT("',");
    }
    sql.RemoveLast();
    sql << wxT(") ");

    if (orderingColumn.IsEmpty() == false) {
        sql << wxT("order by ") << orderingColumn;
        switch (order) {
        case ITagsStorage::OrderAsc:
            sql << wxT(" ASC");
            break;
        case ITagsStorage::OrderDesc:
            sql << wxT(" DESC");
            break;
        case ITagsStorage::OrderNone:
        default:
            break;
        }
    }

    DoFetchTags(sql, tags);
}

void TagsStorageSQLite::GetTagsByPath(const wxArrayString& path, std::vector<TagEntryPtr>& tags)
{
    if (path.empty())
        return;

    wxString sql;

    sql << wxT("select * from tags where path IN(");
    for (size_t i = 0; i < path.GetCount(); i++) {
        sql << wxT("'") << path.Item(i) << wxT("',");
    }
    sql.RemoveLast();
    sql << wxT(")");
    DoFetchTags(sql, tags);
}

void TagsStorageSQLite::GetTagsByNameAndParent(const wxString& name,
                                               const wxString& parent,
                                               std::vector<TagEntryPtr>& tags)
{
    wxString sql;
    sql << wxT("select * from tags where name='") << name << wxT("' LIMIT ") << GetSingleSearchLimit();

    std::vector<TagEntryPtr> tmpResults;
    DoFetchTags(sql, tmpResults);

    // Filter by parent
    for (auto tmpResult : tmpResults) {
        if (tmpResult->GetParent() == parent) {
            tags.push_back(tmpResult);
        }
    }
}

void TagsStorageSQLite::GetTagsByKindAndPath(const wxArrayString& kinds,
                                             const wxString& path,
                                             std::vector<TagEntryPtr>& tags)
{
    if (kinds.empty()) {
        return;
    }

    wxString sql;
    sql << wxT("select * from tags where path='") << path << wxT("' LIMIT ") << GetSingleSearchLimit();

    DoFetchTags(sql, tags, kinds);
}

void TagsStorageSQLite::GetTagsByKindAndFile(const wxArrayString& kind,
                                             const wxString& fileName,
                                             const wxString& orderingColumn,
                                             int order,
                                             std::vector<TagEntryPtr>& tags)
{
    if (kind.empty()) {
        return;
    }

    wxString sql;
    sql << wxT("select * from tags where file='") << fileName << wxT("' and kind in (");

    for (size_t i = 0; i < kind.GetCount(); i++) {
        sql << wxT("'") << kind.Item(i) << wxT("',");
    }
    sql.RemoveLast();
    sql << wxT(")");

    if (orderingColumn.IsEmpty() == false) {
        sql << wxT("order by ") << orderingColumn;
        switch (order) {
        case ITagsStorage::OrderAsc:
            sql << wxT(" ASC");
            break;
        case ITagsStorage::OrderDesc:
            sql << wxT(" DESC");
            break;
        case ITagsStorage::OrderNone:
        default:
            break;
        }
    }
    DoFetchTags(sql, tags);
}

int TagsStorageSQLite::DeleteFileEntry(const wxString& filename)
{
    try {
        wxSQLite3Statement statement = m_db->GetPrepareStatement(wxT("DELETE FROM FILES WHERE FILE=?"));
        statement.Bind(1, filename);
        statement.ExecuteUpdate();

    } catch (const wxSQLite3Exception& exc) {
        if (exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
            return TagExist;
        return TagError;
    }
    return TagOk;
}

bool TagsStorageSQLite::IsTypeAndScopeExist(wxString& typeName, wxString& scope)
{
    wxString sql;
    wxString strippedName;
    wxString secondScope;
    wxString bestScope;
    wxString parent;
    wxString tmpScope(scope);

    strippedName = typeName.AfterLast(wxT(':'));
    secondScope = typeName.BeforeLast(wxT(':'));

    if (secondScope.EndsWith(wxT(":")))
        secondScope.RemoveLast();

    if (strippedName.IsEmpty())
        return false;

    sql << wxT("select scope,parent from tags where name='") << strippedName
        << wxT("' and kind in ('class', 'struct', 'typedef') LIMIT 50");
    int foundOther(0);
    wxString scopeFounded;
    wxString parentFounded;

    if (secondScope.IsEmpty() == false)
        tmpScope << wxT("::") << secondScope;

    parent = tmpScope.AfterLast(wxT(':'));

    try {
        wxSQLite3ResultSet rs = Query(sql);
        while (rs.NextRow()) {

            scopeFounded = rs.GetString(0);
            parentFounded = rs.GetString(1);

            if (scopeFounded == tmpScope) {
                // exact match
                scope = scopeFounded;
                typeName = strippedName;
                return true;

            } else if (parentFounded == parent) {
                bestScope = scopeFounded;

            } else {
                foundOther++;
            }
        }

    } catch (const wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }

    // if we reached here, it means we did not find any exact match
    if (bestScope.IsEmpty() == false) {
        scope = bestScope;
        typeName = strippedName;
        return true;

    } else if (foundOther == 1) {
        scope = scopeFounded;
        typeName = strippedName;
        return true;
    }
    return false;
}

void TagsStorageSQLite::GetTagsByScopesAndKind(const wxArrayString& scopes,
                                               const wxArrayString& kinds,
                                               std::vector<TagEntryPtr>& tags)
{
    if (kinds.empty() || scopes.empty()) {
        return;
    }

    // fetch from the scopes, in-order (i.e. first scope tags and so on)
    for (const wxString& scope : scopes) {
        wxString sql;
        sql << "select * from tags where scope = '" << scope << "' ORDER BY NAME";
        DoAddLimitPartToQuery(sql, tags);

        std::vector<TagEntryPtr> scope_results;
        DoFetchTags(sql, scope_results, kinds);
        tags.reserve(tags.size() + scope_results.size());
        tags.insert(tags.end(), scope_results.begin(), scope_results.end());
        if ((GetSingleSearchLimit() > 0) && (static_cast<int>(tags.size()) > GetSingleSearchLimit())) {
            break;
        }
    }
}

bool TagsStorageSQLite::IsTypeAndScopeExistLimitOne(const wxString& typeName, const wxString& scope)
{
    wxString sql;
    wxString path;

    // Build the path
    if (scope.IsEmpty() == false && scope != wxT("<global>"))
        path << scope << wxT("::");

    path << typeName;
    sql << wxT("select ID from tags where path='") << path
        << wxT("' and kind in ('class', 'struct', 'typedef') LIMIT 1");

    try {
        wxSQLite3ResultSet rs = Query(sql);
        if (rs.NextRow()) {
            return true;
        }

    } catch (const wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return false;
}

//---------------------------------------------------------------------
//-----------------------------TagsStorageSQLiteCache -----------------
//---------------------------------------------------------------------

TagsStorageSQLiteCache::~TagsStorageSQLiteCache() { m_cache.clear(); }

bool TagsStorageSQLiteCache::Get(const wxString& sql, std::vector<TagEntryPtr>& tags) { return DoGet(sql, tags); }

bool TagsStorageSQLiteCache::Get(const wxString& sql, const wxArrayString& kind, std::vector<TagEntryPtr>& tags)
{
    wxString key;
    key << sql;
    for (size_t i = 0; i < kind.GetCount(); i++) {
        key << wxT("@") << kind.Item(i);
    }

    return DoGet(key, tags);
}

void TagsStorageSQLiteCache::Store(const wxString& sql, const std::vector<TagEntryPtr>& tags) { DoStore(sql, tags); }

void TagsStorageSQLiteCache::Clear() { m_cache.clear(); }

void TagsStorageSQLiteCache::Store(const wxString& sql, const wxArrayString& kind, const std::vector<TagEntryPtr>& tags)
{
    wxString key;
    key << sql;
    for (size_t i = 0; i < kind.GetCount(); i++) {
        key << wxT("@") << kind.Item(i);
    }
    DoStore(key, tags);
}

bool TagsStorageSQLiteCache::DoGet(const wxString& key, std::vector<TagEntryPtr>& tags)
{
    auto iter = m_cache.find(key);
    if (iter != m_cache.end()) {
        // Append the results to the output tags
        tags.reserve(tags.size() + iter->second.size());
        tags.insert(tags.end(), iter->second.begin(), iter->second.end());
        return true;
    }
    return false;
}

void TagsStorageSQLiteCache::DoStore(const wxString& key, const std::vector<TagEntryPtr>& tags)
{
    if (m_cache.count(key)) {
        m_cache.erase(key);
    }

    // avoid storing entries with __anon entries
    // since these tags will change their anonymous space
    // each time we save the file
    bool can_cache = true;
    for (auto tag : tags) {
        if (tag->GetScope().StartsWith("__anon")) {
            can_cache = false;
            break;
        }
    }

    if (can_cache) {
        m_cache.insert({key, tags});
    }
}

void TagsStorageSQLite::ClearCache()
{
    // clear the cache
    m_cache.Clear();
}

PPToken TagsStorageSQLite::GetMacro(const wxString& name)
{
    PPToken token;
    try {
        wxString sql;
        sql << wxT("select * from MACROS where name = '") << name << wxT("'");
        wxSQLite3ResultSet res = m_db->ExecuteQuery(sql);
        if (res.NextRow()) {
            PPTokenFromSQlite3ResultSet(res, token);
            return token;
        }
    } catch (const wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }

    return token;
}

void TagsStorageSQLite::SetUseCache(bool useCache) { ITagsStorage::SetUseCache(useCache); }

void TagsStorageSQLite::DoAddNamePartToQuery(wxString& sql, const wxString& name, bool partial, bool prependAnd)
{
    if (name.empty())
        return;
    if (prependAnd) {
        sql << wxT(" AND ");
    }

    if (m_enableCaseInsensitive) {
        wxString tmpName(name);
        tmpName.Replace(wxT("_"), wxT("^_"));
        if (partial) {
            sql << wxT(" name LIKE '") << tmpName << wxT("%%' ESCAPE '^' ");
        } else {
            sql << wxT(" name ='") << name << wxT("' ");
        }
    } else {
        // Don't use LIKE
        wxString from = name;
        wxString until = name;

        wxChar ch = until.Last();
        until.SetChar(until.length() - 1, ch + 1);

        // add the name condition
        if (partial) {
            sql << wxT(" name >= '") << from << wxT("' AND  name < '") << until << wxT("'");
        } else {
            sql << wxT(" name ='") << name << wxT("' ");
        }
    }
}

void TagsStorageSQLite::DoAddLimitPartToQuery(wxString& sql, const std::vector<TagEntryPtr>& tags)
{
    if (tags.size() >= (size_t)GetSingleSearchLimit()) {
        sql << wxT(" LIMIT 1 ");
    } else {
        sql << wxT(" LIMIT ") << (size_t)GetSingleSearchLimit() - tags.size();
    }
}

TagEntryPtr TagsStorageSQLite::GetTagsByNameLimitOne(const wxString& name)
{
    try {
        if (name.IsEmpty())
            return NULL;

        std::vector<TagEntryPtr> tags;
        wxString sql;
        sql << wxT("select * from tags where ");
        DoAddNamePartToQuery(sql, name, false, false);
        sql << wxT(" LIMIT 1 ");

        DoFetchTags(sql, tags);
        if (tags.size() == 1)
            return tags.at(0);
        else
            return NULL;

    } catch (const wxSQLite3Exception& e) {
        clDEBUG() << e.GetMessage() << endl;
    }
    return NULL;
}

const wxString& TagsStorageSQLite::GetVersion() const
{
    static const wxString gTagsDatabaseVersion = "CodeLite v16.0.5";
    return gTagsDatabaseVersion;
}

void TagsStorageSQLite::ReOpenDatabase()
{
    // Did we get a file name to use?
    if (!m_fileName.IsOk())
        return;

    clDEBUG() << "ReOpenDatabase called for file:" << m_fileName;
    // Close database first
    clDEBUG() << "Closing database first";
    try {
        if (m_db) {
            m_db->Close();
            delete m_db;
            m_db = nullptr;
        }
    } catch (...) {
    }

    clDEBUG() << "Open is called for file:" << m_fileName;
    try {
        // First time we open the db
        m_db->Open(m_fileName.GetFullPath());
        m_db->SetBusyTimeout(10);
        CreateSchema();
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "Failed to reopen file:" << m_fileName.GetFullPath() << "." << e.GetMessage();
    }
    clDEBUG() << "Database reopened successfully";
}

TagEntryPtr TagsStorageSQLite::GetScope(const wxString& filename, int line_number)
{
    if (filename.empty() || line_number == wxNOT_FOUND)
        return nullptr;

    wxString sql;
    sql << "select * from tags where file='" << filename << "' and line <= " << line_number
        << " and name NOT LIKE '__anon%' and KIND IN ('function', 'class', 'struct', 'namespace') order by line desc "
           "limit 1";
    LOG_IF_TRACE { clDEBUG1() << "Running SQL:" << sql << endl; }
    std::vector<TagEntryPtr> tags;
    DoFetchTags(sql, tags);

    if (tags.size() == 1) {
        return tags[0];
    }
    return nullptr;
}
