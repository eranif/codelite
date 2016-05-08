//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMake.h
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

#ifndef CMAKE_HPP_
#define CMAKE_HPP_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// C++
#include <map>

// wxWidgets
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/arrstr.h>
#include <wx/vector.h>
#include <wx/progdlg.h>
#include <wx/wxsqlite3.h>

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief Class representing the cmake application.
 *
 * Class can be used for extracting help data from current installed
 * version of CMake like supported commands, modules, etc.
 */
class CMake
{

    // Public Structures
public:
    /**
     * @brief Helper class that helps notify about loading state.
     */
    class LoadNotifier
    {
    public:
        /**
         * @brief Checks if loading should be stopped.
         *
         * @return
         */
        virtual bool RequestStop() const = 0;

        /**
         * @brief Loading is started.
         */
        virtual void Start() = 0;

        /**
         * @brief Update loading progress.
         *
         * @param value Value is in range [0, 100].
         */
        virtual void Update(int value) = 0;

        /**
         * @brief Increase loading progress.
         *
         * @param value
         */
        virtual void Inc(int value) = 0;

        /**
         * @brief Loading is done.
         */
        virtual void Done() = 0;

        /**
         * @brief stop the current thread
         */
        virtual void Stop() = 0;
    };

    // Public Types
public:
    /// Lines map.
    typedef std::map<wxString, wxString> HelpMap;

    // Public Ctors
public:
    /**
     * @brief Constructor.
     *
     * @param path Path to cmake application.
     */
    explicit CMake(const wxFileName& path = wxFileName());

    // Public Accessors
public:
    /**
     * @brief Returns a list of CMake versions.
     *
     * The list is hard-written and must be manually updated.
     *
     * @return
     */
    static wxArrayString GetVersions();

    /**
     * @brief Returns path to the CMake application.
     *
     * Default value is 'cmake'.
     *
     * @return
     */
    const wxFileName& GetPath() const { return m_path; }

    /**
     * @brief Check if CMake path is OK.
     *
     * @return
     */
    bool IsOk() const;

    /**
     * @brief Returns CMake version.
     *
     * @return
     */
    const wxString& GetVersion() const { return m_version; }

    /**
     * @brief Returns a list of available commands.
     *
     * @return
     */
    const HelpMap& GetCommands() const { return m_commands; }

    /**
     * @brief Returns a list of available modules.
     *
     * @return
     */
    const HelpMap& GetModules() const { return m_modules; }

    /**
     * @brief Returns a list of available properties.
     *
     * @return
     */
    const HelpMap& GetProperties() const { return m_properties; }

    /**
     * @brief Returns a list of available variables.
     *
     * @return
     */
    const HelpMap& GetVariables() const { return m_variables; }

    /**
     * @brief Returns path of database file.
     *
     * @return
     */
    const wxFileName& GetDatabaseFileName() const { return m_dbFileName; }

    // Public Mutators
public:
    /**
     * @brief Changes cmake application path.
     *
     * @param path
     */
    void SetPath(const wxFileName& path) { m_path = path; }

    // Public Operations
public:
    /**
     * @brief Loads data from CMake application.
     *
     * @param force    If data should be loaded from cmake instead of
     *                 SQLite database.
     * @param notifier Optional progress notifier.
     *
     * @return If data was loaded.
     */
    bool LoadData(bool force = false, LoadNotifier* notifier = NULL);

    // Private Operations
private:
    /**
     * @brief Prepare database for CMake.
     */
    void PrepareDatabase();

    /**
     * @brief Reads everything from CMake.
     *
     * @param handler Optional progress notifier.
     *
     * @return If data was loaded.
     */
    bool LoadFromCMake(LoadNotifier* notifier = NULL);

    /**
     * @brief Loads data from SQLite3 database.
     *
     * @return If data is loaded.
     */
    bool LoadFromDatabase();

    /**
     * @brief Stores data into SQLite3 database.
     */
    void StoreIntoDatabase();

    /**
     * @brief Loads help of type from command into list.
     *
     * @param type    Help type.
     * @param list    Output variable.
     * @param handler Progress notifier.
     * @param limit   Notifier limit.
     * @return false incase 'RequestStop' was called during the loading of the help
     *         true otherwise
     */
    bool LoadList(const wxString& type, CMake::HelpMap& list, LoadNotifier* notifier, int limit);

    // Private Data Members
private:
    /// CMake application path.
    wxFileName m_path;

    /// Cached CMake version.
    wxString m_version;

    /// List of commands.
    HelpMap m_commands;

    /// List of modules.
    HelpMap m_modules;

    /// List of properties.
    HelpMap m_properties;

    /// List of variables.
    HelpMap m_variables;

    /// Path of database file.
    wxFileName m_dbFileName;

    /// Was the database initialized properly?
    bool m_dbInitialized;
};

/* ************************************************************************ */

#endif // CMAKE_BUILDER_HPP_
