#include "PHPLookupTable.h"
#include <wx/filename.h>

//------------------------------------------------
// Class table
//------------------------------------------------
const static wxString CREATE_CLASS_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS CLASS_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "PARENT_ID INTEGER NOT NULL DEFAULT -1, "
    "SCOPE TEXT, " // Usually, this means the namespace
    "NAME TEXT, "  // no scope, just the class name
    "EXTENDS TEXT, "
    "USING_TRAITS TEXT, "
    "IMPLEMENTS TEXT, "
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
    "SCOPE TEXT, "        // Usually, this means the namespace\class
    "NAME TEXT, "         // no scope, just the function name
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
    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}
