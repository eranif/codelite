//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeProjectSettings.h
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

#ifndef CMAKE_PROJECT_SETTINGS_HPP_
#define CMAKE_PROJECT_SETTINGS_HPP_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// C++
#include <map>

// wxWidgets
#include <wx/string.h>
#include <wx/arrstr.h>

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief CMake project setting.
 *
 * This structure stores all required data for project configuration
 * by CMake.
 *
 * @note Only "Unix Makefiles" generator is supported now.
 */
class CMakeProjectSettings
{
public:
    /// If CMake build is enabled.
    bool enabled;

    /// Source directory.
    /// Directory where the root CMakeLists.txt is located.
    wxString sourceDirectory;

    /// Build directory.
    /// Directory where sources are compiled and builded.
    wxString buildDirectory;

    /// CMake generator.
    wxString generator;

    /// CMake build type.
    wxString buildType;

    /// Configure arguments.
    wxArrayString arguments;


    /// Name of parent project.
    /// Parent project should have proper CMake configuration.
    wxString parentProject;


    /**
     * @brief Constructor.
     */
    CMakeProjectSettings()
        : enabled(false), sourceDirectory("$(ProjectPath)"), buildDirectory("build")
        , generator(), buildType(), arguments(), parentProject()
    {}

};

/* ************************************************************************ */
/* TYPES                                                                    */
/* ************************************************************************ */

/**
 * @brief CMake project settings map indexed by config name.
 */
typedef std::map<wxString, CMakeProjectSettings> CMakeProjectSettingsMap;

/* ************************************************************************ */

#endif // CMAKE_PROJECT_SETTINGS_HPP_
