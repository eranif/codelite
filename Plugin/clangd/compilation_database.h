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
#include "project.h"

#include <wx/filename.h>
#include <wx/sharedptr.h>
#include <wx/string.h>
#include <wx/wxsqlite3.h>

class WXDLLIMPEXP_SDK CompilationDatabase
{
    wxSQLite3Database* m_db;
    wxFileName m_filename;

public:
    typedef wxSharedPtr<CompilationDatabase> Ptr_t;

protected:
    void DropTables();
    void CreateDatabase();
    wxString GetDbVersion();
    /**
     * @brief create our compilation database out of CMake's compile_commands.json file
     */
    void ProcessCMakeCompilationDatabase(const wxFileName& compile_commands);

    wxFileName ConvertCodeLiteCompilationDatabaseToCMake(const wxFileName& compile_file);

public:
    CompilationDatabase();
    CompilationDatabase(const wxString& filename);
    /**
     * @brief an "whole in one" method which attempts to find compile_commands.json file
     * create a .db out of it and return a CompilationDatabase object
     */
    static wxArrayString FindIncludePaths(const wxString& rootFolder, wxFileName& lastCompileCommands,
                                          time_t& lastCompileCommandsModified);

    virtual ~CompilationDatabase();

    static bool IsDbVersionUpToDate(const wxFileName& fn);
    void Open();
    void Close();

    bool IsOpened() const { return m_db && m_db->IsOpen(); }

    /**
     * @brief return the database file name (compilation.db), usually under the workspace private folder
     * WORKSPACE_PATH/.codelite/compilation.db
     */
    wxFileName GetFileName() const;
    /**
     * @brief return the location of the CMake (usually compile_commands.json)
     * Note that this function does not check for the existance of the file
     */
    FileNameVector_t GetCompileCommandsFiles() const;
    static FileNameVector_t GetCompileCommandsFiles(const wxString& rootFolder);
    void CompilationLine(const wxString& filename, wxString& compliationLine, wxString& cwd);
    void Initialize();
    bool IsOk() const;
};

#endif // COMPILATIONDATABASE_H
