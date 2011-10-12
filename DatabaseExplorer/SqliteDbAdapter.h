#ifndef SQLITEDBADAPTER_H
#define SQLITEDBADAPTER_H

#include <wx/wx.h>
#include "IDbAdapter.h"
#include "table.h"
#include "view.h"
#include "database.h"


#ifdef DBL_USE_SQLITE
#include <wx/dblayer/include/SqliteDatabaseLayer.h>
#endif

#include <wx/dblayer/include/DatabaseLayer.h>
#include "SqliteType.h"

class SQLiteDbAdapter : public IDbAdapter {
public:
	SQLiteDbAdapter();
	SQLiteDbAdapter(const wxString& fileName) ;
	~SQLiteDbAdapter();
	virtual void CloseConnection();
	virtual DatabaseLayer* GetDatabaseLayer(const wxString& dbName) ;


	virtual void GetDatabases(DbConnection* dbCon);
	virtual void GetTables(Database* db, bool includeViews);
	virtual bool GetColumns(DBETable* pTab);
	virtual void GetViews(Database* db);



	virtual IDbType* GetDbTypeByName(const wxString& typeName);
	virtual wxArrayString* GetDbTypes();

	virtual IDbAdapter* Clone();
	virtual bool CanConnect();
	virtual bool IsConnected();

	virtual wxString GetUseDb(const wxString& dbName);

	virtual wxString GetDefaultSelect(const wxString& dbName, const wxString& tableName);
	virtual wxString GetDefaultSelect(const wxString& cols, const wxString& dbName, const wxString& tableName);
	virtual wxString GetCreateTableSql(DBETable* tab, bool dropTable);
	virtual wxString GetCreateViewSql(View* view, bool dropView);
	virtual wxString GetAlterTableConstraintSql(DBETable* tab);
	virtual wxString GetCreateDatabaseSql(const wxString& dbName);
	virtual wxString GetDropTableSql(DBETable* pTab);
	virtual wxString GetDropDatabaseSql(Database* pDb);


	virtual IDbType* GetDbTypeByUniversalName(IDbType::UNIVERSAL_TYPE type);
	virtual void ConvertTable(DBETable* pTab);
	virtual IDbType* ConvertType(IDbType* pType);


protected:
	wxString m_sFileName;
};
#endif // SQLITEDBADAPTER_H
