//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : IDbAdapter.h
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

#ifndef IDBADAPTER_H
#define IDBADAPTER_H
#include <wx/wx.h>
#include "smart_ptr.h"
#include <wx/dynarray.h>
#include <wx/dblayer/include/DatabaseLayer.h>
#include "IDbType.h"
//#include "columncol.h"
//#include "tablecol.h"
//#include "databasecol.h"
class DbConnection;
class Database;
class Table;
class View;
class IDbType;


typedef SmartPtr<DatabaseLayer> DatabaseLayerPtr;

/*! \brief Basic virtual class for creating universal interface between different database servers. */
class IDbAdapter
{

public:
    enum TYPE {
        atUNKNOWN = 0,
        atSQLITE,
        atMYSQL,
        atPOSTGRES
    };
    
    IDbAdapter()
        : m_adapterType(atUNKNOWN)
    {
    }

    virtual ~IDbAdapter() {}
    
    /*! \brief Return opened DatabaseLayer for selected database. If dbName is empty, DatabaseLayer will be opend without defalut database. */
    virtual DatabaseLayerPtr GetDatabaseLayer(const wxString& dbName) = 0;

    /*! \brief Return true if dbAdapter is connected. DEPRECATED!!! */
    virtual bool IsConnected() = 0;

    /*! \brief Clone DbAdapter and return new object */
    virtual IDbAdapter* Clone() = 0;

    /*! \brief Return true if adapter settings are set and adapter can connect to the database */
    virtual bool CanConnect() = 0;

    /*! \brief Function connect databases to the DbConnection */
    virtual void GetDatabases(DbConnection* dbCon) = 0;

    /*! \brief Function connect tables to the Database */
    virtual void GetTables(Database* db, bool includeViews) = 0;

    /*! \brief Function connect views to the Database */
    virtual void GetViews(Database* db) = 0;

    /*! \brief Function connect columns to the DBETable */
    virtual bool GetColumns(Table* pTab) = 0;


    /*! \brief Return wxString with USE DB statement for dbName */
    virtual wxString GetUseDb(const wxString& dbName) = 0;
    /*! \brief Return wxString with defalut SELECT for defined table and db */
    virtual wxString GetDefaultSelect(const wxString& dbName, const wxString& tableName) = 0;
    virtual wxString GetDefaultSelect(const wxString& cols, const wxString& dbName, const wxString& tableName) = 0;
    /*! \brief Return Create table sql statement */
    virtual wxString GetCreateTableSql(Table* tab, bool dropTable) = 0;
    /*! \brief Return Create view sql statement */
    virtual wxString GetCreateViewSql(View* view, bool dropView) = 0;
    /*! \brief Return Create update table SQL statement for adding constraints */
    virtual wxString GetAlterTableConstraintSql(Table* tab) = 0;
    /*! \brief Return Create database SQL statement. */
    virtual wxString GetCreateDatabaseSql(const wxString& dbName) = 0;
    /*! \brief Return Drop table SQL statement. */
    virtual wxString GetDropTableSql(Table* pTab) = 0;
    /*! \brief Return Drop view SQL statement. */
    virtual wxString GetDropViewSql(View* pView) = 0;
    /*! \brief Return Drop database SQL statement. */
    virtual wxString GetDropDatabaseSql(Database* pDb) = 0;

    virtual void CloseConnection() = 0;

    /*! \brief Return IDbType by name string */
    virtual IDbType* GetDbTypeByName(const wxString& typeName) = 0;
    /*! \brief Return IDbType by name universal name */
    virtual IDbType* GetDbTypeByUniversalName(IDbType::UNIVERSAL_TYPE type) = 0;
    /*! \brief Return wxArrayString of possible db types */
    virtual wxArrayString* GetDbTypes () = 0;

    /*! \brief Convert IDbType to the special db type. (!!! Old type is deleted !!!) */
    virtual IDbType* ConvertType (IDbType* pType) = 0;
    /*! \brief Return wxArrayString of possible db types */
    virtual void ConvertTable(Table* pTab) = 0;

    const TYPE& GetAdapterType() const {
        return m_adapterType;
    }

protected:
    TYPE m_adapterType;

};

#endif // IDBADAPTER_H
