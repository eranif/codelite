//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : compilation_database.cpp
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

#include "compilation_database.h"

#include "JSON.h"
#include "cl_standard_paths.h"
#include "compiler_command_line_parser.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "project.h"
#include "workspace.h"

#include <algorithm>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/tokenzr.h>

const wxString DB_VERSION = "2.0";

struct wxFileNameSorter {
    bool operator()(const wxFileName& one, const wxFileName& two) const
    {
        return one.GetModificationTime().GetTicks() > two.GetModificationTime().GetTicks();
    }
};

CompilationDatabase::CompilationDatabase()
    : m_db(NULL)
{
}

CompilationDatabase::CompilationDatabase(const wxString& filename)
    : m_db(NULL)
    , m_filename(filename)
{
}

CompilationDatabase::~CompilationDatabase() { Close(); }

void CompilationDatabase::Open()
{
    // Close the old database
    if(m_db) {
        Close();
    }

    // Create new one
    try {

        m_db = new wxSQLite3Database();
        wxFileName dbfile(clCxxWorkspaceST::Get()->GetPrivateFolder(), "compilation.db");
        m_db->Open(dbfile.GetFullPath());
        CreateDatabase();

    } catch(wxSQLite3Exception& e) {

        delete m_db;
        m_db = NULL;
    }
}

wxFileName CompilationDatabase::GetFileName() const
{
    wxFileName dbfile;
    if(!m_filename.IsOk()) {
        dbfile = wxFileName(clCxxWorkspaceST::Get()->GetPrivateFolder(), "compilation.db");

    } else {
        dbfile = m_filename;
    }
    return dbfile;
}

void CompilationDatabase::CompilationLine(const wxString& filename, wxString& compliationLine, wxString& cwd)
{
    if(!IsOpened())
        return;

    try {

        wxFileName file(filename);
        if(FileExtManager::GetType(file.GetFullName()) == FileExtManager::TypeHeader) {
            // This file is a header file, try locating the C++ file for it
            file.SetExt(wxT("cpp"));
        }

        wxString sql;
        sql = wxT("SELECT COMPILE_FLAGS,CWD FROM COMPILATION_TABLE WHERE FILE_NAME=?");
        wxSQLite3Statement st = m_db->PrepareStatement(sql);
        st.Bind(1, file.GetFullPath());
        wxSQLite3ResultSet rs = st.ExecuteQuery();

        if(rs.NextRow()) {
            compliationLine = rs.GetString(0);
            cwd = rs.GetString(1);
            return;

        } else {
            // Could not find the cpp file for this file, try to locate *any* file from this directory
            sql = "SELECT COMPILE_FLAGS,CWD FROM COMPILATION_TABLE WHERE FILE_PATH=?";
            wxSQLite3Statement st2 = m_db->PrepareStatement(sql);
            st2.Bind(1, file.GetPath());
            wxSQLite3ResultSet rs2 = st2.ExecuteQuery();
            if(rs2.NextRow()) {
                compliationLine = rs2.GetString(0);
                cwd = rs2.GetString(1);
                return;
            }
        }

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void CompilationDatabase::Close()
{
    if(m_db) {

        try {
            m_db->Close();
            delete m_db;

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
    }
    m_db = NULL;
}

void CompilationDatabase::Initialize()
{
    Open();
    if(!IsOpened())
        return;

    // get list of files created by cmake
    FileNameVector_t files = GetCompileCommandsFiles();

    // pick codelite's compilation database created by codelite-cc
    // - convert it to compile_commands.json
    // - append it the list of files
    wxFileName clCustomCompileFile = GetFileName();
    clCustomCompileFile.SetExt("db.txt");
    if(clCustomCompileFile.Exists()) {
        wxFileName compile_commands = ConvertCodeLiteCompilationDatabaseToCMake(clCustomCompileFile);
        if(compile_commands.IsOk()) {
            files.push_back(compile_commands);
        }
    }
    // Sort the files by modification time
    std::sort(files.begin(), files.end(), wxFileNameSorter());

    for(size_t i = 0; i < files.size(); ++i) {
        ProcessCMakeCompilationDatabase(files.at(i));
    }
}

void CompilationDatabase::CreateDatabase()
{
    if(!IsOpened())
        return;

    try {
        if(GetDbVersion() != DB_VERSION) {
            DropTables();
        }

        // Create the schema
        m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS COMPILATION_TABLE (FILE_NAME TEXT, FILE_PATH TEXT, CWD TEXT, "
                            "COMPILE_FLAGS TEXT)");
        m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS SCHEMA_VERSION (PROPERTY TEXT, VERSION TEXT)");
        m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX1 ON COMPILATION_TABLE(FILE_NAME)");
        m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS SCHEMA_VERSION_IDX1 ON SCHEMA_VERSION(PROPERTY)");
        m_db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX2 ON COMPILATION_TABLE(FILE_PATH)");
        m_db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX3 ON COMPILATION_TABLE(CWD)");

        wxString versionSql;
        versionSql << "INSERT OR IGNORE INTO SCHEMA_VERSION (PROPERTY, VERSION) VALUES ('Db Version', '" << DB_VERSION
                   << "')";
        m_db->ExecuteUpdate(versionSql);

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

void CompilationDatabase::DropTables()
{
    if(!IsOpened())
        return;

    try {

        // Create the schema
        m_db->ExecuteUpdate("DROP TABLE COMPILATION_TABLE");
        m_db->ExecuteUpdate("DROP TABLE SCHEMA_VERSION");

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

wxString CompilationDatabase::GetDbVersion()
{
    if(!IsOpened())
        return wxT("");

    try {

        // Create the schema
        wxString sql;
        sql = wxT("SELECT VERSION FROM SCHEMA_VERSION WHERE PROPERTY = 'Db Version' ");
        wxSQLite3Statement st = m_db->PrepareStatement(sql);
        wxSQLite3ResultSet rs = st.ExecuteQuery();

        if(rs.NextRow()) {
            wxString schemaVersion = rs.GetString(0);
            return schemaVersion;
        }

    } catch(wxSQLite3Exception& e) {
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

        if(rs.NextRow()) {
            return rs.GetString(0) == DB_VERSION;
        }
        return false;

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return false;
}

bool CompilationDatabase::IsOk() const
{
    wxFileName fnDb = GetFileName();
    return fnDb.Exists() && IsDbVersionUpToDate(fnDb);
}

FileNameVector_t CompilationDatabase::GetCompileCommandsFiles() const
{
    wxFileName databaseFile(GetFileName());
    wxFileName fn(databaseFile);

    // Usually it will be under the top folder
    fn.RemoveLastDir();

    // Since we can have multiple "compile_commands.json" files, we take the most updated file
    // Prepare a list of files to check
    FileNameVector_t files;
    std::queue<std::pair<wxString, int>> dirs;

    // we start with the current path
    dirs.push(std::make_pair(fn.GetPath(), 0));

    const int MAX_DEPTH = 2; // If no files were found, scan 2 levels only

    while(!dirs.empty()) {
        std::pair<wxString, int> curdir = dirs.front();
        dirs.pop();
        if(files.empty() && (curdir.second > MAX_DEPTH)) {
            clDEBUG() << "Could not find compile_commands.json files while reaching depth 2, aborting" << endl;
            break;
        }

        wxFileName fn(curdir.first, "compile_commands.json");
        if(fn.Exists()) {
            clDEBUG() << "CompilationDatabase: found file: " << fn.GetFullPath() << endl;
            files.push_back(fn);
        }

        // Check to see if there are more directories to recurse
        wxDir dir;
        if(dir.Open(curdir.first)) {
            wxString dirname;
            bool cont = dir.GetFirst(&dirname, "", wxDIR_DIRS);
            while(cont) {
                wxString new_dir;
                new_dir << curdir.first << wxFileName::GetPathSeparator() << dirname;
                dirs.push(std::make_pair(new_dir, curdir.second + 1));
                dirname.Clear();
                cont = dir.GetNext(&dirname);
            }
        }
    }
    return files;
}

void CompilationDatabase::ProcessCMakeCompilationDatabase(const wxFileName& compile_commands)
{
    JSON root(compile_commands);
    JSONItem arr = root.toElement();

    try {

        wxString sql;
        sql = wxT("REPLACE INTO COMPILATION_TABLE (FILE_NAME, FILE_PATH, CWD, COMPILE_FLAGS) VALUES(?, ?, ?, ?)");
        wxSQLite3Statement st = m_db->PrepareStatement(sql);
        m_db->ExecuteUpdate("BEGIN");

        for(int i = 0; i < arr.arraySize(); ++i) {
            // Each object has 3 properties:
            // directory, command, file
            JSONItem element = arr.arrayItem(i);
            if(element.hasNamedObject("file") && element.hasNamedObject("directory") &&
               element.hasNamedObject("command")) {
                wxString cmd = element.namedObject("command").toString();
                wxString file = element.namedObject("file").toString();
                wxString path = wxFileName(file).GetPath();
                wxString cwd = element.namedObject("directory").toString();

                cwd = wxFileName(cwd, "").GetPath();
                file = wxFileName(file).GetFullPath();

                st.Bind(1, file);
                st.Bind(2, path);
                st.Bind(3, cwd);
                st.Bind(4, cmd);
                st.ExecuteUpdate();
            }
        }

        m_db->ExecuteUpdate("COMMIT");

    } catch(wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

wxFileName CompilationDatabase::ConvertCodeLiteCompilationDatabaseToCMake(const wxFileName& compile_file)
{
    wxFFile fp(compile_file.GetFullPath(), wxT("rb"));
    if(fp.IsOpened()) {
        wxString content;
        fp.ReadAll(&content, wxConvUTF8);

        if(content.IsEmpty())
            return wxFileName();

        JSON root(cJSON_Array);
        JSONItem arr = root.toElement();
        wxArrayString lines = ::wxStringTokenize(content, "\n\r", wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.GetCount(); ++i) {
            wxArrayString parts = ::wxStringTokenize(lines.Item(i), wxT("|"), wxTOKEN_STRTOK);
            if(parts.GetCount() != 3)
                continue;

            wxString file_name = wxFileName(parts.Item(0).Trim().Trim(false)).GetFullPath();
            wxString cwd = parts.Item(1).Trim().Trim(false);
            wxString cmp_flags = parts.Item(2).Trim().Trim(false);

            JSONItem element = JSONItem::createObject();
            element.addProperty("directory", cwd);
            element.addProperty("command", cmp_flags);
            element.addProperty("file", file_name);
            arr.arrayAppend(element);
        }

        wxFileName fn(compile_file.GetPath(), "compile_commands.json");
        root.save(fn);
        // Delete the old file
        {
            wxLogNull nl;
            fp.Close();
            if(compile_file.Exists()) {
                clRemoveFile(compile_file.GetFullPath());
            }
        }
        return fn;
    }
    return wxFileName();
}

wxArrayString CompilationDatabase::FindIncludePaths(const wxString& rootFolder, wxFileName& lastCompileCommands,
                                                    time_t& lastCompileCommandsModified)
{
    FileNameVector_t files = GetCompileCommandsFiles(rootFolder);
    if(files.empty()) {
        return wxArrayString();
    }
    const wxFileName& compile_commands = files[0]; // we take the first file, which is the most up to date

    // If the last compile_commands.json file was already processed, return an empty array
    if((lastCompileCommands == compile_commands) &&
       (compile_commands.GetModificationTime().GetTicks() == lastCompileCommandsModified)) {
        clDEBUG() << "File" << compile_commands << "already processed. Nothing more to be done here";
        return wxArrayString();
    }

    lastCompileCommands = compile_commands;
    lastCompileCommandsModified = compile_commands.GetModificationTime().GetTicks();

    wxStringSet_t paths;
    JSON root(compile_commands);
    JSONItem arr = root.toElement();
    const int file_size = arr.arraySize();
    for(int i = 0; i < file_size; ++i) {
        // Each object has 3 properties:
        // directory, command, file
        JSONItem element = arr.arrayItem(i);
        if(element.hasNamedObject("file") && element.hasNamedObject("directory") && element.hasNamedObject("command")) {
            wxString cmd = element.namedObject("command").toString();
            wxString cwd = element.namedObject("directory").toString();
            CompilerCommandLineParser cclp(cmd, cwd);
            const wxArrayString& includes = cclp.GetIncludes();
            std::for_each(includes.begin(), includes.end(),
                          [&](const wxString& includePath) { paths.insert(includePath); });
        }
    }
    // Convert the set back to array
    wxArrayString includePaths;
    std::for_each(paths.begin(), paths.end(), [&](const wxString& path) { includePaths.Add(path); });
    return includePaths;
}

FileNameVector_t CompilationDatabase::GetCompileCommandsFiles(const wxString& rootFolder)
{
    // Since we can have multiple "compile_commands.json" files, we take the most updated file
    // Prepare a list of files to check
    FileNameVector_t files;
    std::queue<std::pair<wxString, int>> dirs;

    // we start with the current path
    dirs.push(std::make_pair(rootFolder, 0));

    const int MAX_DEPTH = 2; // If no files were found, scan 2 levels only

    while(!dirs.empty()) {
        std::pair<wxString, int> curdir = dirs.front();
        dirs.pop();
        if(files.empty() && (curdir.second > MAX_DEPTH)) {
            clDEBUG() << "Could not find compile_commands.json files while reaching depth 2, aborting";
            break;
        }

        wxFileName fn(curdir.first, "compile_commands.json");
        if(fn.FileExists()) {
            clDEBUG() << "CompilationDatabase: found file: " << fn.GetFullPath();
            files.push_back(fn);
        }

        // Check to see if there are more directories to recurse
        wxDir dir;
        if(dir.Open(curdir.first)) {
            wxString dirname;
            bool cont = dir.GetFirst(&dirname, "", wxDIR_DIRS);
            while(cont) {
                wxString new_dir;
                new_dir << curdir.first << wxFileName::GetPathSeparator() << dirname;
                dirs.push(std::make_pair(new_dir, curdir.second + 1));
                dirname.Clear();
                cont = dir.GetNext(&dirname);
            }
        }
    }
    std::sort(files.begin(), files.end(), [](const wxFileName& one, const wxFileName& two) {
        return one.GetModificationTime() > two.GetModificationTime();
    });
    return files;
}
