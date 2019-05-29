//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : PostgreSqlDbAdapter.cpp
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

#include "PostgreSqlDbAdapter.h"
#include "dbconnection.h"
#include "database.h"
#include "table.h"
#include "constraint.h"
#include "view.h"

PostgreSqlDbAdapter::PostgreSqlDbAdapter() {
	this->m_serverName = wxT("");
	this->m_userName = wxT("");
	this->m_password = wxT("");
	this->m_adapterType = atPOSTGRES;
    this->m_pDbLayer = NULL;
}
PostgreSqlDbAdapter::PostgreSqlDbAdapter(const wxString& serverName,const int port,const wxString& defaultDb, const wxString& userName, const wxString& password) {
	this->m_serverName = serverName;
	this->m_port = port;
	this->m_userName = userName;
	this->m_password = password;
	this->m_defaultDb = defaultDb;
	this->m_adapterType = atPOSTGRES;
    this->m_pDbLayer = NULL;
}

PostgreSqlDbAdapter::~PostgreSqlDbAdapter() {
}

void PostgreSqlDbAdapter::CloseConnection() {
    if( this->m_pDbLayer ) this->m_pDbLayer->Close();
}

DatabaseLayerPtr PostgreSqlDbAdapter::GetDatabaseLayer(const wxString& dbName) {
	DatabaseLayer* dbLayer = NULL;

#ifdef DBL_USE_POSTGRES
	if (!CanConnect())  return new PostgresDatabaseLayer();
	if (m_port == 0) m_port = 5432;
	if (!dbName.IsEmpty()) dbLayer = new PostgresDatabaseLayer(this->m_serverName,this->m_port, dbName, this->m_userName, this->m_password);
	else dbLayer = new PostgresDatabaseLayer(this->m_serverName,this->m_port,this->m_defaultDb, this->m_userName, this->m_password);
#endif
    this->m_pDbLayer = dbLayer;

	return dbLayer;
}

bool PostgreSqlDbAdapter::IsConnected() {
    if( this->m_pDbLayer )
        return this->m_pDbLayer->IsOpen();
    else
        return false;
}

wxString PostgreSqlDbAdapter::GetCreateTableSql(Table* tab, bool dropTable) {
	//TODO:SQL:
	wxString str = wxT("");
	if (dropTable) str = wxString::Format(wxT("DROP TABLE IF EXISTS %s CASCADE; \n"),tab->GetName().c_str());
	str.append(wxString::Format(wxT("CREATE TABLE %s (\n"),tab->GetName().c_str()));



	SerializableList::compatibility_iterator node = tab->GetFirstChildNode();
	while( node ) {
		Column* col = NULL;
		if( node->GetData()->IsKindOf( CLASSINFO(Column)) ) col = (Column*) node->GetData();
		if(col)	str.append(wxString::Format(wxT("\t%s %s"),col->GetName().c_str(), col->GetType()->ReturnSql().c_str()));

		Constraint* constr = wxDynamicCast(node->GetData(),Constraint);
		if (constr) {
			if (constr->GetType() == Constraint::primaryKey) str.append(wxString::Format(wxT("\tCONSTRAINT %s PRIMARY KEY (%s) \n"),constr->GetName().c_str(), constr->GetLocalColumn().c_str()));
		}

		node = node->GetNext();
		if (node) {
			if (wxDynamicCast(node->GetData(),Column)) str.append(wxT(",\n ")) ;
			else if ((constr = wxDynamicCast(node->GetData(),Constraint))) {
				if (constr->GetType() == Constraint::primaryKey) str.append(wxT(",\n ")) ;
			}

		}
		//else  str.append(wxT("\n ")) ;
	}

	/*	Column* col = tab->GetFristColumn();
		while (col) {
			str.append(wxString::Format(wxT("\t`%s` %s"),col->getName().c_str(), col->getPType()->ReturnSql().c_str()));
			col = wxDynamicCast(col->GetSibbling(),Column);
			if (col) str.append(wxT(",\n ")) ;
			else  str.append(wxT("\n ")) ;
		}*/

	str.append(wxT("\n);\n"));
	str.append(wxT("-- -------------------------------------------------------------\n"));
	return str;
}


IDbType* PostgreSqlDbAdapter::GetDbTypeByName(const wxString& typeName) {
	IDbType* type = NULL;
	// numeric types
	if (typeName == wxT("SMALLINT")) {
		type = new PostgreSqlType(wxT("SMALLINT"), IDbType::dbtNOT_NULL, IDbType::dbtTYPE_INT  );
	} else if (typeName == wxT("INTEGER")) {
		type = new PostgreSqlType(wxT("INTEGER"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_INT );
	} else if (typeName == wxT("BIGINT")) {
		type = new PostgreSqlType(wxT("BIGINT"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_INT );
	} else if (typeName == wxT("DECIMAL")) {
		type = new PostgreSqlType(wxT("DECIMAL"), IDbType::dbtNOT_NULL | IDbType::dbtSIZE | IDbType::dbtSIZE_TWO, IDbType::dbtTYPE_DECIMAL);
	} else if (typeName == wxT("NUMERIC")) {
		type = new PostgreSqlType(wxT("NUMERIC"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE | IDbType::dbtSIZE_TWO, IDbType::dbtTYPE_DECIMAL);
	} else if (typeName == wxT("REAL")) {
		type = new PostgreSqlType(wxT("REAL"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE | IDbType::dbtSIZE_TWO, IDbType::dbtTYPE_FLOAT);
	} else if (typeName == wxT("DOUBLE PRECISION")) {
		type = new PostgreSqlType(wxT("DOUBLE PRECISION"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE | IDbType::dbtSIZE_TWO, IDbType::dbtTYPE_FLOAT);
	} else if (typeName == wxT("SERIAL")) {
		type = new PostgreSqlType(wxT("SERIAL"),IDbType::dbtNOT_NULL , IDbType::dbtTYPE_INT);
	} else if (typeName == wxT("BIGSERIAL")) {
		type = new PostgreSqlType(wxT("BIGSERIAL"),IDbType::dbtNOT_NULL , IDbType::dbtTYPE_INT);

		// Monetary types
	} else if (typeName == wxT("CHARACTER VARYING")) {
		type = new PostgreSqlType(wxT("CHARACTER VARYING"),IDbType::dbtNOT_NULL| IDbType::dbtSIZE, IDbType::dbtTYPE_TEXT);
		// Character types
	} else if (typeName == wxT("VARCHAR")) {
		type = new PostgreSqlType(wxT("VARCHAR"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE , IDbType::dbtTYPE_TEXT);
	} else if (typeName == wxT("CHARACTER")) {
		type = new PostgreSqlType(wxT("CHARACTER"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE, IDbType::dbtTYPE_TEXT );
	} else if (typeName == wxT("CHAR")) {
		type = new PostgreSqlType(wxT("CHAR"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE, IDbType::dbtTYPE_TEXT );
	} else if (typeName == wxT("TEXT")) {
		type = new PostgreSqlType(wxT("TEXT"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_TEXT );

		// Binary types
	} else if (typeName == wxT("BYTEA")) {
		type = new PostgreSqlType(wxT("BYTEA"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER);

		// Date/Time types
	} else if (typeName == wxT("TIMESTAMP")) {
		type = new PostgreSqlType(wxT("TIMESTAMP"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE, IDbType::dbtTYPE_DATE_TIME );
	} else if (typeName == wxT("DATE")) {
		type = new PostgreSqlType(wxT("DATE"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_DATE_TIME );
	} else if (typeName == wxT("TIME")) {
		type = new PostgreSqlType(wxT("TIME"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE, IDbType::dbtTYPE_DATE_TIME );
	} else if (typeName == wxT("INTERVAL")) {
		type = new PostgreSqlType(wxT("INTERVAL"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE, IDbType::dbtTYPE_DATE_TIME );

		// Boolean types
	} else if (typeName == wxT("BOOLEAN")) {
		type = new PostgreSqlType(wxT("BOOLEAN"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_BOOLEAN );

		// Geometric types
	} else if (typeName == wxT("POINT")) {
		type = new PostgreSqlType(wxT("POINT"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("LINE")) {
		type = new PostgreSqlType(wxT("LINE"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("LSEG")) {
		type = new PostgreSqlType(wxT("LSEG"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("BOX")) {
		type = new PostgreSqlType(wxT("BOX"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("PATH")) {
		type = new PostgreSqlType(wxT("PATH"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("POLYGON")) {
		type = new PostgreSqlType(wxT("POLYGON"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("CIRCLE")) {
		type = new PostgreSqlType(wxT("CIRCLE"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );

		// Network address types
	} else if (typeName == wxT("CIDR")) {
		type = new PostgreSqlType(wxT("CIDR"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("INET")) {
		type = new PostgreSqlType(wxT("INET"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("MACADDR")) {
		type = new PostgreSqlType(wxT("MACADDR"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );

		// Bit String types
	} else if (typeName == wxT("BIT")) {
		type = new PostgreSqlType(wxT("BIT"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE, IDbType::dbtTYPE_OTHER );
	} else if (typeName == wxT("BIT VARYING")) {
		type = new PostgreSqlType(wxT("BIT VARYING"),IDbType::dbtNOT_NULL | IDbType::dbtSIZE, IDbType::dbtTYPE_OTHER );

		// UUID type
	} else if (typeName == wxT("UUID")) {
		type = new PostgreSqlType(wxT("UUID"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );

		// XML type
	} else if (typeName == wxT("XML")) {
		type = new PostgreSqlType(wxT("XML"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );

		// Other types
	} else if (typeName == wxT("OID")) {
		type = new PostgreSqlType(wxT("OID"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER );
	} else if (typeName == wxT("XID")) {
		type = new PostgreSqlType(wxT("XID"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	} else if (typeName == wxT("ARRAY")) {
		type = new PostgreSqlType(wxT("ARRAY"),IDbType::dbtNOT_NULL , IDbType::dbtTYPE_OTHER );
	} else if (typeName == wxT("REGPROX")) {
		type = new PostgreSqlType(wxT("REGPROX"),IDbType::dbtNOT_NULL, IDbType::dbtTYPE_OTHER  );
	}

	wxASSERT(type);
	return type;
}

wxArrayString* PostgreSqlDbAdapter::GetDbTypes() {
	wxArrayString* pNames = new wxArrayString();

	pNames->Add(wxT("SMALLINT"));
	pNames->Add(wxT("INTEGER")) ;
	pNames->Add(wxT("BIGINT")) ;
	pNames->Add(wxT("DECIMAL"));
	pNames->Add(wxT("NUMERIC"));
	pNames->Add(wxT("REAL")) ;
	pNames->Add(wxT("DOUBLE PRECISION")) ;
	pNames->Add(wxT("SERIAL")) ;
	pNames->Add(wxT("BIGSERIAL")) ;

	// Monetary types
	pNames->Add(wxT("CHARACTER VARYING")) ;
	// Character types
	pNames->Add(wxT("VARCHAR")) ;
	pNames->Add(wxT("CHARACTER")) ;
	pNames->Add(wxT("CHAR"));
	pNames->Add(wxT("TEXT")) ;

	// Binary types
	pNames->Add(wxT("BYTEA")) ;

	// Date/Time types
	pNames->Add(wxT("TIMESTAMP")) ;
	pNames->Add(wxT("DATE"));
	pNames->Add(wxT("TIME")) ;
	pNames->Add(wxT("INTERVAL"));

	// Boolean types
	pNames->Add(wxT("BOOLEAN"));

	// Geometric types
	pNames->Add(wxT("POINT")) ;
	pNames->Add(wxT("LINE")) ;
	pNames->Add(wxT("LSEG")) ;
	pNames->Add(wxT("BOX")) ;
	pNames->Add(wxT("PATH")) ;
	pNames->Add(wxT("POLYGON"));
	pNames->Add(wxT("CIRCLE")) ;

	// Network address types
	pNames->Add(wxT("CIDR"));
	pNames->Add(wxT("INET"));
	pNames->Add(wxT("MACADDR")) ;

	// Bit String types
	pNames->Add(wxT("BIT"));
	pNames->Add(wxT("BIT VARYING")) ;

	// UUID type
	pNames->Add(wxT("UUID")) ;

	// XML type
	pNames->Add(wxT("XML")) ;

	// OTHER TYPES
	pNames->Add(wxT("OID")) ;
	pNames->Add(wxT("XID")) ;
	pNames->Add(wxT("ARRAY")) ;
	pNames->Add(wxT("REGPROX")) ;


	return pNames;
}
wxString PostgreSqlDbAdapter::GetDefaultSelect(const wxString& dbName, const wxString& tableName) {
	//TODO:SQL:
	wxString ret = wxString::Format(wxT("SELECT * FROM %s"), tableName.c_str());
	return ret;
}
wxString PostgreSqlDbAdapter::GetDefaultSelect(const wxString& cols, const wxString& dbName, const wxString& tableName) {
	//TODO:SQL:
	wxString ret = wxString::Format(wxT("SELECT %s FROM %s"),cols.c_str(), tableName.c_str());
	return ret;
}

bool PostgreSqlDbAdapter::GetColumns(Table* pTab) {
	if (pTab) {
//		SetDatabase(pTab->GetParentName());
		DatabaseLayerPtr dbLayer = this->GetDatabaseLayer(pTab->GetParentName());

		if (!dbLayer->IsOpen()) return NULL;
		// loading columns
		//TODO:SQL:
		//DatabaseResultSet *database = dbLayer->RunQueryWithResults(wxString::Format(wxT("SHOW COLUMNS IN `%s`.`%s`"),pTab->getParentName().c_str(),pTab->getName().c_str()));
		DatabaseResultSet *database = dbLayer->RunQueryWithResults(wxString::Format(wxT("SELECT * FROM information_schema.columns WHERE table_name = '%s'"),pTab->GetName().c_str()));


		while (database->Next()) {
			IDbType* pType = parseTypeString(database->GetResultString(wxT("data_type")));
			if (pType) {
				pType->SetSize(database->GetResultInt(wxT("numeric_precision")));
				pType->SetSize2(database->GetResultInt(wxT("numeric_precision_radix")));
				pType->SetNotNull(database->GetResultString(wxT("is_nullable")) == wxT("NO"));
				Column* pCol = new Column(database->GetResultString(wxT("column_name")),pTab->GetName(), pType);
				pTab->AddChild(pCol);
			}
		}
		dbLayer->CloseResultSet(database);



//wxT("SELECT tc.constraint_name, tc.constraint_type, tc.table_name, kcu.column_name, tc.is_deferrable, tc.initially_deferred, rc.match_option AS match_type, rc.update_rule AS on_update, rc.delete_rule AS on_delete, ccu.table_name AS references_table, ccu.column_name AS references_field FROM information_schema.table_constraints tc LEFT JOIN information_schema.key_column_usage kcu ON tc.constraint_catalog = kcu.constraint_catalog AND tc.constraint_schema = kcu.constraint_schema AND tc.constraint_name = kcu.constraint_name LEFT JOIN information_schema.referential_constraints rc ON tc.constraint_catalog = rc.constraint_catalog AND tc.constraint_schema = rc.constraint_schema AND tc.constraint_name = rc.constraint_name LEFT JOIN information_schema.constraint_column_usage ccu ON rc.unique_constraint_catalog = ccu.constraint_catalog AND rc.unique_constraint_schema = ccu.constraint_schema AND rc.unique_constraint_name = ccu.constraint_name WHERE tc.table_name = '%s'");



		//TODO:SQL:
		wxString constrSql = wxT("SELECT tc.constraint_name, tc.constraint_type, tc.table_name, kcu.column_name, tc.is_deferrable, tc.initially_deferred, rc.match_option AS match_type, rc.update_rule AS on_update, rc.delete_rule AS on_delete, ccu.table_name AS references_table, ccu.column_name AS references_field FROM information_schema.table_constraints tc LEFT JOIN information_schema.key_column_usage kcu ON tc.constraint_catalog = kcu.constraint_catalog AND tc.constraint_schema = kcu.constraint_schema AND tc.constraint_name = kcu.constraint_name LEFT JOIN information_schema.referential_constraints rc ON tc.constraint_catalog = rc.constraint_catalog AND tc.constraint_schema = rc.constraint_schema AND tc.constraint_name = rc.constraint_name LEFT JOIN information_schema.constraint_column_usage ccu ON rc.unique_constraint_catalog = ccu.constraint_catalog AND rc.unique_constraint_schema = ccu.constraint_schema AND rc.unique_constraint_name = ccu.constraint_name WHERE tc.table_name = '%s'");

		database = dbLayer->RunQueryWithResults(wxString::Format(constrSql, pTab->GetName().c_str()));
		while (database->Next()) {
			if ((database->GetResultString(wxT("constraint_type")) == wxT("PRIMARY KEY"))||(database->GetResultString(wxT("constraint_type")) == wxT("FOREIGN KEY"))) {
				Constraint* constr = new Constraint();
				constr->SetName(database->GetResultString(wxT("constraint_name")));
				constr->SetLocalColumn(database->GetResultString(wxT("column_name")));
				constr->SetType(Constraint::primaryKey);
				if (database->GetResultString(wxT("references_table")) != wxT("") ) {
					constr->SetType(Constraint::foreignKey);
					constr->SetRefTable(database->GetResultString(wxT("references_table")));
					constr->SetRefCol(database->GetResultString(wxT("references_field")));

					wxString onDelete = database->GetResultString(wxT("on_update"));
					if (onDelete == wxT("RESTRICT")) constr->SetOnUpdate(Constraint::restrict);
					if (onDelete == wxT("CASCADE")) constr->SetOnUpdate(Constraint::cascade);
					if (onDelete == wxT("SET NULL")) constr->SetOnUpdate(Constraint::setNull);
					if (onDelete == wxT("NO ACTION")) constr->SetOnUpdate(Constraint::noAction);

					wxString onUpdate = database->GetResultString(wxT("on_delete"));
					if (onUpdate == wxT("RESTRICT")) constr->SetOnDelete(Constraint::restrict);
					if (onUpdate == wxT("CASCADE")) constr->SetOnDelete(Constraint::cascade);
					if (onUpdate == wxT("SET NULL")) constr->SetOnDelete(Constraint::setNull);
					if (onUpdate == wxT("NO ACTION")) constr->SetOnDelete(Constraint::noAction);


				}
				pTab->AddChild(constr);
			}
		}
		dbLayer->CloseResultSet(database);
		dbLayer->Close();
	}
	return true;
}
IDbType* PostgreSqlDbAdapter::parseTypeString(const wxString& typeString) {
	wxString text = typeString.Upper().Trim();
	IDbType* type = this->GetDbTypeByName(text);

	return type;
	//return new MySqlType(wxT("DECIMAL"),IDbType::dbtUNIQUE | IDbType::dbtNOT_NULL | IDbType::dbtSIZE | IDbType::dbtSIZE_TWO);
}

bool PostgreSqlDbAdapter::CanConnect() {
	return ((m_serverName != wxT(""))&&(m_userName != wxT("")));
}
void PostgreSqlDbAdapter::GetDatabases(DbConnection* dbCon) {
	if (dbCon) {
		DatabaseLayerPtr dbLayer = this->GetDatabaseLayer(wxT(""));
		if (dbLayer) {
			if (!dbLayer->IsOpen()) return;

			// loading databases
			//TODO:SQL:
			DatabaseResultSet *databaze = dbLayer->RunQueryWithResults(wxT("SELECT datname FROM pg_database WHERE datallowconn = 't' "));
			while (databaze->Next()) {
				dbCon->AddChild( new Database(this, databaze->GetResultString(1)) );
			}
			dbLayer->CloseResultSet(databaze);
			dbLayer->Close();
		}
	}
	return;
}

void PostgreSqlDbAdapter::GetTables(Database* db, bool includeViews) {
	if (db) {
		//SetDatabase(db->GetName());
		DatabaseLayerPtr dbLayer = this->GetDatabaseLayer(db->GetName());
		if (dbLayer) {
			if (!dbLayer->IsOpen()) return;
			// lading tables for database
			//TODO:SQL:

			//DatabaseResultSet *tabulky = dbLayer->RunQueryWithResults(wxString::Format(wxT("SHOW TABLES IN `%s`"), db->getName().c_str()) );
			//DatabaseResultSet *tabulky = dbLayer->RunQueryWithResults(wxString::Format(wxT("SELECT * FROM `INFORMATION_SCHEMA`.`TABLES` WHERE `TABLE_SCHEMA` = '%s' AND `TABLE_TYPE` = 'BASE TABLE'"), db->getName().c_str()) );
			DatabaseResultSet *tabulky = NULL;
			if (includeViews) {
				tabulky = dbLayer->RunQueryWithResults(wxString::Format(wxT("SELECT * FROM information_schema.tables WHERE table_schema = 'public' AND (table_type = 'BASE TABLE' OR table_type = 'VIEW')")) );
			} else {
				tabulky = dbLayer->RunQueryWithResults(wxString::Format(wxT("SELECT * FROM information_schema.tables WHERE table_schema = 'public' AND table_type = 'BASE TABLE'")) );
			}
			while (tabulky->Next()) {
				db->AddChild(new Table(this,  tabulky->GetResultString(wxT("TABLE_NAME")), db->GetName(),    tabulky->GetResultString(wxT("TABLE_TYPE")).Contains(wxT("VIEW"))    ));
			}
			dbLayer->CloseResultSet(tabulky);
			dbLayer->Close();
		}
	}
	return;
}
wxString PostgreSqlDbAdapter::GetCreateDatabaseSql(const wxString& dbName) {
	return wxString::Format(wxT("CREATE DATABASE %s"), dbName.c_str());
}
wxString PostgreSqlDbAdapter::GetDropTableSql(Table* pTab) {
	return wxString::Format(wxT("DROP TABLE IF EXISTS %s;"), pTab->GetName().c_str());
}
wxString PostgreSqlDbAdapter::GetAlterTableConstraintSql(Table* tab) {
	//TODO:SQL:
	wxString str =  wxString::Format(wxT("-- ---------- CONSTRAINTS FOR TABLE %s \n"),tab->GetName().c_str());
	str.append(wxT("-- -------------------------------------------------------------\n"));
	wxString prefix = wxString::Format(wxT("ALTER TABLE %s "),tab->GetName().c_str());

	SerializableList::compatibility_iterator node = tab->GetFirstChildNode();
	while( node ) {
		Constraint* constr = NULL;
		constr = wxDynamicCast(node->GetData(), Constraint);
		if (constr) {
			if (constr->GetType() == Constraint::foreignKey) {
				str.append(prefix + wxString::Format(wxT("ADD CONSTRAINT %s FOREIGN KEY (%s) REFERENCES %s(%s) " ), constr->GetName().c_str(), constr->GetLocalColumn().c_str(), constr->GetRefTable().c_str(), constr->GetRefCol().c_str()));
				str.append(wxT("ON UPDATE "));
				switch(constr->GetOnUpdate()) {
				case Constraint::restrict:
					str.append(wxT("RESTRICT "));
					break;
				case Constraint::cascade:
					str.append(wxT("CASCADE "));
					break;
				case Constraint::setNull:
					str.append(wxT("SET NULL "));
					break;
				case Constraint::noAction:
					str.append(wxT("NO ACTION "));
					break;
				}
				str.append(wxT("ON DELETE "));
				switch(constr->GetOnDelete()) {
				case Constraint::restrict:
					str.append(wxT("RESTRICT "));
					break;
				case Constraint::cascade:
					str.append(wxT("CASCADE "));
					break;
				case Constraint::setNull:
					str.append(wxT("SET NULL "));
					break;
				case Constraint::noAction:
					str.append(wxT("NO ACTION "));
					break;
				}
				str.append(wxT("; \n"));
			}
		}//if (constr->GetType() == Constraint::primaryKey) str.append(prefix + wxString::Format(wxT("ADD CONSTRAINT `%s` PRIMARY KEY (`%s`); \n"), constr->GetName().c_str(), constr->GetLocalColumn().c_str()));


		node = node->GetNext();
	}
	str.append(wxT("-- -------------------------------------------------------------\n"));
	return str;
}
wxString PostgreSqlDbAdapter::GetDropDatabaseSql(Database* pDb) {
	return wxString::Format(wxT("DROP DATABASE %s"),pDb->GetName().c_str());
}
wxString PostgreSqlDbAdapter::GetUseDb(const wxString& dbName) {
	return wxT("");
}
void PostgreSqlDbAdapter::GetViews(Database* db) {
	if (db) {
		//SetDatabase(db->GetName());
		DatabaseLayerPtr dbLayer = this->GetDatabaseLayer(db->GetName());
		if (dbLayer) {
			if (!dbLayer->IsOpen()) return;
			// lading tables for database
			//TODO:SQL:

			//DatabaseResultSet *tabulky = dbLayer->RunQueryWithResults(wxString::Format(wxT("SHOW TABLES IN `%s`"), db->getName().c_str()) );
			//DatabaseResultSet *tabulky = dbLayer->RunQueryWithResults(wxString::Format(wxT("SELECT * FROM `INFORMATION_SCHEMA`.`TABLES` WHERE `TABLE_SCHEMA` = '%s' AND `TABLE_TYPE` = 'BASE TABLE'"), db->getName().c_str()) );
			DatabaseResultSet *tabulky = dbLayer->RunQueryWithResults(wxString::Format(wxT("SELECT * FROM pg_views WHERE schemaname = 'public'")) );

			while (tabulky->Next()) {
				db->AddChild(new View(this,  tabulky->GetResultString(wxT("viewname")),db->GetName(), tabulky->GetResultString(wxT("definition"))));
			}
			dbLayer->CloseResultSet(tabulky);
			dbLayer->Close();
		}
	}
	return;
}
wxString PostgreSqlDbAdapter::GetCreateViewSql(View* view, bool dropView) {
	wxString str = wxT("");
	if (view) {
		if (dropView) {
			str.append(wxString::Format(wxT("DROP VIEW IF EXISTS %s;\n"),view->GetName().c_str()));
		}
		str.append(wxString::Format(wxT("CREATE VIEW %s AS %s ;\n"),view->GetName().c_str(), view->GetSelect().c_str()));
	}
	str.append(wxT("-- -------------------------------------------------------------\n"));
	return str;
}
void PostgreSqlDbAdapter::ConvertTable(Table* pTab) {
	SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
	while( node ) {
		if( node->GetData()->IsKindOf( CLASSINFO(Column)) )  {
			Column* col = (Column*) node->GetData();
			col->SetType(ConvertType(col->GetType()));
		}
		node = node->GetNext();
	}
}

IDbType* PostgreSqlDbAdapter::ConvertType(IDbType* pType) {
	IDbType* newType = wxDynamicCast(pType, PostgreSqlType);
	if (!newType) {
		newType = GetDbTypeByUniversalName(pType->GetUniversalType());
		delete pType;
	}
	return newType;
}

IDbType* PostgreSqlDbAdapter::GetDbTypeByUniversalName(IDbType::UNIVERSAL_TYPE type) {
	IDbType* newType = NULL;
	switch (type) {
	case IDbType::dbtTYPE_INT:
		newType = GetDbTypeByName(wxT("INTEGER"));
		break;
	case IDbType::dbtTYPE_BOOLEAN:
		newType = GetDbTypeByName(wxT("BOOLEAN"));
		break;
	case IDbType::dbtTYPE_DATE_TIME:
		newType = GetDbTypeByName(wxT("DATE"));
		break;
	case IDbType::dbtTYPE_DECIMAL:
		newType = GetDbTypeByName(wxT("DECIMAL"));
		break;
	case IDbType::dbtTYPE_FLOAT:
		newType = GetDbTypeByName(wxT("DOUBLE PRECISION"));
		break;
	case IDbType::dbtTYPE_TEXT:
		newType = GetDbTypeByName(wxT("TEXT"));
		break;
	case IDbType::dbtTYPE_OTHER:
		newType = GetDbTypeByName(wxT("BYTEA"));
		break;
	}
	return newType;
}
IDbAdapter* PostgreSqlDbAdapter::Clone() {
	return new PostgreSqlDbAdapter(m_serverName,m_port, m_defaultDb, m_userName, m_password);
}

wxString PostgreSqlDbAdapter::GetDropViewSql(View* pView)
{
	return wxString::Format(wxT("DROP VIEW IF EXISTS %s;"), pView->GetName().c_str());
}
