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

	virtual bool GetColumns(DBETable* pTab);
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
	virtual wxString GetCreateTableSql(DBETable* tab, bool dropTable);
	virtual wxString GetCreateViewSql(View* view, bool dropView);
	virtual wxString GetAlterTableConstraintSql(DBETable* tab);
	virtual wxString GetCreateDatabaseSql(const wxString& dbName);
	virtual wxString GetDropTableSql(DBETable* pTab);
	virtual wxString GetDropDatabaseSql(Database* pDb);

	virtual IDbType* GetDbTypeByName(const wxString& typeName);
	virtual wxArrayString* GetDbTypes();


	virtual IDbType* GetDbTypeByUniversalName(IDbType::UNIVERSAL_TYPE type);
	virtual void ConvertTable(DBETable* pTab);
	virtual IDbType* ConvertType(IDbType* pType);



protected:
	//void SetDatabase(const wxString& db) { m_defaultDb = db; }

	IDbType* parseTypeString(const wxString& typeString);

	wxString m_serverName;
	int m_port;
	wxString m_userName;
	wxString m_password;
	wxString m_defaultDb;

	DatabaseLayer* m_pDbLayer;
};

#endif // POSTGRESQLDBADAPTER_H
