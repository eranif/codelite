#include "PHPLookupTable.h"
#include <wx/filename.h>
#include "file_logger.h"

//------------------------------------------------
// Scope table
//------------------------------------------------
const static wxString CREATE_SCOPE_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS SCOPE_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCOPE_TYPE INTEGER, " // The scope type: 0 for namespace, 1 for class
    "SCOPE_ID INTEGER NOT NULL DEFAULT -1, "
    "NAME TEXT, " // no scope, just the class name
    "EXTENDS TEXT DEFAULT '', "
    "IMPLEMENTS TEXT DEFAULT '', "
    "USING_TRAITS TEXT DEFAULT '', "
    "DOC_COMMENT TEXT DEFAULT '', "
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT DEFAULT '')";

const static wxString CREATE_SCOPE_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_1 ON SCOPE_TABLE(SCOPE_ID)";
const static wxString CREATE_SCOPE_TABLE_SQL_IDX2 =
    "CREATE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_2 ON SCOPE_TABLE(FILE_NAME)";
const static wxString CREATE_SCOPE_TABLE_SQL_IDX3 =
    "CREATE UNIQUE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_3 ON SCOPE_TABLE(NAME)";
const static wxString CREATE_SCOPE_TABLE_SQL_IDX4 =
    "CREATE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_4 ON SCOPE_TABLE(SCOPE_TYPE)";

//------------------------------------------------
// Function table
//------------------------------------------------
const static wxString CREATE_FUNCTION_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS FUNCTION_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCOPE_ID INTEGER NOT NULL DEFAULT -1, "
    "NAME TEXT, "         // no scope, just the function name
    "SCOPE TEXT, "        // Usually, this means the namespace\class
    "SIGNATURE TEXT, "    // Formatted signature
    "RETURN_VALUE TEXT, " // Fullname (including namespace)
    "FLAGS INTEGER DEFAULT 0, "
    "DOC_COMMENT TEXT, "
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT )";

const static wxString CREATE_FUNCTION_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_1 ON FUNCTION_TABLE(SCOPE_ID)";
const static wxString CREATE_FUNCTION_TABLE_SQL_IDX2 =
    "CREATE UNIQUE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_2 ON FUNCTION_TABLE(SCOPE, NAME)";
const static wxString CREATE_FUNCTION_TABLE_SQL_IDX3 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_3 ON FUNCTION_TABLE(FILE_NAME)";

//------------------------------------------------
// Variables table
//------------------------------------------------
const static wxString CREATE_VARIABLES_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS VARIABLES_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCOPE_ID INTEGER NOT NULL DEFAULT -1, " // for global variable or class member this will be the scope_id parent id
    "FUNCTION_ID INTEGER NOT NULL DEFAULT -1, " // for function argument
    "NAME TEXT, "                               // no scope, just the function name
    "SCOPE TEXT, "                              // Usually, this means the namespace\class
    "TYPEHINT TEXT, "                           // the Variable type hint
    "FLAGS INTEGER DEFAULT 0, "
    "DOC_COMMENT TEXT, "
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT )";

const static wxString CREATE_VARIABLES_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_1 ON VARIABLES_TABLE(SCOPE_ID)";
const static wxString CREATE_VARIABLES_TABLE_SQL_IDX2 =
    "CREATE UNIQUE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_2 ON VARIABLES_TABLE(SCOPE, NAME)";
const static wxString CREATE_VARIABLES_TABLE_SQL_IDX3 =
    "CREATE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_3 ON VARIABLES_TABLE(FILE_NAME)";
const static wxString CREATE_VARIABLES_TABLE_SQL_IDX4 =
    "CREATE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_4 ON VARIABLES_TABLE(FUNCTION_ID)";

PHPLookupTable::PHPLookupTable() {}

PHPLookupTable::~PHPLookupTable() {}

PHPEntityBase::Ptr_t PHPLookupTable::FindSymbol(const wxString& fullname)
{
    // locate the scope
    try {
        wxString name, scope;
        SplitFullname(fullname, name, scope);

    } catch(wxSQLite3Exception& e) {
        CL_WARNING("PHPLookupTable::FindSymbol: %s", e.GetMessage());
    }
    return PHPEntityBase::Ptr_t(NULL);
}

void PHPLookupTable::Open(const wxString& workspacePath)
{
    wxFileName fnDBFile(workspacePath, "phpsymbols.db");
    try {
        wxFileName::Mkdir(fnDBFile.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        m_db.Open(fnDBFile.GetFullPath());
        CreateSchema();

    } catch(wxSQLite3Exception& e) {
        CL_WARNING("PHPLookupTable::Open: %s", e.GetMessage());
    }
}

void PHPLookupTable::CreateSchema()
{
    try {
        m_db.ExecuteUpdate("pragma synchronous = off");

        // class / namespace table "scope"
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX3);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX4);

        // function table
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX3);

        // variables (function args, globals class members and consts)
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX3);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX4);

    } catch(wxSQLite3Exception& e) {
        CL_WARNING("PHPLookupTable::CreateSchema: %s", e.GetMessage());
    }
}

void PHPLookupTable::SplitFullname(const wxString& fullname, wxString& name, wxString& scope)
{
    wxString path = fullname;
    path.Replace("\\\\", "\\");
    if(path.Find("\\") == wxNOT_FOUND) {
        name = path;
        scope.clear();
    } else {
        scope = path.BeforeLast('\\');
        name = path.AfterLast('\\');
        if(!scope.StartsWith("\\")) {
            scope.Prepend("\\");
        }
    }
}

void PHPLookupTable::UpdateSourceFile(PHPSourceFile& source)
{
    try {
        m_db.Begin();
        // Delete all entries for this file

        // Store new entries
        PHPEntityBase::Ptr_t topNamespace = source.Namespace();
        if(topNamespace) {
            topNamespace->StoreRecursive(m_db);
        }
        m_db.Commit();

    } catch(wxSQLite3Exception& e) {
        CL_WARNING("PHPLookupTable::SaveSourceFile: %s", e.GetMessage());
    }
}
