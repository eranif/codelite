#ifndef COMPILATIONDATABASE_H
#define COMPILATIONDATABASE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/wxsqlite3.h>

class WXDLLIMPEXP_SDK CompilationDatabase
{
    wxSQLite3Database* m_db;
    
public:
    CompilationDatabase();
    virtual ~CompilationDatabase();
    
    void Open(const wxFileName& fn = wxFileName());
    void Close();
    
    bool IsOpened() const {
        return m_db && m_db->IsOpen();
    }
    
    wxFileName GetFileName() const;
    void CompilationLine(const wxString &filename, wxString &compliationLine, wxString &cwd);
    void Initialize();
};

#endif // COMPILATIONDATABASE_H
