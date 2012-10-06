#include "compilation_database.h"
#include <wx/filename.h>
#include "workspace.h"
#include <wx/tokenzr.h>
#include <wx/log.h>
#include <wx/ffile.h>
#include "fileextmanager.h"

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
        wxFileName dbfile = WorkspaceST::Get()->GetWorkspaceFileName();
        dbfile.SetFullName(wxT("compilation.db"));
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
        dbfile = WorkspaceST::Get()->GetWorkspaceFileName();
        dbfile.SetFullName(wxT("compilation.db"));
        
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

    wxString textfile = GetFileName().GetFullPath();
    textfile << wxT(".txt");
    wxFFile fp(textfile, wxT("rb"));
    if( fp.IsOpened() ) {
        wxString content;
        fp.ReadAll(&content, wxConvUTF8);

        if( content.IsEmpty() )
            return;


        wxArrayString lines = ::wxStringTokenize(content, wxT("\n\r"), wxTOKEN_STRTOK);
        try {

            wxString sql;
            sql = wxT("REPLACE INTO COMPILATION_TABLE (FILE_NAME, CWD, COMPILE_FLAGS) VALUES(?, ?, ?)");
            wxSQLite3Statement st = m_db->PrepareStatement(sql);

            m_db->ExecuteUpdate("BEGIN");
            for(size_t i=0; i<lines.GetCount(); ++i) {
                wxArrayString parts = ::wxStringTokenize(lines.Item(i), wxT("|"), wxTOKEN_STRTOK);
                if( parts.GetCount() != 3 )
                    continue;

                wxString file_name = parts.Item(0).Trim().Trim(false);
                wxString cwd       = parts.Item(1).Trim().Trim(false);
                wxString cmp_flags = parts.Item(2).Trim().Trim(false);;

                st.Bind(1, file_name);
                st.Bind(2, cwd);
                st.Bind(3, cmp_flags);

                st.ExecuteUpdate();
            }
            m_db->ExecuteUpdate("COMMIT");

        } catch (wxSQLite3Exception &e) {
            wxUnusedVar(e);
        }


        wxLogNull nl;
        fp.Close();
        ::wxRemoveFile(textfile);

    }
}

void CompilationDatabase::CreateDatabase()
{
    if ( !IsOpened() )
        return;

    try {

        if ( GetDbVersion() != wxT("1.0") )
            DropTables();

        // Create the schema
        m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS COMPILATION_TABLE (FILE_NAME TEXT, CWD TEXT, COMPILE_FLAGS TEXT)");
        m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS SCHEMA_VERSION (PROPERTY TEXT, VERSION TEXT)");
        m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX1 ON COMPILATION_TABLE(FILE_NAME)");
        m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS SCHEMA_VERSION_IDX1 ON SCHEMA_VERSION(VERSION)");
        m_db->ExecuteUpdate("INSERT OR IGNORE INTO SCHEMA_VERSION (PROPERTY, VERSION) VALUES ('Db Version', '1.0')");

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
