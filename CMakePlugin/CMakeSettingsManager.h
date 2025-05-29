//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeSettingsManager.h
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

#ifndef CMAKE_SETTINGS_MANAGER_H_
#define CMAKE_SETTINGS_MANAGER_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// C++
#include <map>

// wxWidgets
#include <wx/string.h>
#include <wx/scopedptr.h>

// CMakePlugin
#include "CMakeProjectSettings.h"

/* ************************************************************************ */
/* FORWARD DECLARATIONS                                                     */
/* ************************************************************************ */

class CMakePlugin;

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief Manager for CMake projects settings.
 *
 * Configuration is stored in JSON format:
 * [
 *   { name: "Debug", enabled: true, ...}
 *   { name: "Release", enabled: true, ...}
 * ]
 */
class CMakeSettingsManager
{

// Public Ctors
public:


    /**
     * @brief Create settings manager.
     *
     * @param plugin
     */
    explicit CMakeSettingsManager(CMakePlugin* plugin);


// Public Accessors
public:


    /**
     * @brief Returns all project configurations.
     *
     * @param project Project name.
     * @param create  If not exists, create one.
     *
     * @return A pointer to project settings for specific config or
     * nullptr if config doesn't exists.
     */
    CMakeProjectSettingsMap* GetProjectSettings(const wxString& project,
                                                bool create = false);


    /**
     * @brief Returns all project configurations.
     *
     * @param project Project name.
     *
     * @return A pointer to project settings for specific config or
     * nullptr if config doesn't exists.
     */
    const CMakeProjectSettingsMap* GetProjectSettings(const wxString& project) const;


    /**
     * @brief Return project configuration for given config.
     *
     * @param project Project name.
     * @param config  Configuration name.
     * @param create  If not exists, create one.
     *
     * @return A pointer to project settings for specific config or
     * nullptr if config doesn't exists.
     */
    CMakeProjectSettings* GetProjectSettings(const wxString& project,
                                             const wxString& config,
                                             bool create = false);


    /**
     * @brief Return project configuration for given config.
     *
     * @param project Project name.
     * @param config  Configuration name.
     *
     * @return A pointer to project settings for specific config or
     * nullptr if config doesn't exists.
     */
    const CMakeProjectSettings* GetProjectSettings(const wxString& project,
                                                   const wxString& config) const;

// Public Operations
public:


    /**
     * @brief Save all settings.
     */
    void Save() { SaveProjects(); }


    /**
     * @brief Save all projects settings.
     */
    void SaveProjects();


    /**
     * @brief Save project settings.
     *
     * @param name Project name.
     */
    void SaveProject(const wxString& name);


    /**
     * @brief Load all settings.
     */
    void Load() { LoadProjects(); }


    /**
     * @brief Load all projects settings.
     */
    void LoadProjects();


    /**
     * @brief Load project settings.
     *
     * @param name Project name.
     */
    void LoadProject(const wxString& name);


// Private Data Members
private:


    /// CMake plugin pointer.
    CMakePlugin* const m_plugin;

    /// Settings for all projects.
    std::map<wxString, CMakeProjectSettingsMap> m_projectSettings;

};

/* ************************************************************************ */

#endif // CMAKE_SETTINGS_MANAGER_H_
