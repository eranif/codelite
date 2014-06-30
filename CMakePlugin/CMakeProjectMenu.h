//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CMakeProjectMenu.h
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

#ifndef CMAKE_PROJECT_MENU_H_
#define CMAKE_PROJECT_MENU_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// wxWidgets
#include <wx/menu.h>

// CMakePlugin
#include "CMakePlugin.h"

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief Project menu.
 */
class CMakeProjectMenu : public wxMenu
{

// Public Enums
public:


    enum
    {
        ID_OPEN_CMAKELISTS = 2556,
        ID_EXPORT_CMAKELISTS,
        ID_MAKE_DIRTY
    };


// Public Ctors & Dtors
public:


    /**
     * @brief Create a project menu.
     *
     * @param plugin A pointer to CMake Plugin.
     */
    explicit CMakeProjectMenu(CMakePlugin* plugin);


    /**
     * @brief Destructor.
     */
    ~CMakeProjectMenu();


// Public Events
public:


    /**
     * @brief On CMakeLists.txt open request.
     *
     * @param event
     */
    void OnCMakeListsOpen(wxCommandEvent& event);


    /**
     * @brief On CMakeLists.txt export request.
     *
     * @param event
     */
    void OnExport(wxCommandEvent& event);


    /**
     * @brief On request to make current CMake output files dirty that
     * forces cmake call.
     *
     * @param event
     */
    void OnMakeDirty(wxCommandEvent& event);


    /**
     * @brief Enable open CMakeLists.txt only if exists
     *
     * @param
     */
    void OnFileExists(wxUpdateUIEvent& event);


    /**
     * @brief Enable make dirty when CMake is enabled for project.
     *
     * @param
     */
    void OnCMakeEnabled(wxUpdateUIEvent& event);


// Private Data Members
private:


    /// A pointer to plugin.
    CMakePlugin* const m_plugin;

};

/* ************************************************************************ */

#endif // CMAKE_PROJECT_MENU_H_
