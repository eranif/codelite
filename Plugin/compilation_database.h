//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : compilation_database.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef COMPILATIONDATABASE_H
#define COMPILATIONDATABASE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/wxsqlite3.h>

class WXDLLIMPEXP_SDK CompilationDatabase
{
    wxSQLite3Database* m_db;
    wxFileName         m_filename;
    
protected:
    void DropTables();
    void CreateDatabase();
    wxString GetDbVersion();
    
public:
    CompilationDatabase();
    CompilationDatabase(const wxString &filename);
    virtual ~CompilationDatabase();
    
    static bool IsDbVersionUpToDate(const wxFileName &fn);
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
