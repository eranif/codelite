#include "PHPLookupTable.h"

#include "PHPEntityClass.h"
#include "PHPEntityFunction.h"
#include "PHPEntityFunctionAlias.h"
#include "PHPEntityNamespace.h"
#include "PHPEntityVariable.h"
#include "clFilesCollector.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"

#include <algorithm>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stopwatch.h>
#include <wx/tokenzr.h>

wxDEFINE_EVENT(wxPHP_PARSE_STARTED, clParseEvent);
wxDEFINE_EVENT(wxPHP_PARSE_ENDED, clParseEvent);
wxDEFINE_EVENT(wxPHP_PARSE_PROGRESS, clParseEvent);

static wxString PHP_SCHEMA_VERSION = "9.3.0.1";

//------------------------------------------------
// Metadata table
//------------------------------------------------
const static wxString CREATE_METADATA_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS METADATA_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCHEMA_NAME TEXT, "
    "SCHEMA_VERSION TEXT)";
const static wxString CREATE_METADATA_TABLE_SQL_IDX1 =
    "CREATE UNIQUE INDEX IF NOT EXISTS METADATA_TABLE_IDX_1 ON METADATA_TABLE(SCHEMA_NAME)";
//------------------------------------------------
// Scope table
//------------------------------------------------
const static wxString CREATE_SCOPE_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS SCOPE_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCOPE_TYPE INTEGER, " // The scope type: 0 for namespace, 1 for class
    "SCOPE_ID INTEGER NOT NULL DEFAULT -1, "
    "NAME TEXT, "     // no scope, just the class name
    "FULLNAME TEXT, " // full path
    "EXTENDS TEXT DEFAULT '', "
    "IMPLEMENTS TEXT DEFAULT '', "
    "USING_TRAITS TEXT DEFAULT '', "
    "FLAGS INTEGER DEFAULT 0, "
    "DOC_COMMENT TEXT DEFAULT '', "
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT DEFAULT '')";

const static wxString CREATE_SCOPE_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_1 ON SCOPE_TABLE(SCOPE_ID)";
const static wxString CREATE_SCOPE_TABLE_SQL_IDX2 =
    "CREATE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_2 ON SCOPE_TABLE(FILE_NAME)";
const static wxString CREATE_SCOPE_TABLE_SQL_IDX3 = "CREATE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_3 ON SCOPE_TABLE(NAME)";
const static wxString CREATE_SCOPE_TABLE_SQL_IDX4 =
    "CREATE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_4 ON SCOPE_TABLE(SCOPE_TYPE)";
const static wxString CREATE_SCOPE_TABLE_SQL_IDX5 =
    "CREATE UNIQUE INDEX IF NOT EXISTS SCOPE_TABLE_IDX_5 ON SCOPE_TABLE(FULLNAME)";

//------------------------------------------------
// Function table
//------------------------------------------------
const static wxString CREATE_FUNCTION_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS FUNCTION_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCOPE_ID INTEGER NOT NULL DEFAULT -1, "
    "NAME TEXT, "         // no scope, just the function name
    "FULLNAME TEXT, "     // Fullname with scope
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
    "CREATE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_2 ON FUNCTION_TABLE(FILE_NAME)";
const static wxString CREATE_FUNCTION_TABLE_SQL_IDX3 =
    "CREATE UNIQUE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_3 ON FUNCTION_TABLE(FULLNAME)";
const static wxString CREATE_FUNCTION_TABLE_SQL_IDX4 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_4 ON FUNCTION_TABLE(NAME)";
const static wxString CREATE_FUNCTION_TABLE_SQL_IDX5 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_TABLE_IDX_5 ON FUNCTION_TABLE(LINE_NUMBER)";

//------------------------------------------------
// Function Alias table
//------------------------------------------------
const static wxString CREATE_FUNCTION_ALIAS_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS FUNCTION_ALIAS_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCOPE_ID INTEGER NOT NULL DEFAULT -1, "
    "NAME TEXT, "     // no scope, just the function name
    "REALNAME TEXT, " // The fullname of the actual function we are referencing
    "FULLNAME TEXT, " // Fullname with scope (of the alias name)
    "SCOPE TEXT, "    // Usually, this means the namespace\class
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT )";

const static wxString CREATE_FUNCTION_ALIAS_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_ALIAS_TABLE_IDX_1 ON FUNCTION_ALIAS_TABLE(SCOPE_ID)";
const static wxString CREATE_FUNCTION_ALIAS_TABLE_SQL_IDX2 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_ALIAS_TABLE_IDX_2 ON FUNCTION_ALIAS_TABLE(NAME)";
const static wxString CREATE_FUNCTION_ALIAS_TABLE_SQL_IDX3 =
    "CREATE INDEX IF NOT EXISTS FUNCTION_ALIAS_TABLE_IDX_3 ON FUNCTION_ALIAS_TABLE(REALNAME)";
const static wxString CREATE_FUNCTION_ALIAS_TABLE_SQL_IDX4 =
    "CREATE UNIQUE INDEX IF NOT EXISTS FUNCTION_ALIAS_TABLE_IDX_4 ON FUNCTION_ALIAS_TABLE(NAME,REALNAME,SCOPE_ID)";

//------------------------------------------------
// Variables table
//------------------------------------------------
const static wxString CREATE_VARIABLES_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS VARIABLES_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCOPE_ID INTEGER NOT NULL DEFAULT -1, " // for global variable or class member this will be the scope_id parent id
    "FUNCTION_ID INTEGER NOT NULL DEFAULT -1, " // for function argument
    "NAME TEXT, "                               // no scope, just the function name
    "FULLNAME TEXT, "                           // Fullname with scope
    "SCOPE TEXT, "                              // Usually, this means the namespace\class
    "TYPEHINT TEXT, "                           // the Variable type hint
    "DEFAULT_VALUE TEXT, "                      // Default value
    "FLAGS INTEGER DEFAULT 0, "
    "DOC_COMMENT TEXT, "
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT )";

const static wxString CREATE_VARIABLES_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_1 ON VARIABLES_TABLE(SCOPE_ID)";
const static wxString CREATE_VARIABLES_TABLE_SQL_IDX2 =
    "CREATE UNIQUE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_2 ON VARIABLES_TABLE(SCOPE, NAME, FUNCTION_ID, SCOPE_ID)";
const static wxString CREATE_VARIABLES_TABLE_SQL_IDX3 =
    "CREATE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_3 ON VARIABLES_TABLE(FILE_NAME)";
const static wxString CREATE_VARIABLES_TABLE_SQL_IDX4 =
    "CREATE INDEX IF NOT EXISTS VARIABLES_TABLE_IDX_4 ON VARIABLES_TABLE(FUNCTION_ID)";

//------------------------------------------------
// Variables table
//------------------------------------------------
const static wxString CREATE_PHPDOC_VAR_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS PHPDOC_VAR_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "SCOPE_ID INTEGER NOT NULL DEFAULT -1, " // for global variable or class member this will be the scope_id parent id
    "NAME TEXT, "                            // the name
    "TYPE TEXT, "                            // the type, full path
    "LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
    "FILE_NAME TEXT )";

const static wxString CREATE_PHPDOC_VAR_TABLE_SQL_IDX1 =
    "CREATE INDEX IF NOT EXISTS PHPDOC_VAR_TABLE_SQL_IDX1 ON PHPDOC_VAR_TABLE(SCOPE_ID)";
const static wxString CREATE_PHPDOC_VAR_TABLE_SQL_IDX2 =
    "CREATE INDEX IF NOT EXISTS PHPDOC_VAR_TABLE_SQL_IDX2 ON PHPDOC_VAR_TABLE(FILE_NAME)";

//------------------------------------------------
// Files table
//------------------------------------------------
const static wxString CREATE_FILES_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS FILES_TABLE(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
    "FILE_NAME TEXT, "                        // for global variable or class member this will be the scope_id parent id
    "LAST_UPDATED INTEGER NOT NULL DEFAULT 0" // for function argument
    ")";
const static wxString CREATE_FILES_TABLE_SQL_IDX1 =
    "CREATE UNIQUE INDEX IF NOT EXISTS FILES_TABLE_IDX_1 ON FILES_TABLE(FILE_NAME)";

PHPLookupTable::PHPLookupTable()
    : m_sizeLimit(50)
{
}

PHPLookupTable::~PHPLookupTable() { Close(); }

PHPEntityBase::Ptr_t PHPLookupTable::FindMemberOf(wxLongLong parentDbId, const wxString& exactName, size_t flags)
{
    // find the entity
    PHPEntityBase::Ptr_t scope = DoFindScope(parentDbId);
    if(scope && scope->Cast<PHPEntityClass>()) {
        std::vector<wxLongLong> parents;
        std::set<wxLongLong> parentsVisited;

        DoGetInheritanceParentIDs(scope, parents, parentsVisited, flags & kLookupFlags_Parent);
        // std::reverse(parents.begin(), parents.end());

        // Parents should now contain an ordered list of all the inheritance
        for(size_t i = 0; i < parents.size(); ++i) {
            PHPEntityBase::Ptr_t match = DoFindMemberOf(parents.at(i), exactName);
            if(match) {
                PHPEntityBase::List_t matches;
                matches.push_back(match);
                DoFixVarsDocComment(matches, parentDbId);
                return match;
            }
        }
    } else {
        // namespace
        return DoFindMemberOf(parentDbId, exactName, true);
    }
    return PHPEntityBase::Ptr_t(NULL);
}

PHPEntityBase::Ptr_t PHPLookupTable::FindScope(const wxString& fullname)
{
    wxString scopeName = fullname;
    scopeName.Trim().Trim(false);
    if(scopeName.EndsWith("\\") && scopeName.length() > 1) {
        scopeName.RemoveLast();
    }
    return DoFindScope(scopeName);
}

void PHPLookupTable::Open(const wxFileName& dbfile)
{
    try {

        if(dbfile.Exists()) {
            // Check for its integrity. If the database is corrupted,
            // it will be deleted
            EnsureIntegrity(dbfile);
        }

        wxFileName::Mkdir(dbfile.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        m_db.Open(dbfile.GetFullPath());
        m_db.SetBusyTimeout(10); // Don't lock when we cant access to the database
        m_filename = dbfile;
        CreateSchema();

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::Open" << e.GetMessage() << clEndl;
    }
}

void PHPLookupTable::Open(const wxString& workspacePath)
{
    wxFileName fnDBFile(workspacePath, "phpsymbols.db");

    // ensure that the database directory exists
    fnDBFile.AppendDir(".codelite");
    fnDBFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    Open(fnDBFile);
}

void PHPLookupTable::CreateSchema()
{
    wxString schemaVersion;
    try {
        wxString sql;
        sql = wxT("PRAGMA journal_mode = ON;");
        m_db.ExecuteUpdate(sql);

        sql = wxT("PRAGMA synchronous = OFF;");
        m_db.ExecuteUpdate(sql);

        sql = wxT("PRAGMA temp_store = MEMORY;");
        m_db.ExecuteUpdate(sql);

        wxSQLite3Statement st =
            m_db.PrepareStatement("select SCHEMA_VERSION from METADATA_TABLE where SCHEMA_NAME=:SCHEMA_NAME");
        st.Bind(st.GetParamIndex(":SCHEMA_NAME"), "CODELITEPHP");
        wxSQLite3ResultSet res = st.ExecuteQuery();
        if(res.NextRow()) {
            schemaVersion = res.GetString("SCHEMA_VERSION");
        }
    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }

    if(schemaVersion != PHP_SCHEMA_VERSION) {
        // Drop the tables and recreate the schema from scratch
        m_db.ExecuteUpdate("drop table if exists SCHEMA_VERSION");
        m_db.ExecuteUpdate("drop table if exists SCOPE_TABLE");
        m_db.ExecuteUpdate("drop table if exists FUNCTION_TABLE");
        m_db.ExecuteUpdate("drop table if exists FUNCTION_ALIAS_TABLE");
        m_db.ExecuteUpdate("drop table if exists VARIABLES_TABLE");
        m_db.ExecuteUpdate("drop table if exists FILES_TABLE");
        m_db.ExecuteUpdate("drop table if exists PHPDOC_VAR_TABLE");
    }

    try {

        // Metadata
        m_db.ExecuteUpdate(CREATE_METADATA_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_METADATA_TABLE_SQL_IDX1);

        // class / namespace table "scope"
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX3);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX4);
        m_db.ExecuteUpdate(CREATE_SCOPE_TABLE_SQL_IDX5);

        // function table
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX3);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX4);
        m_db.ExecuteUpdate(CREATE_FUNCTION_TABLE_SQL_IDX5);

        // function alias table
        m_db.ExecuteUpdate(CREATE_FUNCTION_ALIAS_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_FUNCTION_ALIAS_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_FUNCTION_ALIAS_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_FUNCTION_ALIAS_TABLE_SQL_IDX3);
        m_db.ExecuteUpdate(CREATE_FUNCTION_ALIAS_TABLE_SQL_IDX4);

        // variables (function args, globals class members and consts)
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX2);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX3);
        m_db.ExecuteUpdate(CREATE_VARIABLES_TABLE_SQL_IDX4);

        // phpdoc var table
        m_db.ExecuteUpdate(CREATE_PHPDOC_VAR_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_PHPDOC_VAR_TABLE_SQL_IDX1);
        m_db.ExecuteUpdate(CREATE_PHPDOC_VAR_TABLE_SQL_IDX2);

        // Files
        m_db.ExecuteUpdate(CREATE_FILES_TABLE_SQL);
        m_db.ExecuteUpdate(CREATE_FILES_TABLE_SQL_IDX1);

        // Update the schema version
        wxSQLite3Statement st =
            m_db.PrepareStatement("replace into METADATA_TABLE (ID, SCHEMA_NAME, SCHEMA_VERSION) VALUES (NULL, "
                                  ":SCHEMA_NAME, :SCHEMA_VERSION)");
        st.Bind(st.GetParamIndex(":SCHEMA_NAME"), "CODELITEPHP");
        st.Bind(st.GetParamIndex(":SCHEMA_VERSION"), PHP_SCHEMA_VERSION);
        st.ExecuteUpdate();

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::CreateSchema" << e.GetMessage() << endl;
    }
}

void PHPLookupTable::UpdateSourceFile(PHPSourceFile& source, bool autoCommit)
{
    try {
        if(autoCommit)
            m_db.Begin();

        // Delete all entries for this file
        DeleteFileEntries(source.GetFilename(), false);

        // Store new entries
        PHPEntityBase::Ptr_t topNamespace = source.Namespace();
        if(topNamespace) {
            topNamespace->StoreRecursive(this);
            UpdateFileLastParsedTimestamp(source.GetFilename());
        }

        // Store defines
        // --------------
        // 'defines' are handled separately as they dont really comply to the standard PHP rules
        // define() will define constants exactly as specified.
        // The following code will define the constant "MESSAGE" in the global namespace (i.e. "\MESSAGE").
        // <?php
        //  namespace test;
        //  define('MESSAGE', 'Hello world!');
        // ?>
        // In the above code, the constant 'MESSAGE' is defined in the GLOBAL namespace even though the
        // define was called inside namespace 'test'
        // For this reason, we need get the list of defined parsed in the source file and associate them
        // with their namespace (we either load the namespace from the database or create one)

        if(!source.GetDefines().empty()) {
            const PHPEntityBase::List_t& defines = source.GetDefines();
            PHPEntityBase::List_t::const_iterator iter = defines.begin();
            PHPEntityBase::Map_t nsMap;
            for(; iter != defines.end(); ++iter) {
                PHPEntityBase::Ptr_t pDefine = *iter;
                PHPEntityBase::Ptr_t pNamespace(NULL);

                wxString nameSpaceName, shortName;
                DoSplitFullname(pDefine->GetFullName(), nameSpaceName, shortName);

                PHPEntityBase::Map_t::iterator nsIter = nsMap.find(nameSpaceName);
                if(nsIter == nsMap.end()) {
                    // we did not load this namespace yet => load and cache it
                    pNamespace = CreateNamespaceForDefine(pDefine);
                    nsMap.insert(std::make_pair(pNamespace->GetFullName(), pNamespace));

                } else {
                    // We already loaded this namespace
                    pNamespace = nsIter->second;
                }
                pNamespace->AddChild(pDefine);
            }

            // Now, loop over the namespace map and store all entries
            PHPEntityBase::Map_t::iterator nsIter = nsMap.begin();
            for(; nsIter != nsMap.end(); ++nsIter) {
                nsIter->second->StoreRecursive(this);
            }
        }

        if(autoCommit)
            m_db.Commit();

    } catch(wxSQLite3Exception& e) {
        if(autoCommit)
            m_db.Rollback();
        clWARNING() << "PHPLookupTable::SaveSourceFile" << e.GetMessage() << endl;
    }
}

PHPEntityBase::Ptr_t PHPLookupTable::DoFindMemberOf(wxLongLong parentDbId, const wxString& exactName,
                                                    bool parentIsNamespace)
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
            // Search functions alias table
            wxString sql;
            sql << "SELECT * from FUNCTION_ALIAS_TABLE WHERE SCOPE_ID=" << parentDbId << " AND NAME='" << exactName
                << "'";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityFunctionAlias());
                match->FromResultSet(res);
                PHPEntityBase::Ptr_t pFunc = FindFunction(match->Cast<PHPEntityFunctionAlias>()->GetRealname());
                if(pFunc) {
                    match->Cast<PHPEntityFunctionAlias>()->SetFunc(pFunc);
                    matches.push_back(match);
                }
            }
        }

        if(matches.empty() && parentIsNamespace) {
            // search the scope table as well
            wxString sql;
            sql << "SELECT * from SCOPE_TABLE WHERE SCOPE_ID=" << parentDbId << " AND NAME='" << exactName << "'";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                ePhpScopeType st = kPhpScopeTypeAny;
                st =
                    res.GetInt("SCOPE_TYPE", 1) == kPhpScopeTypeNamespace ? kPhpScopeTypeNamespace : kPhpScopeTypeClass;

                PHPEntityBase::Ptr_t match = NewEntity("SCOPE_TABLE", st);
                if(match) {
                    match->FromResultSet(res);
                    matches.push_back(match);
                }
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

            // Fix variables type using the PHPDOC_VAR_TABLE content for this class
            DoFixVarsDocComment(matches, parentDbId);

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
        clWARNING() << "PHPLookupTable::DoFindMemberOf" << e.GetMessage() << endl;
    }
    return PHPEntityBase::Ptr_t(NULL);
}

void PHPLookupTable::DoGetInheritanceParentIDs(PHPEntityBase::Ptr_t cls, std::vector<wxLongLong>& parents,
                                               std::set<wxLongLong>& parentsVisited, bool excludeSelf)
{
    if(!excludeSelf) {
        parents.push_back(cls->GetDbId());
    }

    parentsVisited.insert(cls->GetDbId());
    wxArrayString parentsArr = cls->Cast<PHPEntityClass>()->GetInheritanceArray();
    for(size_t i = 0; i < parentsArr.GetCount(); ++i) {
        PHPEntityBase::Ptr_t parent = FindClass(parentsArr.Item(i));
        if(parent && !parentsVisited.count(parent->GetDbId())) {
            DoGetInheritanceParentIDs(parent, parents, parentsVisited, false);
        }
        parent.Reset(nullptr);
    }
}

PHPEntityBase::Ptr_t PHPLookupTable::DoFindScope(const wxString& fullname, ePhpScopeType scopeType)
{
    // locate the scope
    try {
        wxString sql;

        // limit by 2 for performance reason
        // we will return NULL incase the number of matches is greater than 1...
        sql << "SELECT * from SCOPE_TABLE WHERE FULLNAME='" << fullname << "'";
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
                match.Reset(new PHPEntityNamespace());
            } else {
                // class
                match.Reset(new PHPEntityClass());
            }
            match->FromResultSet(res);
        }
        return match;

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::FindScope" << e.GetMessage() << endl;
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
                match.Reset(new PHPEntityNamespace());
            } else {
                // class
                match.Reset(new PHPEntityClass());
            }
            match->FromResultSet(res);
            return match;
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::DoFindScope" << e.GetMessage() << endl;
    }
    return PHPEntityBase::Ptr_t(NULL);
}

PHPEntityBase::Ptr_t PHPLookupTable::FindClass(wxLongLong id) { return DoFindScope(id, kPhpScopeTypeClass); }

PHPEntityBase::List_t PHPLookupTable::FindChildren(wxLongLong parentId, size_t flags, const wxString& nameHint)
{
    PHPEntityBase::List_t matches, matchesNoAbstracts;
    PHPEntityBase::Ptr_t scope = DoFindScope(parentId);
    if(scope && scope->Is(kEntityTypeClass)) {
        std::vector<wxLongLong> parents;
        std::set<wxLongLong> parentsVisited;

        DoGetInheritanceParentIDs(scope, parents, parentsVisited, flags & kLookupFlags_Parent);
        // Reverse the order of the parents
        std::reverse(parents.begin(), parents.end());

        for(size_t i = 0; i < parents.size(); ++i) {
            DoFindChildren(matches, parents.at(i), flags, nameHint);
        }

        // Filter out abstract functions
        if(!(flags & kLookupFlags_IncludeAbstractMethods)) {
            PHPEntityBase::List_t::iterator iter = matches.begin();
            for(; iter != matches.end(); ++iter) {
                PHPEntityBase::Ptr_t child = *iter;
                if(child->Is(kEntityTypeFunction) && child->HasFlag(kFunc_Abstract))
                    continue;
                matchesNoAbstracts.push_back(child);
            }
            matches.swap(matchesNoAbstracts);
        }
    } else if(scope && scope->Is(kEntityTypeNamespace)) {
        DoFindChildren(matches, parentId, flags | kLookupFlags_NameHintIsScope, nameHint);
    }
    return matches;
}

PHPEntityBase::List_t PHPLookupTable::LoadFunctionArguments(wxLongLong parentId)
{
    PHPEntityBase::List_t matches;

    try {
        {
            // load functions
            wxString sql;
            sql << "SELECT * from VARIABLES_TABLE WHERE FUNCTION_ID=" << parentId << " ORDER BY ID ASC";

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityVariable());
                match->FromResultSet(res);
                matches.push_back(match);
            }
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::LoadFunctionArguments" << e.GetMessage() << endl;
    }
    return matches;
}

wxString PHPLookupTable::EscapeWildCards(const wxString& str)
{
    wxString s(str);
    s.Replace(wxT("_"), wxT("^_"));
    return s;
}

void PHPLookupTable::DoAddLimit(wxString& sql) { sql << " LIMIT " << m_sizeLimit; }

void PHPLookupTable::DoAddNameFilter(wxString& sql, const wxString& nameHint, size_t flags)
{
    wxString name = nameHint;
    name.Trim().Trim(false);

    if(name.IsEmpty()) {
        sql.Trim();
        if(sql.EndsWith("AND") || sql.EndsWith("and")) {
            sql.RemoveLast(3);
        }
        sql << " ";
        return;
    }

    if(flags & kLookupFlags_ExactMatch && !name.IsEmpty()) {
        sql << " NAME = '" << name << "'";

    } else if(flags & kLookupFlags_Contains && !name.IsEmpty()) {
        sql << " NAME LIKE '%%" << EscapeWildCards(name) << "%%' ESCAPE '^'";

    } else if(flags & kLookupFlags_StartsWith && !name.IsEmpty()) {
        sql << " NAME LIKE '" << EscapeWildCards(name) << "%%' ESCAPE '^'";
    }
}

void PHPLookupTable::LoadAllByFilter(PHPEntityBase::List_t& matches, const wxString& nameHint, eLookupFlags flags)
{
    try {
        // Split the nameHint to spaces/tabs
        LoadFromTableByNameHint(matches, "SCOPE_TABLE", nameHint, flags);
        LoadFromTableByNameHint(matches, "FUNCTION_TABLE", nameHint, flags);

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::LoadAllByFilter:" << e.GetMessage() << clEndl;
    }
}

PHPEntityBase::Ptr_t PHPLookupTable::NewEntity(const wxString& tableName, ePhpScopeType scopeType)
{
    if(tableName == "FUNCTION_TABLE") {
        return PHPEntityBase::Ptr_t(new PHPEntityFunction());
    } else if(tableName == "VARIABLES_TABLE") {
        return PHPEntityBase::Ptr_t(new PHPEntityVariable());
    } else if(tableName == "SCOPE_TABLE" && scopeType == kPhpScopeTypeNamespace) {
        return PHPEntityBase::Ptr_t(new PHPEntityNamespace());
    } else if(tableName == "SCOPE_TABLE" && scopeType == kPhpScopeTypeClass) {
        return PHPEntityBase::Ptr_t(new PHPEntityClass());
    } else {
        return PHPEntityBase::Ptr_t(NULL);
    }
}

void PHPLookupTable::LoadFromTableByNameHint(PHPEntityBase::List_t& matches, const wxString& tableName,
                                             const wxString& nameHint, eLookupFlags flags)
{
    wxArrayString parts = ::wxStringTokenize(nameHint, " \t", wxTOKEN_STRTOK);
    if(parts.IsEmpty()) {
        return;
    }

    // Build the filter query
    wxString filterQuery = "where ";
    wxString sql;
    for(size_t i = 0; i < parts.size(); ++i) {
        wxString tmpName = parts.Item(i);
        tmpName.Replace(wxT("_"), wxT("^_"));
        filterQuery << "fullname like '%%" << tmpName << "%%' " << ((i == (parts.size() - 1)) ? "" : "AND ");
    }
    sql << "select * from " << tableName << " " << filterQuery << " ESCAPE '^' ";
    DoAddLimit(sql);

    try {
        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();

        while(res.NextRow()) {
            ePhpScopeType st = kPhpScopeTypeAny;
            if(tableName == "SCOPE_TABLE") {
                st =
                    res.GetInt("SCOPE_TYPE", 1) == kPhpScopeTypeNamespace ? kPhpScopeTypeNamespace : kPhpScopeTypeClass;
            }

            PHPEntityBase::Ptr_t match = NewEntity(tableName, st);
            if(match) {
                match->FromResultSet(res);
                matches.push_back(match);
            }
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::LoadFromTableByNameHint:" << sql << ":" << e.GetMessage() << clEndl;
    }
}

void PHPLookupTable::DeleteFileEntries(const wxFileName& filename, bool autoCommit)
{
    try {
        if(autoCommit)
            m_db.Begin();
        {
            // When deleting from the 'SCOPE_TABLE' don't remove namespaces
            // since they can be still be pointed by other entries in the database
            wxString sql;
            sql << "delete from SCOPE_TABLE where FILE_NAME=:FILE_NAME AND SCOPE_TYPE != "
                << (int)kPhpScopeTypeNamespace;
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from FUNCTION_TABLE where FILE_NAME=:FILE_NAME";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from FUNCTION_ALIAS_TABLE where FILE_NAME=:FILE_NAME";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from VARIABLES_TABLE where FILE_NAME=:FILE_NAME";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from FILES_TABLE where FILE_NAME=:FILE_NAME";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from PHPDOC_VAR_TABLE where FILE_NAME=:FILE_NAME";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
            st.ExecuteUpdate();
        }

        if(autoCommit)
            m_db.Commit();
    } catch(wxSQLite3Exception& e) {
        if(autoCommit)
            m_db.Rollback();
        clWARNING() << "PHPLookupTable::DeleteFileEntries" << e.GetMessage() << endl;
    }
}

void PHPLookupTable::Close()
{
    try {
        if(m_db.IsOpen()) {
            m_db.Close();
        }
        m_filename.Clear();
        m_allClasses.clear();

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::Close" << e.GetMessage() << endl;
    }
}

bool PHPLookupTable::IsOpened() const { return m_db.IsOpen(); }

void PHPLookupTable::DoFindChildren(PHPEntityBase::List_t& matches, wxLongLong parentId, size_t flags,
                                    const wxString& nameHint)
{
    // Find members of of parentDbID
    try {
        // Load classes
        if(!(flags & kLookupFlags_FunctionsAndConstsOnly)) {
            wxString sql;
            sql << "SELECT * from SCOPE_TABLE WHERE SCOPE_ID=" << parentId << " AND SCOPE_TYPE = 1 AND ";
            DoAddNameFilter(sql, nameHint, flags);
            DoAddLimit(sql);

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityClass());
                match->FromResultSet(res);
                matches.push_back(match);
            }
        }

        {
            // load functions
            wxString sql;
            sql << "SELECT * from FUNCTION_TABLE WHERE SCOPE_ID=" << parentId << " AND ";
            DoAddNameFilter(sql, nameHint, flags);
            DoAddLimit(sql);

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityFunction());
                match->FromResultSet(res);
                bool isStaticFunction = match->HasFlag(kFunc_Static);
                if(isStaticFunction) {
                    // always return static functions
                    matches.push_back(match);

                } else {
                    // Non static function.
                    if(!(flags & kLookupFlags_Static)) {
                        matches.push_back(match);
                    }
                }
            }
        }

        {
            // load function aliases
            wxString sql;
            sql << "SELECT * from FUNCTION_ALIAS_TABLE WHERE SCOPE_ID=" << parentId << " AND ";
            DoAddNameFilter(sql, nameHint, flags);
            DoAddLimit(sql);

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();
            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityFunctionAlias());
                match->FromResultSet(res);
                const wxString& realFuncName = match->Cast<PHPEntityFunctionAlias>()->GetRealname();
                // Load the function pointed by this reference
                PHPEntityBase::Ptr_t pFunc = FindFunction(realFuncName);
                if(pFunc) {
                    // Keep the reference to the real function
                    match->Cast<PHPEntityFunctionAlias>()->SetFunc(pFunc);
                    matches.push_back(match);
                }
            }
        }

        {
            // Add members from the variables table
            wxString sql;
            sql << "SELECT * from VARIABLES_TABLE WHERE SCOPE_ID=" << parentId << " AND ";
            DoAddNameFilter(sql, nameHint, flags);
            DoAddLimit(sql);

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityVariable());
                match->FromResultSet(res);

                if(flags & kLookupFlags_FunctionsAndConstsOnly) {
                    // Filter non consts from the list
                    if(!match->Cast<PHPEntityVariable>()->IsConst() && !match->Cast<PHPEntityVariable>()->IsDefine()) {
                        continue;
                    }
                }

                bool isConst = match->Cast<PHPEntityVariable>()->IsConst();
                bool isStatic = match->Cast<PHPEntityVariable>()->IsStatic();
                bool bAddIt = ((isStatic || isConst) && CollectingStatics(flags)) ||
                              (!isStatic && !isConst && !CollectingStatics(flags));
                if(bAddIt) {
                    matches.push_back(match);
                }
            }
            DoFixVarsDocComment(matches, parentId);
        }

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::FindChildren" << e.GetMessage() << endl;
    }
}

wxLongLong PHPLookupTable::GetFileLastParsedTimestamp(const wxFileName& filename)
{
    try {
        wxSQLite3Statement st =
            m_db.PrepareStatement("SELECT LAST_UPDATED FROM FILES_TABLE WHERE FILE_NAME=:FILE_NAME");
        st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
        wxSQLite3ResultSet res = st.ExecuteQuery();
        if(res.NextRow()) {
            return res.GetInt64("LAST_UPDATED");
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::FindChildren" << e.GetMessage() << endl;
    }
    return 0;
}

void PHPLookupTable::UpdateFileLastParsedTimestamp(const wxFileName& filename)
{
    try {
        wxSQLite3Statement st = m_db.PrepareStatement(
            "REPLACE INTO FILES_TABLE (ID, FILE_NAME, LAST_UPDATED) VALUES (NULL, :FILE_NAME, :LAST_UPDATED)");
        st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
        st.Bind(st.GetParamIndex(":LAST_UPDATED"), (wxLongLong)time(NULL));
        st.ExecuteUpdate();

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::UpdateFileLastParsedTimestamp" << e.GetMessage() << endl;
    }
}

void PHPLookupTable::ClearAll(bool autoCommit)
{
    try {
        if(autoCommit)
            m_db.Begin();
        {
            wxString sql;
            sql << "delete from SCOPE_TABLE";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from FUNCTION_TABLE";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from VARIABLES_TABLE";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from FILES_TABLE";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.ExecuteUpdate();
        }

        {
            wxString sql;
            sql << "delete from FUNCTION_ALIAS_TABLE";
            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            st.ExecuteUpdate();
        }

        if(autoCommit)
            m_db.Commit();
    } catch(wxSQLite3Exception& e) {
        if(autoCommit)
            m_db.Rollback();
        clWARNING() << "PHPLookupTable::ClearAll" << e.GetMessage() << endl;
    }
}

PHPEntityBase::Ptr_t PHPLookupTable::FindFunction(const wxString& fullname)
{
    // locate the scope
    try {
        wxString sql;

        // limit by 2 for performance reason
        // we will return NULL incase the number of matches is greater than 1...
        sql << "SELECT * from FUNCTION_TABLE WHERE FULLNAME='" << fullname << "'";
        sql << " LIMIT 2";

        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        PHPEntityBase::Ptr_t match(NULL);

        while(res.NextRow()) {
            if(match) {
                // only one match
                return PHPEntityBase::Ptr_t(NULL);
            }

            match.Reset(new PHPEntityFunction());
            match->FromResultSet(res);
        }
        return match;

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::FindFunction" << e.GetMessage() << endl;
    }
    return PHPEntityBase::Ptr_t(NULL);
}

PHPEntityBase::List_t PHPLookupTable::FindGlobalFunctionAndConsts(size_t flags, const wxString& nameHint)
{
    PHPEntityBase::List_t matches;
    // Sanity
    if(nameHint.IsEmpty())
        return matches;
    // First, locate the global namespace in the database
    PHPEntityBase::Ptr_t globalNs = FindScope("\\");
    if(!globalNs)
        return matches;
    DoFindChildren(matches, globalNs->GetDbId(), kLookupFlags_FunctionsAndConstsOnly | flags, nameHint);
    return matches;
}

PHPEntityBase::Ptr_t PHPLookupTable::CreateNamespaceForDefine(PHPEntityBase::Ptr_t define)
{
    wxString nameSpaceName, shortName;
    DoSplitFullname(define->GetFullName(), nameSpaceName, shortName);

    PHPEntityBase::Ptr_t pNamespace = DoFindScope(nameSpaceName, kPhpScopeTypeNamespace);
    if(!pNamespace) {
        // Create it
        pNamespace.Reset(new PHPEntityNamespace());
        pNamespace->SetFullName(nameSpaceName);
        pNamespace->SetShortName(nameSpaceName.AfterLast('\\'));
        pNamespace->SetFilename(define->GetFilename());
        pNamespace->SetLine(define->GetLine());
        pNamespace->Store(this);
    }
    return pNamespace;
}

void PHPLookupTable::DoSplitFullname(const wxString& fullname, wxString& ns, wxString& shortName)
{
    // get the namespace part
    ns = fullname.BeforeLast('\\');
    if(!ns.StartsWith("\\")) {
        // This means that the fullname contained a single '\'
        // and we removed it
        ns.Prepend("\\");
    }
    // Now the short name
    shortName = fullname.AfterLast('\\');
}

PHPEntityBase::List_t PHPLookupTable::FindNamespaces(const wxString& fullnameStartsWith,
                                                     const wxString& shortNameContains)
{
    PHPEntityBase::List_t matches;
    try {
        wxString sql;
        wxString prefix = fullnameStartsWith;
        sql << "SELECT * from SCOPE_TABLE WHERE SCOPE_TYPE = 0 ";
        DoAddLimit(sql);

        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();

        wxString fullpath = fullnameStartsWith;
        if(!shortNameContains.IsEmpty()) {
            if(!fullpath.EndsWith("\\")) {
                fullpath << "\\";
            }
            fullpath << shortNameContains;
        }

        while(res.NextRow()) {
            PHPEntityBase::Ptr_t match(new PHPEntityNamespace());
            match->FromResultSet(res);
            if(match->Cast<PHPEntityNamespace>()->GetParentNamespace() == fullnameStartsWith &&
               match->GetShortName().StartsWith(shortNameContains)) {
                matches.push_back(match);
            }
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::FindNamespaces" << e.GetMessage() << endl;
    }
    return matches;
}

PHPEntityBase::Ptr_t PHPLookupTable::FindFunctionByLineAndFile(const wxFileName& filename, int line)
{
    try {
        wxString sql;
        // Try to locate function first
        sql << "SELECT * from FUNCTION_TABLE WHERE FILE_NAME=:FILE_NAME AND LINE_NUMBER=:LINE_NUMBER LIMIT 1";
        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        st.Bind(st.GetParamIndex(":FILE_NAME"), filename.GetFullPath());
        st.Bind(st.GetParamIndex(":LINE_NUMBER"), line);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        if(res.NextRow()) {
            PHPEntityBase::Ptr_t match(new PHPEntityFunction());
            match->FromResultSet(res);
            return match;
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::FindFunctionByLineAndFile" << e.GetMessage() << endl;
    }
    return NULL;
}

void PHPLookupTable::ResetDatabase()
{
    wxFileName curfile = m_filename;
    Close(); // Close the databse releasing any file capture we have
    // Delete the file
    if(curfile.IsOk() && curfile.Exists()) {
        // Delete it from the file system
        wxLogNull noLog;
        if(!clRemoveFile(curfile.GetFullPath())) {
            // ??
        }
    }
    Open(curfile);
}

bool PHPLookupTable::CheckDiskImage(wxSQLite3Database& db, const wxFileName& filename)
{
    try {
        wxSQLite3ResultSet res = db.ExecuteQuery("PRAGMA integrity_check");
        if(res.NextRow()) {
            wxString value = res.GetString(0);
            clDEBUG() << "PHP: 'PRAGMA integrity_check' returned:" << value << clEndl;
            return (value.Lower() == "ok");
        } else {
            return false;
        }
    } catch(wxSQLite3Exception& exec) {
        // this can only happen if we have a corrupt disk image
        clWARNING() << "PHP: exception caught:" << exec.GetMessage() << clEndl;
        clWARNING() << "PHP: database image is corrupted:" << filename.GetFullPath() << clEndl;
        return false;
    }
    return true;
}

void PHPLookupTable::EnsureIntegrity(const wxFileName& filename)
{
    wxSQLite3Database db;
    db.Open(filename.GetFullPath());
    if(db.IsOpen()) {
        if(!CheckDiskImage(db, filename)) {
            // disk image is malformed
            db.Close();
            wxLogNull noLog;
            clRemoveFile(filename.GetFullPath());
        }
    }
}

PHPEntityBase::List_t PHPLookupTable::FindSymbol(const wxString& name)
{
    // locate the scope
    PHPEntityBase::List_t matches;
    try {
        {
            //---------------------------------------------------------------------
            // Load scopes (classes / namespaces)
            //---------------------------------------------------------------------
            wxString sql;
            sql << "SELECT * from SCOPE_TABLE WHERE NAME='" << name << "'";

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                ePhpScopeType st = kPhpScopeTypeAny;
                st =
                    res.GetInt("SCOPE_TYPE", 1) == kPhpScopeTypeNamespace ? kPhpScopeTypeNamespace : kPhpScopeTypeClass;

                PHPEntityBase::Ptr_t match = NewEntity("SCOPE_TABLE", st);
                if(match) {
                    match->FromResultSet(res);
                    matches.push_back(match);
                }
            }
        }

        {
            //---------------------------------------------------------------------
            // Load functions
            //---------------------------------------------------------------------
            wxString sql;
            sql << "SELECT * from FUNCTION_TABLE WHERE NAME='" << name << "'";

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityFunction());
                match->FromResultSet(res);
                matches.push_back(match);
            }
        }

        {
            //---------------------------------------------------------------------
            // Load function aliases
            //---------------------------------------------------------------------
            wxString sql;
            sql << "SELECT * from FUNCTION_ALIAS_TABLE WHERE NAME='" << name << "'";

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match(new PHPEntityFunction());
                match->FromResultSet(res);
                matches.push_back(match);
            }
        }

        {
            //---------------------------------------------------------------------
            // Load variables
            //---------------------------------------------------------------------
            wxString sql;
            sql << "SELECT * from VARIABLES_TABLE WHERE NAME='" << name << "'";

            wxSQLite3Statement st = m_db.PrepareStatement(sql);
            wxSQLite3ResultSet res = st.ExecuteQuery();

            while(res.NextRow()) {
                PHPEntityBase::Ptr_t match = NewEntity("VARIABLES_TABLE", kPhpScopeTypeAny);
                match->FromResultSet(res);
                matches.push_back(match);
            }
        }

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::FindSymbol" << e.GetMessage() << endl;
    }
    return matches;
}

void PHPLookupTable::DoFixVarsDocComment(PHPEntityBase::List_t& matches, wxLongLong parentId)
{
    // Load all PHPDocVar belonged to this class
    PHPDocVar::Map_t docs;
    wxString sql;
    sql << "SELECT * from PHPDOC_VAR_TABLE WHERE SCOPE_ID=" << parentId;
    DoAddLimit(sql);
    wxSQLite3Statement st = m_db.PrepareStatement(sql);
    wxSQLite3ResultSet res = st.ExecuteQuery();

    while(res.NextRow()) {
        PHPDocVar::Ptr_t var(new PHPDocVar());
        var->FromResultSet(res);
        docs.insert(std::make_pair(var->GetName(), var));
    }

    // Let the PHPDOC table content override the matches' type
    std::for_each(matches.begin(), matches.end(), [&](PHPEntityBase::Ptr_t match) {
        if(match->Is(kEntityTypeVariable)) {
            if(docs.count(match->GetShortName())) {
                PHPDocVar::Ptr_t docvar = docs.find(match->GetShortName())->second;
                if(!docvar->GetType().IsEmpty()) {
                    match->Cast<PHPEntityVariable>()->SetTypeHint(docvar->GetType());
                }
            }
        }
    });
}

void PHPLookupTable::UpdateClassCache(const wxString& classname)
{
    if(m_allClasses.count(classname) == 0) {
        m_allClasses.insert(classname);
    }
}

bool PHPLookupTable::ClassExists(const wxString& classname) const { return m_allClasses.count(classname) != 0; }

void PHPLookupTable::RebuildClassCache()
{
    // locate the scope
    clDEBUG() << "Rebuilding PHP class cache..." << clEndl;
    m_allClasses.clear();
    size_t count = 0;
    try {
        wxString sql;
        sql << "SELECT FULLNAME from SCOPE_TABLE WHERE SCOPE_TYPE=1";

        wxSQLite3ResultSet res = m_db.ExecuteQuery(sql);
        while(res.NextRow()) {
            UpdateClassCache(res.GetString("FULLNAME"));
            ++count;
        }

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::RebuildClassCache:" << e.GetMessage() << clEndl;
        return;
    }
    clDEBUG() << "Loading" << count << "class names into the cache" << clEndl;
    clDEBUG() << "Rebuilding PHP class cache...done" << clEndl;
}

PHPEntityBase::Ptr_t PHPLookupTable::FindFunctionNearLine(const wxFileName& filename, int lineNumber)
{
    try {
        wxString sql;

        // limit by 2 for performance reason
        // we will return NULL incase the number of matches is greater than 1...
        // SELECT * from FUNCTION_TABLE WHERE
        sql << "SELECT * from FUNCTION_TABLE WHERE FILE_NAME='" << filename.GetFullPath()
            << "' AND LINE_NUMBER <=" << lineNumber << " order by LINE_NUMBER DESC LIMIT 1";

        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        PHPEntityBase::Ptr_t match(NULL);

        if(res.NextRow()) {
            match.Reset(new PHPEntityFunction());
            match->FromResultSet(res);
        }
        return match;

    } catch(wxSQLite3Exception& e) {
        clWARNING() << "PHPLookupTable::FindFunctionNearLine:" << e.GetMessage() << clEndl;
    }
    return PHPEntityBase::Ptr_t(NULL);
}

size_t PHPLookupTable::FindFunctionsByFile(const wxFileName& filename, PHPEntityBase::List_t& functions)
{
    wxString sql;
    try {

        // limit by 2 for performance reason
        // we will return NULL incase the number of matches is greater than 1...
        // SELECT * from FUNCTION_TABLE WHERE
        sql << "SELECT * from FUNCTION_TABLE WHERE FILE_NAME='" << filename.GetFullPath()
            << "' order by LINE_NUMBER ASC";
        wxSQLite3Statement st = m_db.PrepareStatement(sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        while(res.NextRow()) {
            PHPEntityBase::Ptr_t func(new PHPEntityFunction());
            func->FromResultSet(res);
            functions.push_back(func);
        }
    } catch(wxSQLite3Exception& e) {
        clWARNING() << "SQLite 3 error:" << e.GetMessage() << clEndl;
    }
    return functions.size();
}

void PHPLookupTable::ParseFolder(const wxString& folder, const wxString& filemask, eUpdateMode updateMode)
{
    clFilesScanner scanner;
    std::vector<wxString> files;
    if(scanner.Scan(folder, files, filemask) == 0) {
        return;
    }
    std::for_each(files.begin(), files.end(), [&](const wxString& file) {
        try {
            wxFileName fnFile(file);
            bool reParseNeeded(true);
            if(updateMode == kUpdateMode_Fast) {
                // Check to see if we need to re-parse this file
                // and store it to the database

                if(!fnFile.Exists()) {
                    reParseNeeded = false;
                } else {
                    time_t lastModifiedOnDisk = fnFile.GetModificationTime().GetTicks();
                    wxLongLong lastModifiedInDB = GetFileLastParsedTimestamp(fnFile);
                    if(lastModifiedOnDisk <= lastModifiedInDB.ToLong()) {
                        reParseNeeded = false;
                    }
                }
            }

            // Ensure that the file exists
            if(!fnFile.Exists()) {
                reParseNeeded = false;
            }
            if(!reParseNeeded)
                return;

            wxString content;
            if(!FileUtils::ReadFileContent(fnFile, content, wxConvISO8859_1)) {
                clWARNING() << "PHP: Failed to read file:" << fnFile << "for parsing";
                return;
            }
            LOG_IF_TRACE { clDEBUG1() << "Parsing PHP file:" << fnFile; }
            PHPSourceFile sourceFile(content, this);
            sourceFile.SetFilename(fnFile);
            sourceFile.SetParseFunctionBody(true);
            sourceFile.Parse();
            UpdateSourceFile(sourceFile, false);
        } catch(wxSQLite3Exception& e) {
            try {
                m_db.Rollback();
            } catch(...) {
            }
            clWARNING() << "PHPLookupTable::ParseFolder:" << e.GetMessage();
        }
    });
}
