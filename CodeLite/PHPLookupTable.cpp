#include "PHPLookupTable.h"
#include <wx/filename.h>

//------------------------------------------------
// Namespace table
//------------------------------------------------
const static wxString CREATE_NAMESPACE_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS NAMESPACE_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "NAME TEXT"  // full name including the starting slash
    ")";

const static wxString CREATE_NAMESPACE_TABLE_SQL_IDX1 =
    "CREATE UNIQUE INDEX IF NOT EXISTS NAMESPACE_TABLE_IDX_1 ON NAMESPACE_TABLE(NAME)";

//------------------------------------------------
// Class table
//------------------------------------------------
const static wxString CREATE_CLASS_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS CLASS_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "PARENT_ID INTEGER NOT NULL DEFAULT -1, "
    "NAME TEXT, "  // no scope, just the class name
    "SCOPE TEXT, " // Usually, this means the namespace
    "EXTENDS TEXT, "
    "IMPLEMENTS TEXT, "
    "USING_TRAITS TEXT, "
    "DOC_COMMENT TEXT, "
    "LINE_NUMBER INTEGER  NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT )";

const static wxString CREATE_CLASS_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS CLASS_TABLE_IDX_1 ON CLASS_TABLE(PARENT_ID)";
const static wxString CREATE_CLASS_TABLE_SQL_IDX2 =
    "CREATE UNIQUE INDEX IF NOT EXISTS CLASS_TABLE_IDX_2 ON CLASS_TABLE(SCOPE, NAME)";
const static wxString CREATE_CLASS_TABLE_SQL_IDX3 =
    "CREATE INDEX IF NOT EXISTS CLASS_TABLE_IDX_3 ON CLASS_TABLE(FILE_NAME)";

//------------------------------------------------
// Function table
//------------------------------------------------
const static wxString CREATE_FUNCTION_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS FUNCTION_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "PARENT_ID INTEGER NOT NULL DEFAULT -1, "
    "NAME TEXT, "         // no scope, just the function name
    "SCOPE TEXT, "        // Usually, this means the namespace\class
    "SIGNATURE TEXT, "    // Formatted signature
    "RETURN_VALUE TEXT, " // Fullname (including namespace)
    "FLAGS INTEGER DEFAULT 0, "
    "DOC_COMMENT TEXT, "
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT )";

const static wxString CREATE_FUNCTION_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_1 ON FUNCTION_TABLE(PARENT_ID)";
const static wxString CREATE_FUNCTION_TABLE_SQL_IDX2 =
    "CREATE UNIQUE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_2 ON FUNCTION_TABLE(SCOPE, NAME)";
const static wxString CREATE_FUNCTION_TABLE_SQL_IDX3 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_3 ON FUNCTION_TABLE(FILE_NAME)";

//------------------------------------------------
// Variables table
//------------------------------------------------
const static wxString CREATE_VARIABLES_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS VARIABLES_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "PARENT_ID INTEGER NOT NULL DEFAULT -1, "
    "NAME TEXT, "     // no scope, just the function name
    "SCOPE TEXT, "    // Usually, this means the namespace\class
    "TYPEHINT TEXT, " // the Variable type hint
    "FLAGS INTEGER DEFAULT 0, "
    "DOC_COMMENT TEXT, "
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT )";

const static wxString CREATE_VARIABLES_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_1 ON VARIABLES_TABLE(PARENT_ID)";
const static wxString CREATE_VARIABLES_TABLE_SQL_IDX2 =
    "CREATE UNIQUE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_2 ON VARIABLES_TABLE(SCOPE, NAME)";
const static wxString CREATE_VARIABLES_TABLE_SQL_IDX3 =
    "CREATE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_3 ON VARIABLES_TABLE(FILE_NAME)";

PHPLookupTable::PHPLookupTable() {}

PHPLookupTable::~PHPLookupTable() {}

PHPEntityBase::Ptr_t PHPLookupTable::FindSymbol(const wxString& name, const wxString& path)
{
    return PHPEntityBase::Ptr_t(NULL);
}

void PHPLookupTable::Open(const wxString& workspacePath)
{
    wxFileName fnDBFile(workspacePath, "phpsymbols.db");
    try {

        m_db.Open(fnDBFile.GetFullPath());
        CreateSchema();

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void PHPLookupTable::CreateSchema()
{
    try {
        m_db.ExecuteUpdate("pragma synchronous = off");
        
        m_db.ExecuteUpdate(CREATE_NAMESPACE_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_NAMESPACE_TABLE_SQL_IDX1);
        
        m_db.ExecuteUpdate(CREATE_CLASS_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_CLASS_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_CLASS_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_CLASS_TABLE_SQL_IDX3);

        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX3);

        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX3);

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}
