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
#include "CMakePlugin.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "cmakehelpthread.h"
#include "imanager.h"

// wxWidgets
#include <wx/regex.h>
#include <wx/wxsqlite3.h>

// Codelite
#include "procutils.h"
#include "workspace.h"
#include "globals.h"

void CMake::DoPrepareDatabase()
{
    try {
        m_db.Open(wxFileName(clStandardPaths::Get().GetUserDataDir(), "cmake.db").GetFullPath());

        // Not opened
        if (!m_db.IsOpen()) {
            m_dbInitialized = false;
        }

        // Create tables
        m_db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS commands (name TEXT, desc TEXT)");
        m_db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS modules (name TEXT, desc TEXT)");
        m_db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS properties (name TEXT, desc TEXT)");
        m_db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS variables (name TEXT, desc TEXT)");
        m_db.ExecuteUpdate("CREATE TABLE IF NOT EXISTS strings (name TEXT, desc TEXT)");

        // Create indices
        m_db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS commands_idx ON commands(name)");
        m_db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS modules_idx ON modules(name)");
        m_db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS properties_idx ON properties(name)");
        m_db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS variables_idx ON variables(name)");
        m_db.ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS strings_idx ON strings(name)");

        // Everything is OK
        m_dbInitialized = true;

    } catch (const wxSQLite3Exception& e) {
        // Unable to use SQLite database
        CL_ERROR("CMake DoPrepareDatabase error: %s", e.GetMessage());
        m_dbInitialized = false;
    }
}

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMake::CMake(CMakePlugin* plugin, const wxFileName& path)
    : m_dirty(true)
    , m_path(path)
    , m_helpLoaded(false)
    , m_helpLoaderThread(NULL)
    , m_plugin(plugin)
{
    DoPrepareDatabase();
}

/* ************************************************************************ */

wxArrayString
CMake::GetVersions()
{
    static const wxString VERSIONS[] = {
        "2.8.11",
        "2.8.10",
        "2.8.9",
        "2.8.8",
        "2.8.7",
        "2.8.6",
        "2.8.5",
        "2.8.4",
        "2.8.3",
        "2.8.2",
        "2.8.1",
        "2.8.0",
        "2.6.4",
        "2.6.3",
        "2.6.2",
        "2.6.1",
        "2.6.0",
        "2.4.8",
        "2.4.7",
        "2.4.6",
        "2.4.5",
        "2.4.4",
        "2.4.3",
        "2.2.3",
        "2.0.6",
        "1.8.3"
    };

    return wxArrayString(sizeof(VERSIONS) / sizeof(VERSIONS[0]), VERSIONS);
}

/* ************************************************************************ */

bool
CMake::IsOk() const
{
    wxArrayString output;
    ProcUtils::SafeExecuteCommand(GetPath().GetFullPath() + " -h", output);

    // SafeExecuteCommand doesn't return status code so the only way
    // to test the success is the output emptiness.
    return !output.empty();
}

/* ************************************************************************ */

bool
CMake::LoadData(bool force)
{
    // Loading data again is not required
    if (!m_dirty && !force)
        return true;

    if ( m_helpLoaderThread ) {
        // if the helper thread is running, return immediately
        return false;
    }

    // Clear old data
    m_helpLoaded = false;
    m_version.clear();
    m_commands.clear();
    m_modules.clear();
    m_properties.clear();
    m_variables.clear();

    // Load data from database
    if (!force && m_dbInitialized && LoadFromDatabase()) {
        return true;
    }
    
    // Unable to use CMake
    if (!IsOk()) {
        return false;
    }

    // Get cmake program path
    const wxString program = GetPath().GetFullPath();

    // Version
    {
        wxArrayString output;
        ProcUtils::SafeExecuteCommand(program + " --version", output);

        // Unable to find version
        if (!output.IsEmpty()) {
            const wxString& versionLine = output[0];
            wxRegEx expression("cmake version ([0-9\\.]+)");

            if (expression.IsValid() && expression.Matches(versionLine)) {
                m_version = expression.GetMatch(versionLine, 1);
            }
        }
    }

    // Load data
    if ( !LoadFromCMake() ) {
        return false;
    }

    m_dirty = false;
    return true;
}

/* ************************************************************************ */

bool
CMake::LoadFromCMake()
{
    if ( m_helpLoaderThread ) {
        // help is still being loaded
        return false;
    }

    if ( m_helpLoaded ) {
        return true;
    }

    // Get cmake program path
    const wxString program = GetPath().GetFullPath();

    // Create new help load thread to load the info for us
    m_plugin->GetManager()->SetStatusMessage(_("Loading CMake help content..."));
    m_helpLoaderThread = new CMakeHelpThread(this, program);
    m_helpLoaderThread->Start();
    return false;
}

/* ************************************************************************ */

bool
CMake::LoadFromDatabase()
{
    // Database is closed
    if (!m_db.IsOpen())
        return false;

    try {
        // Strings - Version
        {
            wxSQLite3ResultSet res = m_db.ExecuteQuery("SELECT desc FROM strings WHERE name = 'version'");
            if (res.NextRow()) {
                m_version = res.GetAsString(0);
            }
        }

        // No data stored
        if (m_version.IsEmpty())
            return false;

        // Commands
        {
            wxSQLite3ResultSet res = m_db.ExecuteQuery("SELECT name, desc FROM commands");
            while (res.NextRow()) {
                m_commands[res.GetAsString(0)] = res.GetAsString(1);
            }
        }

        // Modules
        {
            wxSQLite3ResultSet res = m_db.ExecuteQuery("SELECT name, desc FROM modules");
            while (res.NextRow()) {
                m_modules[res.GetAsString(0)] = res.GetAsString(1);
            }
        }

        // Properties
        {
            wxSQLite3ResultSet res = m_db.ExecuteQuery("SELECT name, desc FROM properties");
            while (res.NextRow()) {
                m_properties[res.GetAsString(0)] = res.GetAsString(1);
            }
        }

        // Variables
        {
            wxSQLite3ResultSet res = m_db.ExecuteQuery("SELECT name, desc FROM variables");
            while (res.NextRow()) {
                m_variables[res.GetAsString(0)] = res.GetAsString(1);
            }
        }
    } catch (wxSQLite3Exception &e) {
        CL_ERROR("Error occured while loading data from CMake database: %s", e.GetMessage());
        return false;
    }

    // Data is OK
    m_dirty = false;

    // Everything is loaded
    return true;
}

/* ************************************************************************ */

void CMake::StoreIntoDatabase()
{
    if ( !m_dbInitialized ) {
        CL_WARNING("CMake: can't store data into database. Database was not initialized properly");
        return;
    }

    try {
        
        // Wrap everythign within a transcation (it will boost sqite performance by magnitude of X100
        m_db.Begin();
        
        // Commands
        {
            wxSQLite3Statement stmt = m_db.PrepareStatement("REPLACE INTO commands (name, desc) VALUES(?, ?)");
            for (HelpMap::const_iterator it = m_commands.begin(), ite = m_commands.end(); it != ite; ++it) {
                stmt.Bind(1, it->first);
                stmt.Bind(2, it->second);
                stmt.ExecuteUpdate();
            }
        }

        // Modules
        {
            wxSQLite3Statement stmt = m_db.PrepareStatement("REPLACE INTO modules (name, desc) VALUES(?, ?)");
            for (HelpMap::const_iterator it = m_modules.begin(), ite = m_modules.end(); it != ite; ++it) {
                stmt.Bind(1, it->first);
                stmt.Bind(2, it->second);
                stmt.ExecuteUpdate();
            }
        }

        // Properties
        {
            wxSQLite3Statement stmt = m_db.PrepareStatement("REPLACE INTO properties (name, desc) VALUES(?, ?)");
            for (HelpMap::const_iterator it = m_properties.begin(), ite = m_properties.end(); it != ite; ++it) {
                stmt.Bind(1, it->first);
                stmt.Bind(2, it->second);
                stmt.ExecuteUpdate();
            }
        }

        // Variables
        {
            wxSQLite3Statement stmt = m_db.PrepareStatement("REPLACE INTO variables (name, desc) VALUES(?, ?)");
            for (HelpMap::const_iterator it = m_variables.begin(), ite = m_variables.end(); it != ite; ++it) {
                stmt.Bind(1, it->first);
                stmt.Bind(2, it->second);
                stmt.ExecuteUpdate();
            }
        }

        // Strings - Version
        {
            wxSQLite3Statement stmt = m_db.PrepareStatement("REPLACE INTO strings (name, desc) VALUES('version', ?)");
            stmt.Bind(1, m_version);
            stmt.ExecuteUpdate();
        }
        m_db.Commit();
    } catch (wxSQLite3Exception &e) {
        CL_ERROR("An error occured while storing CMake data into database: %s", e.GetMessage());
    }
}

void CMake::OnCommandsHelpLoaded(CMake::HelpMap* helpMap)
{
    m_commands.clear();
    m_commands.swap( *helpMap );
    wxDELETE( helpMap );
}

void CMake::OnModulesHelpLoaded(CMake::HelpMap* helpMap)
{
    m_modules.clear();
    m_modules.swap( *helpMap );
    wxDELETE(helpMap);
}

void CMake::OnPropertiesHelpLoaded(CMake::HelpMap* helpMap)
{
    m_properties.clear();
    m_properties.swap( *helpMap );
    wxDELETE(helpMap);
}

void CMake::OnVariablesHelpLoaded(CMake::HelpMap* helpMap)
{
    m_variables.clear();
    m_variables.swap( *helpMap );
    wxDELETE(helpMap);
}

void CMake::OnHelpLoadCompleted()
{
    // Cleanup after our help thread
    m_helpLoaded = true;
    m_dirty = false;
    m_helpLoaderThread->Stop();
    wxDELETE(m_helpLoaderThread);

    // Save the info into the database
    StoreIntoDatabase();
    m_plugin->GetManager()->SetStatusMessage(_("CMake help content load is completed!"));
}

CMake::~CMake()
{
    if ( m_helpLoaderThread ) {
        m_helpLoaderThread->Stop();
        wxDELETE(m_helpLoaderThread);
    }
}

/* ************************************************************************ */
