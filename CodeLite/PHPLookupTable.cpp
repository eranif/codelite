#include "PHPLookupTable.h"
#include <wx/filename.h>
#include "file_logger.h"
#include "PHPEntityNamespace.h"
#include "PHPEntityClass.h"
#include "PHPEntityVariable.h"
#include "PHPEntityFunction.h"

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

PHPEntityBase::Ptr_t PHPLookupTable::FindMemberOf(wxLongLong parentDbId, const wxString& exactName)
{
    // find the entity
    PHPEntityBase::Ptr_t scope = DoFindScope(parentDbId);
    if(scope->Cast<PHPEntityClass>()) {
        std::vector<wxLongLong> parents;
        std::set<wxLongLong> parentsVisited;

        DoGetInheritanceParentIDs(scope, parents, parentsVisited);
        
        // Parents should now contain an ordered list of all the inheritance
        for(size_t i=0; i<parents.size(); ++i) {
            PHPEntityBase::Ptr_t match = DoFindMemberOf(parents.at(i), exactName);
            if(match) {
                return match;
            }
        }
    } else {
        // namespace
        return DoFindMemberOf(parentDbId, exactName);
    }
    return PHPEntityBase::Ptr_t(NULL);
}

PHPEntityBase::Ptr_t PHPLookupTable::FindScope(const wxString& fullname) { return DoFindScope(fullname); }

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

void PHPLookupTable::SplitFullname(const wxString& fullname, wxString& name, wxString& scope) const
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

PHPEntityBase::Ptr_t PHPLookupTable::DoFindMemberOf(wxLongLong parentDbId, const wxString& exactName)
{
    // Find members of of parentDbID
    try {
        PHPEntityBase::List_t matches;
        {
            wxString sql;
            sql << "SELECT * from FUNCTION_TABLE WHERE SCOPE_ID=" << parentDbId << " AND NAME='" << exactName << "'";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityFunction());
                match->FromResultSet(res);
                matches.push_back(match);
            }
        }
        if(matches.empty()) {
            // Could not find a match in the function table, check the variable table
            wxString sql;
            wxString nameWDollar, namwWODollar;
            nameWDollar = exactName;
            if(exactName.StartsWith("$")) {
                namwWODollar = exactName.Mid(1);
            } else {
                namwWODollar = exactName;
                nameWDollar.Prepend("$");
            }

            sql << "SELECT * from VARIABLES_TABLE WHERE SCOPE_ID=" << parentDbId << " AND NAME IN ('" << nameWDollar
                << "', '" << namwWODollar << "')";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityVariable());
                match->FromResultSet(res);
                matches.push_back(match);
            }
            if(matches.empty() || matches.size() > 1) {
                return PHPEntityBase::Ptr_t(NULL);
            } else {
                return (*matches.begin());
            }
        } else if(matches.size() > 1) {
            // we found more than 1 match in the function table
            // return NULL
            return PHPEntityBase::Ptr_t(NULL);
        } else {
            // exactly one match was found in the function table
            // return it
            return (*matches.begin());
        }

    } catch(wxSQLite3Exception& e) {
        CL_WARNING("PHPLookupTable::DoFindMemberOf: %s", e.GetMessage());
    }
    return PHPEntityBase::Ptr_t(NULL);
}

void PHPLookupTable::DoGetInheritanceParentIDs(PHPEntityBase::Ptr_t cls,
                                               std::vector<wxLongLong>& parents,
                                               std::set<wxLongLong>& parentsVisited)
{
    parents.push_back(cls->GetDbId());
    parentsVisited.insert(cls->GetDbId());
    wxArrayString parentsArr = cls->Cast<PHPEntityClass>()->GetInheritanceArray();
    for(size_t i=0; i<parentsArr.GetCount(); ++i) {
        PHPEntityBase::Ptr_t parent = FindClass(parentsArr.Item(i));
        if(parent && !parentsVisited.count(parent->GetDbId())) {
            DoGetInheritanceParentIDs(parent, parents, parentsVisited);
        }
    }
}

PHPEntityBase::Ptr_t PHPLookupTable::DoFindScope(const wxString& fullname, ePhpScopeType scopeType)
{
    // locate the scope
    try {
        wxString sql;

        // limit by 2 for performance reason
        // we will return NULL incase the number of matches is greater than 1...
        sql << "SELECT * from SCOPE_TABLE WHERE NAME='" << fullname << "'";
        if(scopeType != kPhpScopeTypeAny) {
            sql << " AND SCOPE_TYPE = " << static_cast<int>(scopeType);
        }
        sql << " LIMIT 2 ";

        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        PHPEntityBase::Ptr_t match(NULL);

        while(res.NextRow()) {
            if(match) {
                // only one match
                return PHPEntityBase::Ptr_t(NULL);
            }

            int scopeType = res.GetInt("SCOPE_TYPE", 1);
            if(scopeType == 0) {
                // namespace
                match.reset(new PHPEntityNamespace());
            } else {
                // class
                match.reset(new PHPEntityClass());
            }
            match->FromResultSet(res);
        }
        return match;

    } catch(wxSQLite3Exception& e) {
        CL_WARNING("PHPLookupTable::FindScope: %s", e.GetMessage());
    }
    return PHPEntityBase::Ptr_t(NULL);
}

PHPEntityBase::Ptr_t PHPLookupTable::FindClass(const wxString& fullname)
{
    return DoFindScope(fullname, kPhpScopeTypeClass);
}

PHPEntityBase::Ptr_t PHPLookupTable::DoFindScope(wxLongLong id, ePhpScopeType scopeType)
{
    // locate the scope
    try {
        wxString sql;

        // limit by 2 for performance reason
        // we will return NULL incase the number of matches is greater than 1...
        sql << "SELECT * from SCOPE_TABLE WHERE ID=" << id;
        if(scopeType != kPhpScopeTypeAny) {
            sql << " AND SCOPE_TYPE = " << static_cast<int>(scopeType);
        }
        sql << " LIMIT 1";

        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();

        if(res.NextRow()) {
            PHPEntityBase::Ptr_t match(NULL);
            int scopeType = res.GetInt("SCOPE_TYPE", 1);
            if(scopeType == kPhpScopeTypeNamespace) {
                // namespace
                match.reset(new PHPEntityNamespace());
            } else {
                // class
                match.reset(new PHPEntityClass());
            }
            match->FromResultSet(res);
            return match;
        }
    } catch(wxSQLite3Exception& e) {
        CL_WARNING("PHPLookupTable::DoFindScope: %s", e.GetMessage());
    }
    return PHPEntityBase::Ptr_t(NULL);
}

PHPEntityBase::Ptr_t PHPLookupTable::FindClass(wxLongLong id) { return DoFindScope(id, kPhpScopeTypeClass); }
