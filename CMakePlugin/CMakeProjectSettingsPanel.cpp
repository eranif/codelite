//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CMakeProjectSettingsPanel.cpp
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
#include "CMakeProjectSettingsPanel.h"

// wxWidgets
#include <wx/busyinfo.h>

// Codelite
#include "workspace.h"
#include "imanager.h"

// CMakePlugin
#include "CMake.h"
#include "CMakeSettingsManager.h"
#include "CMakePlugin.h"

/* ************************************************************************ */
/* FUNCTIONS                                                                */
/* ************************************************************************ */

/**
 * @brief Find workspace config name for project - config pair.
 *
 * @param configs
 * @param project
 * @param config
 *
 * @return
 */
static
wxString FindWorkspaceConfig(const std::list<WorkspaceConfigurationPtr>& configs,
    const wxString& project, const wxString& config)
{
    // Name of workspace config
    wxString workspaceConfig;

    // Foreach all workspace configurations
    for (std::list<WorkspaceConfigurationPtr>::const_iterator it = configs.begin(), ite = configs.end();
        it != ite; ++it) {
        const WorkspaceConfiguration::ConfigMappingList& mapping = (*it)->GetMapping();

        // Let's talk about how easy and readable C++98/03 is.
        for (WorkspaceConfiguration::ConfigMappingList::const_iterator it2 = mapping.begin(), ite2 = mapping.end();
            it2 != ite2; ++it2) {
            if (it2->m_project == project && it2->m_name == config) {
                // Config name found
                return (*it)->GetName();
            }
        }
    }

    // Not found WHAT??
    return "";
}

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakeProjectSettingsPanel::CMakeProjectSettingsPanel(wxWindow* parent,
                                                     CMakePlugin* plugin)
    : CMakeProjectSettingsPanelBase(parent, wxID_ANY)
    , m_plugin(plugin)
{
    // Set available generators
    m_choiceGenerator->Insert("", 0);
    m_choiceGenerator->Append(m_plugin->GetSupportedGenerators());

    // wxCrafter removes empty value
    m_comboBoxBuildType->Insert("", 0);

    // Set default setting
    ClearSettings();
}

/* ************************************************************************ */

void
CMakeProjectSettingsPanel::SetSettings(CMakeProjectSettings* settings, const wxString& project, const wxString& config)
{
    // Remove old projects
    m_choiceParent->Clear();

    // Get all available projects
    wxArrayString projects;
    m_plugin->GetManager()->GetWorkspace()->GetProjectList(projects);

    // Get build matrix
    // Required for translation between current config and other projects' configs.
    BuildMatrixPtr matrix = m_plugin->GetManager()->GetWorkspace()->GetBuildMatrix();

    // We have to find name of workspace configuration by project name and selected config
    // Name of workspace config
    const wxString workspaceConfig = FindWorkspaceConfig(
        matrix->GetConfigurations(), project, config
    );

    // Foreach projects
    for (wxArrayString::const_iterator it = projects.begin(),
        ite = projects.end(); it != ite; ++it) {
        // Translate project config
        const wxString projectConfig = matrix->GetProjectSelectedConf(workspaceConfig, *it);

        const CMakeSettingsManager* mgr = m_plugin->GetSettingsManager();
        wxASSERT(mgr);
        const CMakeProjectSettings* projectSettings = mgr->GetProjectSettings(*it, projectConfig);

        const bool append =
            projectSettings &&
            projectSettings->enabled &&
            projectSettings != settings &&
            projectSettings->parentProject.IsEmpty()
        ;

        // Add project if CMake is enabled for it
        if (append)
            m_choiceParent->Append(*it);
    }

    m_settings = settings;
    LoadSettings();
}

/* ************************************************************************ */

void
CMakeProjectSettingsPanel::LoadSettings()
{
    if (!m_settings) {
        ClearSettings();
    } else {
        SetCMakeEnabled(m_settings->enabled);
        SetSourceDirectory(m_settings->sourceDirectory);
        SetBuildDirectory(m_settings->buildDirectory);
        SetGenerator(m_settings->generator);
        SetBuildType(m_settings->buildType);
        SetArguments(m_settings->arguments);
        SetParentProject(m_settings->parentProject);
    }
}

/* ************************************************************************ */

void
CMakeProjectSettingsPanel::StoreSettings()
{
    if (!m_settings)
        return;

    m_settings->enabled = IsCMakeEnabled();
    m_settings->sourceDirectory = GetSourceDirectory();
    m_settings->buildDirectory = GetBuildDirectory();
    m_settings->generator = GetGenerator();
    m_settings->buildType = GetBuildType();
    m_settings->arguments = GetArguments();
    m_settings->parentProject = GetParentProject();
}

/* ************************************************************************ */

void
CMakeProjectSettingsPanel::ClearSettings()
{
    SetCMakeEnabled(false);
    SetSourceDirectory("");
    SetBuildDirectory("");
    SetGenerator("");
    SetArguments(wxArrayString());
    SetParentProject("");
}

/* ************************************************************************ */
