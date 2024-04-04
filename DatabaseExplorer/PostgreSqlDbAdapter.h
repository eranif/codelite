//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : PostgreSqlDbAdapter.h
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

#ifndef POSTGRESQLDBADAPTER_H
#define POSTGRESQLDBADAPTER_H

#ifdef DBL_USE_POSTGRES
#include <wx/dblayer/include/PostgresDatabaseLayer.h>
#endif

#include <wx/dblayer/include/DatabaseLayer.h>
#include <wx/dynarray.h>
#include "IDbAdapter.h" // Base class: IDbAdapter
#include "IDbType.h"
#include "PostgreSqlType.h"


class PostgreSqlDbAdapter : public IDbAdapter {

public:
	PostgreSqlDbAdapter();
	PostgreSqlDbAdapter(const wxString& serverName,const int port, const wxString& defaultDb, const wxString& userName, const wxString& password);
	~PostgreSqlDbAdapter();

	virtual bool GetColumns(Table* pTab);
	virtual void GetDatabases(DbConnection* dbCon);
	virtual void GetTables(Database* db, bool includeViews);
	virtual void GetViews(Database* db);

	virtual IDbAdapter* Clone();
	virtual bool CanConnect();
	virtual bool IsConnected();
	virtual void CloseConnection();
	virtual DatabaseLayerPtr GetDatabaseLayer(const wxString& dbName);

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

	virtual IDbType* GetDbTypeByName(const wxString& typeName);
	virtual wxArrayString* GetDbTypes();


	virtual IDbType* GetDbTypeByUniversalName(IDbType::UNIVERSAL_TYPE type);
	virtual void ConvertTable(Table* pTab);
	virtual IDbType* ConvertType(IDbType* pType);



protected:
	//void SetDatabase(const wxString& db) { m_defaultDb = db; }

	IDbType* parseTypeString(const wxString& typeString);

	wxString m_serverName;
	int m_port;
	wxString m_userName;
	wxString m_password;
	wxString m_defaultDb;

	DatabaseLayerPtr m_pDbLayer;
};

#endif // POSTGRESQLDBADAPTER_H
