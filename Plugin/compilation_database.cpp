#include "compilation_database.h"
#include <wx/filename.h>
#include "workspace.h"
#include <wx/tokenzr.h>
#include <wx/log.h>
#include <wx/ffile.h>
#include "fileextmanager.h"
#include "project.h"
#include "json_node.h"
#include "project.h"

const wxString DB_VERSION = "2.0";

CompilationDatabase::CompilationDatabase()
    : m_db(NULL)
{
}

CompilationDatabase::CompilationDatabase(const wxString &filename)
    : m_db(NULL)
    , m_filename(filename)
{
}

CompilationDatabase::~CompilationDatabase()
{
    Close();
}

void CompilationDatabase::Open(const wxFileName& fn)
{
    // Close the old database
    if ( m_db ) {
        Close();
    }

    // Create new one
    try {

        m_db = new wxSQLite3Database();
        wxFileName dbfile(WorkspaceST::Get()->GetPrivateFolder(), "compilation.db");
        m_db->Open(dbfile.GetFullPath());
        CreateDatabase();

    } catch (wxSQLite3Exception &e) {

        delete m_db;
        m_db = NULL;

    }
}

wxFileName CompilationDatabase::GetFileName() const
{
    wxFileName dbfile;
    if ( !m_filename.IsOk() ) {
        dbfile = wxFileName(WorkspaceST::Get()->GetPrivateFolder(), "compilation.db");

    } else {
        dbfile = m_filename;
    }
    return dbfile;
}

void CompilationDatabase::CompilationLine(const wxString& filename, wxString &compliationLine, wxString &cwd)
{
    if ( !IsOpened() )
        return;

    try {

        wxFileName file ( filename );
        if( FileExtManager::GetType(file.GetFullName()) == FileExtManager::TypeHeader ) {
            // This file is a header file, try locating the C++ file for it
            file.SetExt(wxT("cpp"));
        }

        wxString sql;
        sql = wxT("SELECT COMPILE_FLAGS,CWD FROM COMPILATION_TABLE WHERE FILE_NAME=?");
        wxSQLite3Statement st = m_db->PrepareStatement(sql);
        st.Bind(1, file.GetFullPath());
        wxSQLite3ResultSet rs = st.ExecuteQuery();

        if ( rs.NextRow() ) {
            compliationLine = rs.GetString(0);
            cwd             = rs.GetString(1);
            return;

        } else {
            // Could not find the cpp file for this file, try to locate *any* file from this directory
            sql = "SELECT COMPILE_FLAGS,CWD FROM COMPILATION_TABLE WHERE FILE_PATH=?";
            wxSQLite3Statement st2 = m_db->PrepareStatement(sql);
            st2.Bind(1, file.GetPath());
            wxSQLite3ResultSet rs2 = st2.ExecuteQuery();
            if ( rs2.NextRow() ) {
                compliationLine = rs2.GetString(0);
                cwd             = rs2.GetString(1);
                return;
            }
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
}

void CompilationDatabase::Close()
{
    if ( m_db ) {

        try {
            m_db->Close();
            delete m_db;

        } catch (wxSQLite3Exception &e) {
            wxUnusedVar(e);
        }
    }
    m_db = NULL;
}

void CompilationDatabase::Initialize()
{
    Open();
    if ( !IsOpened() )
        return;

    wxFileName cmakeCompilationDb  = GetCMakeDatabase();
    wxFileName clCustomCompileFile = GetFileName();
    clCustomCompileFile.SetExt("db.txt");
    
    if ( cmakeCompilationDb.Exists() && !clCustomCompileFile.Exists() ) {
        ProcessCMakeCompilationDatabase( cmakeCompilationDb );
        
    } else if ( cmakeCompilationDb.Exists() && clCustomCompileFile.Exists() && cmakeCompilationDb.GetModificationTime().GetTicks() > clCustomCompileFile.GetModificationTime().GetTicks() ) {
        // both cmake and our file exists, however, the cmake compilation database is newer - use it instead of ours
        ProcessCMakeCompilationDatabase( cmakeCompilationDb );

    } else if ( clCustomCompileFile.Exists() ) {
        // Our file is newer - use it instead
        ProcessCodeLiteCompilationDatabase( clCustomCompileFile );

    }
}

void CompilationDatabase::CreateDatabase()
{
    if ( !IsOpened() )
        return;

    try {

        if ( GetDbVersion() != DB_VERSION )
            DropTables();

        // Create the schema
        m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS COMPILATION_TABLE (FILE_NAME TEXT, FILE_PATH TEXT, CWD TEXT, COMPILE_FLAGS TEXT)");
        m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS SCHEMA_VERSION (PROPERTY TEXT, VERSION TEXT)");
        m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX1 ON COMPILATION_TABLE(FILE_NAME)");
        m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS SCHEMA_VERSION_IDX1 ON SCHEMA_VERSION(PROPERTY)");
        m_db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX2 ON COMPILATION_TABLE(FILE_PATH)");

        wxString versionSql;
        versionSql << "INSERT OR IGNORE INTO SCHEMA_VERSION (PROPERTY, VERSION) VALUES ('Db Version', '" << DB_VERSION << "')";
        m_db->ExecuteUpdate(versionSql);

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
}

void CompilationDatabase::DropTables()
{
    if ( !IsOpened() )
        return;

    try {

        // Create the schema
        m_db->ExecuteUpdate("DROP TABLE COMPILATION_TABLE");
        m_db->ExecuteUpdate("DROP TABLE SCHEMA_VERSION");

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
}

wxString CompilationDatabase::GetDbVersion()
{
    if ( !IsOpened() )
        return wxT("");

    try {

        // Create the schema
        wxString sql;
        sql = wxT("SELECT VERSION FROM SCHEMA_VERSION WHERE PROPERTY = 'Db Version' ");
        wxSQLite3Statement st = m_db->PrepareStatement(sql);
        wxSQLite3ResultSet rs = st.ExecuteQuery();

        if ( rs.NextRow() ) {
            wxString schemaVersion = rs.GetString(0);
            return schemaVersion;
        }


    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return wxT("");
}

bool CompilationDatabase::IsDbVersionUpToDate(const wxFileName& fn)
{
    try {
        wxString sql;
        wxSQLite3Database db;
        db.Open(fn.GetFullPath());
        sql = "SELECT VERSION FROM SCHEMA_VERSION WHERE PROPERTY = 'Db Version' ";
        wxSQLite3Statement st = db.PrepareStatement(sql);
        wxSQLite3ResultSet rs = st.ExecuteQuery();

        if ( rs.NextRow() ) {
            return rs.GetString(0) == DB_VERSION;
        }
        return false;

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return false;
}

bool CompilationDatabase::IsOk() const
{
    wxFileName fnDb = GetFileName();
    return fnDb.Exists() && IsDbVersionUpToDate( fnDb );
}

wxFileName CompilationDatabase::GetCMakeDatabase() const
{
    wxFileName fn( GetFileName() ); // Usually it will be under the top folder
    fn.RemoveLastDir();

    // Since we can have multiple "compile_commands.json" files, we take the most updated file
    wxDir dir;
    wxString dirname;
    wxFileName compileCommandsUpToDate;
    dir.Open( fn.GetPath() );
    
    // Prepare a list of files to check
    FileNameVector_t files;
    files.push_back( wxFileName(fn.GetPath(), "compile_commands.json" ) );
    bool cont = dir.GetFirst(&dirname, "", wxDIR_DIRS);
    while ( cont ) {
        wxFileName compile_commands( fn.GetPath(), "compile_commands.json" );
        compile_commands.AppendDir( dirname );
        files.push_back( compile_commands );
        cont = dir.GetNext( &dirname );
    }
    
    // Take the most up-to-date compile_commands.json file
    for(size_t i=0; i<files.size(); ++i) {
        const wxFileName &compile_commands = files.at(i);
        if ( !compileCommandsUpToDate.IsOk() && compile_commands.Exists() ) {
            compileCommandsUpToDate = compile_commands;

        } else if ( compile_commands.Exists() && compile_commands.GetModificationTime().GetTicks() > compileCommandsUpToDate.GetModificationTime().GetTicks() ) {
            compileCommandsUpToDate = compile_commands;
        }
    }
    return compileCommandsUpToDate;
}

void CompilationDatabase::ProcessCMakeCompilationDatabase(const wxFileName& compile_commands)
{
    JSONRoot root(compile_commands);
    JSONElement arr = root.toElement();

    try {

        wxString sql;
        sql = wxT("REPLACE INTO COMPILATION_TABLE (FILE_NAME, FILE_PATH, CWD, COMPILE_FLAGS) VALUES(?, ?, ?, ?)");
        wxSQLite3Statement st = m_db->PrepareStatement(sql);
        m_db->ExecuteUpdate("BEGIN");
        
        for( int i=0; i<arr.arraySize(); ++i ) {
            // Each object has 3 properties:
            // directory, command, file
            JSONElement element = arr.arrayItem(i);
            wxString cwd       = wxFileName(element.namedObject("directory").toString(), "").GetPath();
            wxString file      = wxFileName(element.namedObject("file").toString()).GetFullPath();
            wxString path      = wxFileName(file).GetPath();
            wxString cmp_flags = element.namedObject("command").toString();
            
            st.Bind(1, file);
            st.Bind(2, path);
            st.Bind(3, cwd);
            st.Bind(4, cmp_flags);
            st.ExecuteUpdate();
        }
        
        m_db->ExecuteUpdate("COMMIT");
        
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
}

void CompilationDatabase::ProcessCodeLiteCompilationDatabase(const wxFileName& compile_file)
{
    wxFFile fp(compile_file.GetFullPath(), wxT("rb"));
    if( fp.IsOpened() ) {
        wxString content;
        fp.ReadAll(&content, wxConvUTF8);

        if( content.IsEmpty() )
            return;


        wxArrayString lines = ::wxStringTokenize(content, "\n\r", wxTOKEN_STRTOK);
        try {

            wxString sql;
            sql = wxT("REPLACE INTO COMPILATION_TABLE (FILE_NAME, FILE_PATH, CWD, COMPILE_FLAGS) VALUES(?, ?, ?, ?)");
            wxSQLite3Statement st = m_db->PrepareStatement(sql);

            m_db->ExecuteUpdate("BEGIN");
            for(size_t i=0; i<lines.GetCount(); ++i) {
                wxArrayString parts = ::wxStringTokenize(lines.Item(i), wxT("|"), wxTOKEN_STRTOK);
                if( parts.GetCount() != 3 )
                    continue;

                wxString file_name = wxFileName(parts.Item(0).Trim().Trim(false)).GetFullPath();
                wxString path      = wxFileName(file_name).GetPath();
                wxString cwd       = parts.Item(1).Trim().Trim(false);
                wxString cmp_flags = parts.Item(2).Trim().Trim(false);;

                st.Bind(1, file_name);
                st.Bind(2, path);
                st.Bind(3, cwd);
                st.Bind(4, cmp_flags);

                st.ExecuteUpdate();
            }
            m_db->ExecuteUpdate("COMMIT");

        } catch (wxSQLite3Exception &e) {
            wxUnusedVar(e);
        }

        wxLogNull nl;
        fp.Close();
        ::wxRemoveFile( compile_file.GetFullPath() );

    }
}
