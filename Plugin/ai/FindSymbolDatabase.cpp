//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2026 by Eran Ifrah
// file name            : FindSymbolDatabase.cpp
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "FindSymbolDatabase.h"

#include "file_logger.h"

std::unique_ptr<FindSymbolDatabase> FindSymbolDatabase::OpenDatabase(const wxFileName& fileName)
{
    if (!fileName.IsOk()) {
        return nullptr;
    }

    auto db = std::make_unique<FindSymbolDatabase>();
    wxFileName dbFile(fileName);
    dbFile.Mkdir(wxS(""), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    try {
        db->Open(dbFile.GetFullPath());
        db->SetBusyTimeout(10);
        db->CreateSchema();
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "FindSymbolDatabase::OpenDatabase failed for '" << dbFile.GetFullPath()
                    << "': " << e.GetMessage();
        return nullptr;
    }
    return db;
}

void FindSymbolDatabase::CreateSchema()
{
    try {
        ExecuteUpdate(wxT("PRAGMA journal_mode= OFF;"));
        ExecuteUpdate(wxT("PRAGMA synchronous = OFF;"));
        ExecuteUpdate(wxT("PRAGMA temp_store = MEMORY;"));
        ExecuteUpdate(wxT("PRAGMA case_sensitive_like = 0;"));

        ExecuteUpdate(wxT("create table if not exists symbols ("
                          "id integer primary key autoincrement,"
                          "kind integer not null,"
                          "name text not null,"
                          "scope text,"
                          "qualified_name text,"
                          "file_path text not null,"
                          "line integer not null,"
                          "end_line integer,"
                          "search_text text,"
                          "workspace_rel_path text,"
                          "file_ext text,"
                          "updated_at integer);"));
        ExecuteUpdate(wxT("create table if not exists workspace_meta (key text primary key, value text not null);"));
        ExecuteUpdate(wxT("create index if not exists symbols_kind_idx on symbols(kind);"));
        ExecuteUpdate(wxT("create index if not exists symbols_name_idx on symbols(name);"));
        ExecuteUpdate(wxT("create index if not exists symbols_scope_idx on symbols(scope);"));
        ExecuteUpdate(wxT("create index if not exists symbols_qname_idx on symbols(qualified_name);"));
        ExecuteUpdate(wxT("create index if not exists symbols_path_idx on symbols(file_path);"));
        ExecuteUpdate(wxT("create index if not exists symbols_search_idx on symbols(search_text);"));
        ExecuteUpdate(wxString::Format(
            wxT("insert or replace into workspace_meta(key, value) values('schema_version', '%s');"), kSchemaVersion));
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "FindSymbolDatabase::CreateSchema failed: " << e.GetMessage();
    }
}

wxString FindSymbolDatabase::GetSchemaVersion() const
{
    try {
        wxSQLite3ResultSet rs = const_cast<FindSymbolDatabase*>(this)->ExecuteQuery(
            wxT("select value from workspace_meta where key='schema_version';"));
        if (rs.NextRow()) {
            return rs.GetString(0);
        }
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "FindSymbolDatabase::GetSchemaVersion failed: " << e.GetMessage();
    }

    clDEBUG1() << "FindSymbolDatabase::GetSchemaVersion returning empty schema version";

    return wxEmptyString;
}

void FindSymbolDatabase::Begin()
{
    try {
        wxSQLite3Database::Begin();
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "FindSymbolDatabase::Begin failed: " << e.GetMessage();
    }
}

void FindSymbolDatabase::Commit()
{
    try {
        wxSQLite3Database::Commit();
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "FindSymbolDatabase::Commit failed: " << e.GetMessage();
    }
}

void FindSymbolDatabase::Rollback()
{
    try {
        wxSQLite3Database::Rollback();
    } catch (const wxSQLite3Exception& e) {
        clWARNING() << "FindSymbolDatabase::Rollback failed: " << e.GetMessage();
    }
}
