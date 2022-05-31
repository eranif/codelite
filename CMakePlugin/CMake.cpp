//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMake.cpp
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

/* ************************************************************************ */
/*                                                                          */
/* CMakePlugin for Codelite                                                 */
/* Copyright (C) 2013 Jiří Fatka <ntsfka@gmail.com>                         */
/*                                                                          */
/* This program is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program. If not, see <http://www.gnu.org/licenses/>.     */
/*                                                                          */
/* ************************************************************************ */

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// Declaration
#include "CMake.h"

#include "CMakeBuilder.h"
#include "buildmanager.h"
#include "cl_standard_paths.h"

// C++
#include <utility>

// wxWidgets
#include <wx/event.h>
#include <wx/regex.h>
#include <wx/scopedptr.h>
#include <wx/thread.h>

// Codelite
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"
#include "workspace.h"

/* ************************************************************************ */
/* FUNCTIONS                                                                */
/* ************************************************************************ */

/**
 * @brief Joins array of strings into single string.
 *
 * @param array Input array.
 *
 * @return Result string.
 */
static wxString CreateHtml(const wxArrayString& array)
{
    wxString result;

    for(wxArrayString::const_iterator it = array.begin(), ite = array.end(); it != ite; ++it) {
        if(it != array.begin()) {
            result += "<br />";
        }
        wxString esc = *it;
        // Escape
        esc.Replace("<", "&lt;");
        esc.Replace(">", "&gt;");
        result += esc;
    }

    return result;
}

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMake::CMake(const wxFileName& path)
    : m_path(path)
    , m_version("?")
    , m_dbFileName(clStandardPaths::Get().GetUserDataDir(), "cmake.db")
{
    // Prepare database
    PrepareDatabase();

    // Register the CMake builder
    BuildManagerST::Get()->AddBuilder(new CMakeBuilder());
}

/* ************************************************************************ */

wxArrayString CMake::GetVersions()
{
    static const wxString VERSIONS[] = { "2.8.11", "2.8.10", "2.8.9", "2.8.8", "2.8.7", "2.8.6", "2.8.5",
                                         "2.8.4",  "2.8.3",  "2.8.2", "2.8.1", "2.8.0", "2.6.4", "2.6.3",
                                         "2.6.2",  "2.6.1",  "2.6.0", "2.4.8", "2.4.7", "2.4.6", "2.4.5",
                                         "2.4.4",  "2.4.3",  "2.2.3", "2.0.6", "1.8.3" };

    return wxArrayString(sizeof(VERSIONS) / sizeof(VERSIONS[0]), VERSIONS);
}

/* ************************************************************************ */

bool CMake::IsOk() const
{
    wxArrayString output;
    ProcUtils::SafeExecuteCommand(GetPath().GetFullPath() + " -h", output);

    // SafeExecuteCommand doesn't return status code so the only way
    // to test the success is the output emptiness.
    return !output.empty();
}

/* ************************************************************************ */

bool CMake::LoadData(bool force, LoadNotifier* notifier)
{
    // Clear old data
    m_version.clear();
    m_commands.clear();
    m_modules.clear();
    m_properties.clear();
    m_variables.clear();

    if(notifier) {
        notifier->Start();
    }

    // Load data from database
    if(!force && m_dbInitialized && LoadFromDatabase()) {
        // Loading is done
        if(notifier) {
            notifier->Done();
        }
        return true;
    }

    // Unable to use CMake
    if(!IsOk())
        return false;

    // Request to stop
    if(notifier && notifier->RequestStop()) {
        return false;
    }

    // Get cmake program path
    const wxString program = GetPath().GetFullPath();

    // Version
    {
        wxArrayString output;
        ProcUtils::SafeExecuteCommand(program + " --version", output);

        // Unable to find version
        if(!output.IsEmpty()) {
            const wxString& versionLine = output[0];
            wxRegEx expression("cmake version (.+)");

            if(expression.IsValid() && expression.Matches(versionLine)) {
                m_version = expression.GetMatch(versionLine, 1).Trim().Trim(false);
            }
        }
    }

    // Request to stop
    if(notifier && notifier->RequestStop()) {
        return false;
    }

    // Load data
    if(!LoadFromCMake(notifier))
        return false;

    // Request to stop
    if(notifier && notifier->RequestStop()) {
        return false;
    }

    // Database is open so we can store result into database
    if(m_dbInitialized) {
        StoreIntoDatabase();
    }

    // Loading is done
    if(notifier) {
        notifier->Update(100);
        notifier->Done();
    }

    return true;
}

/* ************************************************************************ */

void CMake::PrepareDatabase()
{
    m_dbInitialized = false;

    try {

        /// Open database only for initializing
        wxSQLite3Database db;

        // Try to open database
        db.Open(GetDatabaseFileName().GetFullPath());

        // Not opened
        if(!db.IsOpen())
            return;

        // Create tables
        db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS commands (name TEXT, desc TEXT)");
        db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS modules (name TEXT, desc TEXT)");
        db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS properties (name TEXT, desc TEXT)");
        db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS variables (name TEXT, desc TEXT)");
        db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS strings (name TEXT, desc TEXT)");

        // Create indices
        db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS commands_idx ON commands(name)");
        db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS modules_idx ON modules(name)");
        db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS properties_idx ON properties(name)");
        db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS variables_idx ON variables(name)");
        db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS strings_idx ON strings(name)");

        // Everything is OK
        m_dbInitialized = true;

    } catch(const wxSQLite3Exception& e) {
        // Unable to use SQLite database
        clERROR() << "CMake DoPrepareDatabase error" << e.GetMessage() << endl;
    }
}

/* ************************************************************************ */

bool CMake::LoadFromCMake(LoadNotifier* notifier)
{
    // Possible types
    static const std::pair<wxString, HelpMap*> types[] = {
        std::make_pair("command", &m_commands), std::make_pair("module", &m_modules),
        std::make_pair("property", &m_properties), std::make_pair("variable", &m_variables)
        // make_pair("policy", &m_policies)
    };
    static const int typesCount = sizeof(types) / sizeof(types[0]);
    static const int PROGRESS = 90;
    static const int STEP = PROGRESS / typesCount;

    // Foreach all types
    for(int i = 0; i < typesCount; ++i) {
        // Notify??
        if(notifier) {
            // Stop request?
            if(notifier->RequestStop())
                return false;

            notifier->Update(STEP * i);
        }

        // Load
        if(!LoadList(types[i].first, *types[i].second, notifier, STEP))
            return false;
    }

    return true;
}

/* ************************************************************************ */

bool CMake::LoadFromDatabase()
{
    if(!m_dbInitialized) {
        return false;
    }

    try {
        /// Open database only for reading
        wxSQLite3Database db;

        // Open
        db.Open(GetDatabaseFileName().GetFullPath());

        // Not opened
        if(!db.IsOpen())
            return false;

        // Strings - Version
        {
            wxSQLite3ResultSet res = db.ExecuteQuery("SELECT desc FROM strings WHERE name = 'version'");
            if(res.NextRow()) {
                m_version = res.GetAsString(0);
            }
        }

        // No data stored
        if(m_version.IsEmpty())
            return false;

        // Commands
        {
            wxSQLite3ResultSet res = db.ExecuteQuery("SELECT name, desc FROM commands");
            while(res.NextRow()) {
                m_commands[res.GetAsString(0)] = res.GetAsString(1);
            }
        }

        // Modules
        {
            wxSQLite3ResultSet res = db.ExecuteQuery("SELECT name, desc FROM modules");
            while(res.NextRow()) {
                m_modules[res.GetAsString(0)] = res.GetAsString(1);
            }
        }

        // Properties
        {
            wxSQLite3ResultSet res = db.ExecuteQuery("SELECT name, desc FROM properties");
            while(res.NextRow()) {
                m_properties[res.GetAsString(0)] = res.GetAsString(1);
            }
        }

        // Variables
        {
            wxSQLite3ResultSet res = db.ExecuteQuery("SELECT name, desc FROM variables");
            while(res.NextRow()) {
                m_variables[res.GetAsString(0)] = res.GetAsString(1);
            }
        }

    } catch(const wxSQLite3Exception& e) {
        clERROR() << "Error occurred while loading data from CMake database" << e.GetMessage();
    }

    // Everything is loaded
    return true;
}

/* ************************************************************************ */

void CMake::StoreIntoDatabase()
{
    if(!m_dbInitialized) {
        clWARNING() << "CMake: can't store data into database. Database was not initialized properly" << endl;
        return;
    }

    try {
        /// Open database only for writing
        wxSQLite3Database db;

        // Open
        db.Open(GetDatabaseFileName().GetFullPath());

        // Not opened
        if(!db.IsOpen())
            return;

        db.Begin();

        // Commands
        {
            db.ExecuteUpdate("DELETE FROM commands");
            wxSQLite3Statement stmt = db.PrepareStatement("INSERT INTO commands (name, desc) VALUES(?, ?)");
            for(HelpMap::const_iterator it = m_commands.begin(), ite = m_commands.end(); it != ite; ++it) {
                stmt.Bind(1, it->first);
                stmt.Bind(2, it->second);
                stmt.ExecuteUpdate();
            }
        }

        // Modules
        {
            db.ExecuteUpdate("DELETE FROM modules");
            wxSQLite3Statement stmt = db.PrepareStatement("INSERT INTO modules (name, desc) VALUES(?, ?)");
            for(HelpMap::const_iterator it = m_modules.begin(), ite = m_modules.end(); it != ite; ++it) {
                stmt.Bind(1, it->first);
                stmt.Bind(2, it->second);
                stmt.ExecuteUpdate();
            }
        }

        // Properties
        {
            db.ExecuteUpdate("DELETE FROM properties");
            wxSQLite3Statement stmt = db.PrepareStatement("INSERT INTO properties (name, desc) VALUES(?, ?)");
            for(HelpMap::const_iterator it = m_properties.begin(), ite = m_properties.end(); it != ite; ++it) {
                stmt.Bind(1, it->first);
                stmt.Bind(2, it->second);
                stmt.ExecuteUpdate();
            }
        }

        // Variables
        {
            db.ExecuteUpdate("DELETE FROM variables");
            wxSQLite3Statement stmt = db.PrepareStatement("INSERT INTO variables (name, desc) VALUES(?, ?)");
            for(HelpMap::const_iterator it = m_variables.begin(), ite = m_variables.end(); it != ite; ++it) {
                stmt.Bind(1, it->first);
                stmt.Bind(2, it->second);
                stmt.ExecuteUpdate();
            }
        }

        // Strings - Version
        {
            wxSQLite3Statement stmt = db.PrepareStatement("REPLACE INTO strings (name, desc) VALUES('version', ?)");
            stmt.Bind(1, m_version);
            stmt.ExecuteUpdate();
        }

        db.Commit();

    } catch(wxSQLite3Exception& e) {
        clERROR() << "An error occurred while storing CMake data into database:" << e.GetMessage();
    }
}

/* ************************************************************************ */

bool CMake::LoadList(const wxString& type, CMake::HelpMap& list, LoadNotifier* notifier, int limit)
{
    const wxString command = GetPath().GetFullPath();

    // Get list
    wxArrayString names;
    const wxString cmdList = command + " --help-" + type + "-list";
    ProcUtils::SafeExecuteCommand(cmdList, names);

    // Remove version
    if(!names.IsEmpty())
        names.RemoveAt(0);

    const int notifyCount = (names.GetCount() / limit) + 1;
    int loaded = 0;

    // Foreach names
    for(wxArrayString::const_iterator it = names.begin(), ite = names.end(); it != ite; ++it) {

        if(notifier && notifier->RequestStop()) {
            // Someone called 'wxThread::Delete'
            return false;
        }

        // Trim name
        wxString name = *it;
        name.Trim().Trim(false);

        // Export help
        wxArrayString desc;
        const wxString cmdItem = command + " --help-" + type + " \"" + name + "\"";
        ProcUtils::SafeExecuteCommand(cmdItem, desc);

        // Skip empty results
        if(desc.IsEmpty())
            continue;

        // Remove first line (cmake version)
        if(desc.Item(0).Matches("*cmake version*"))
            desc.RemoveAt(0);

        // Store help page
        list[name] = CreateHtml(desc);

        // One more loaded
        loaded++;

        // Add 1%
        if(notifier && loaded == notifyCount) {
            notifier->Inc(1);
            loaded = 0;
        }
    }
    return true;
}

/* ************************************************************************ */
