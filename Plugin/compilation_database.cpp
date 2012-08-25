#include "compilation_database.h"
#include <wx/filename.h>
#include "workspace.h"

CompilationDatabase::CompilationDatabase()
    : m_db(NULL)
{
}

CompilationDatabase::~CompilationDatabase()
{
    Close();
}

void CompilationDatabase::Open()
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
        
        // Create the schema
        m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS COMPILATION_TABLE (FILE_NAME TEXT, COMPILE_FLAGS TEXT)");
        m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX1 ON COMPILATION_TABLE(FILE_NAME)");
        
    } catch (wxSQLite3Exception &e) {
        
        delete m_db;
        m_db = NULL;
        
    }
}

wxFileName CompilationDatabase::GetFileName() const
{
    wxFileName dbfile = WorkspaceST::Get()->GetWorkspaceFileName();
    dbfile.SetFullName(wxT("compilation.db"));
    return dbfile;
}

wxString CompilationDatabase::CompilationLine(const wxString& filename)
{
    if ( !IsOpened() )
        return wxT("");
        
    try {
        
        wxString sql;
        sql = wxT("SELECT COMPILE_FLAGS FROM COMPILATION_TABLE WHERE FILE_NAME=?");
        wxSQLite3Statement st = m_db->PrepareStatement(sql);
        st.Bind(1, filename);
        wxSQLite3ResultSet rs = st.ExecuteQuery();
        
        if ( rs.NextRow() ) {
            return rs.GetString(0);
        }
        
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return wxT("");
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

