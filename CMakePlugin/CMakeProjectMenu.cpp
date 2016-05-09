//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeProjectMenu.cpp
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
#include "CMakeProjectMenu.h"

// wxWidgets
#include <wx/app.h>

// Codelite
#include "dirsaver.h"

// CMakePlugin
#include "CMakeGenerator.h"
#include "CMakeProjectSettings.h"

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakeProjectMenu::CMakeProjectMenu(CMakePlugin* plugin)
    : wxMenu()
    , m_plugin(plugin)
{
    // Open file
    Append(new wxMenuItem(this, ID_OPEN_CMAKELISTS, _("Open CMakeLists.txt")));

    AppendSeparator();

    // Export
    Append(new wxMenuItem(this, ID_EXPORT_CMAKELISTS, _("Export CMakeLists.txt")));
    
    // Binding directly to the wxMenu doesn't work
    wxTheApp->Bind(wxEVT_MENU, &CMakeProjectMenu::OnCMakeListsOpen, this, ID_OPEN_CMAKELISTS);
    wxTheApp->Bind(wxEVT_MENU, &CMakeProjectMenu::OnExport, this, ID_EXPORT_CMAKELISTS);
    wxTheApp->Bind(wxEVT_UPDATE_UI, &CMakeProjectMenu::OnFileExists, this, ID_OPEN_CMAKELISTS);
}

/* ************************************************************************ */

CMakeProjectMenu::~CMakeProjectMenu()
{
    wxTheApp->Unbind(wxEVT_UPDATE_UI, &CMakeProjectMenu::OnFileExists, this, ID_OPEN_CMAKELISTS);
    wxTheApp->Unbind(wxEVT_MENU, &CMakeProjectMenu::OnExport, this, ID_EXPORT_CMAKELISTS);
    wxTheApp->Unbind(wxEVT_MENU, &CMakeProjectMenu::OnCMakeListsOpen, this, ID_OPEN_CMAKELISTS);
}

/* ************************************************************************ */

void CMakeProjectMenu::OnCMakeListsOpen(wxCommandEvent& event)
{
    wxUnusedVar(event);

    ProjectPtr project = m_plugin->GetSelectedProject();

    if(project) m_plugin->OpenCMakeLists(m_plugin->GetProjectDirectory(project->GetName()));
}

/* ************************************************************************ */

void CMakeProjectMenu::OnExport(wxCommandEvent& event)
{
    CMakeGenerator::Generate(m_plugin->GetSelectedProject(), true);
}

/* ************************************************************************ */

void CMakeProjectMenu::OnFileExists(wxUpdateUIEvent& event)
{
    ProjectPtr project = m_plugin->GetSelectedProject();

    if(project) event.Enable(m_plugin->ExistsCMakeLists(m_plugin->GetProjectDirectory(project->GetName())));
}
