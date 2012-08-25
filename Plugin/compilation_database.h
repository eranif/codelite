#ifndef COMPILATIONDATABASE_H
#define COMPILATIONDATABASE_H

#include "codelite_exports.h"
#include <wx/wxsqlite3.h>

class WXDLLIMPEXP_SDK CompilationDatabase
{
    wxSQLite3Database* m_db;
    
public:
    CompilationDatabase();
    virtual ~CompilationDatabase();
    
    void Open();
    void Close();
    
    bool IsOpened() const {
        return m_db && m_db->IsOpen();
    }
    
    wxFileName GetFileName() const;
    wxString CompilationLine(const wxString &filename);
};

#endif // COMPILATIONDATABASE_H
