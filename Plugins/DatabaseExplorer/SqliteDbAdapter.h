//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SqliteDbAdapter.h
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

#ifndef SQLITEDBADAPTER_H
#define SQLITEDBADAPTER_H

#include "IDbAdapter.h"
#include "database.h"
#include "table.h"
#include "view.h"

#include <wx/wx.h>

#ifdef DBL_USE_SQLITE
#include <wx/dblayer/include/SqliteDatabaseLayer.h>
#endif

#include "SqliteType.h"

#include <wx/dblayer/include/DatabaseLayer.h>

class SQLiteDbAdapter : public IDbAdapter
{
public:
    SQLiteDbAdapter();
    SQLiteDbAdapter(const wxString& fileName);
    ~SQLiteDbAdapter() override = default;
    void CloseConnection() override;
    virtual DatabaseLayerPtr GetDatabaseLayer(const wxString& dbName);

    void GetDatabases(DbConnection* dbCon) override;
    void GetTables(Database* db, bool includeViews) override;
    bool GetColumns(Table* pTab) override;
    void GetViews(Database* db) override;

    virtual IDbType* GetDbTypeByName(const wxString& typeName);
    virtual wxArrayString* GetDbTypes();

    IDbAdapter* Clone() override;
    bool CanConnect() override;
    bool IsConnected() override;

    virtual wxString GetUseDb(const wxString& dbName);

    virtual wxString GetDefaultSelect(const wxString& dbName, const wxString& tableName);
    virtual wxString GetDefaultSelect(const wxString& cols, const wxString& dbName, const wxString& tableName);
    virtual wxString GetCreateTableSql(Table* tab, bool dropTable);
    virtual wxString GetCreateViewSql(View* view, bool dropView);
    virtual wxString GetAlterTableConstraintSql(Table* tab);
    virtual wxString GetCreateDatabaseSql(const wxString& dbName);
    virtual wxString GetDropTableSql(Table* pTab);
    virtual wxString GetDropViewSql(View* pView);
    virtual wxString GetDropDatabaseSql(Database* pDb);

    IDbType* GetDbTypeByUniversalName(IDbType::UNIVERSAL_TYPE type) override;
    void ConvertTable(Table* pTab) override;
    IDbType* ConvertType(IDbType* pType) override;

protected:
    wxString m_sFileName;
};
#endif // SQLITEDBADAPTER_H
