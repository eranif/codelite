//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeSettingsManager.cpp
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
#include "CMakeSettingsManager.h"

// wxWidgets
#include <wx/arrstr.h>

// Codelite
#include "json_utils.h"
#include "workspace.h"

#include <assistant/common/json.hpp>

// CMakePlugin
#include "CMakePlugin.h"

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakeSettingsManager::CMakeSettingsManager(CMakePlugin* plugin)
    : m_plugin(plugin)
{
    // Nothing to do
}

/* ************************************************************************ */

CMakeProjectSettingsMap* CMakeSettingsManager::GetProjectSettings(const wxString& project, bool create)
{
    if(create) {
        return &(m_projectSettings[project]);
    } else {
        std::map<wxString, CMakeProjectSettingsMap>::iterator it = m_projectSettings.find(project);

        if(it == m_projectSettings.end())
            return NULL;

        return &(it->second);
    }
}

/* ************************************************************************ */

const CMakeProjectSettingsMap* CMakeSettingsManager::GetProjectSettings(const wxString& project) const
{
    std::map<wxString, CMakeProjectSettingsMap>::const_iterator it = m_projectSettings.find(project);

    if(it == m_projectSettings.end())
        return NULL;

    return &(it->second);
}

/* ************************************************************************ */

CMakeProjectSettings* CMakeSettingsManager::GetProjectSettings(const wxString& project, const wxString& config,
                                                               bool create)
{
    // Get project settings
    CMakeProjectSettingsMap* settings = GetProjectSettings(project, create);

    if(create) {
        // GetProjectSettings should create the new one
        wxASSERT(settings);

        // Find or create configuration
        return &(*settings)[config];
    } else {
        // Not found
        if(!settings)
            return NULL;

        // Find configuration
        CMakeProjectSettingsMap::iterator it = settings->find(config);

        // Not found
        if(it == settings->end())
            return NULL;

        return &(it->second);
    }
}

/* ************************************************************************ */

const CMakeProjectSettings* CMakeSettingsManager::GetProjectSettings(const wxString& project,
                                                                     const wxString& config) const
{
    // Get project settings
    const CMakeProjectSettingsMap* settings = GetProjectSettings(project);

    // Not found
    if(!settings)
        return NULL;

    // Find configuration
    CMakeProjectSettingsMap::const_iterator it = settings->find(config);

    // Not found
    if(it == settings->end())
        return NULL;

    return &(it->second);
}

/* ************************************************************************ */

void CMakeSettingsManager::SaveProjects()
{
    clCxxWorkspace* workspace = m_plugin->GetManager()->GetWorkspace();
    wxASSERT(workspace);

    wxArrayString projects;
    workspace->GetProjectList(projects);

    for (const auto& projectName : projects) {
        SaveProject(projectName);
    }
}

/* ************************************************************************ */

void CMakeSettingsManager::SaveProject(const wxString& name)
{
    clCxxWorkspace* workspace = m_plugin->GetManager()->GetWorkspace();
    wxASSERT(workspace);

    wxString err;
    ProjectPtr project = workspace->FindProjectByName(name, err);

    if(!project)
        return;

    // Find project settings
    std::map<wxString, CMakeProjectSettingsMap>::const_iterator itSettings = m_projectSettings.find(name);

    // Ehm...
    if(itSettings == m_projectSettings.end())
        return;

    // Create JSON object
    nlohmann::json json = nlohmann::json::array();

    // Foreach settings
    for (const auto& [name, settings] : itSettings->second) {
        json.push_back({{"name", name},
                        // Store settings
                        {"enabled", settings.enabled},
                        {"buildDirectory", settings.buildDirectory},
                        {"sourceDirectory", settings.sourceDirectory},
                        {"generator", settings.generator},
                        {"buildType", settings.buildType},
                        {"arguments", settings.arguments},
                        {"parentProject", settings.parentProject}});
    }

    // Must be an array
    wxASSERT(json.is_array());

    // Store plugin data
    project->SetPluginData("CMakePlugin", wxString::FromUTF8(json.dump()));
}

/* ************************************************************************ */

void CMakeSettingsManager::LoadProjects()
{
    clCxxWorkspace* workspace = m_plugin->GetManager()->GetWorkspace();
    wxASSERT(workspace);

    wxArrayString projects;
    workspace->GetProjectList(projects);

    for (const auto& projectName : projects) {
        LoadProject(projectName);
    }
}

/* ************************************************************************ */

void CMakeSettingsManager::LoadProject(const wxString& name)
{
    clCxxWorkspace* workspace = m_plugin->GetManager()->GetWorkspace();
    wxASSERT(workspace);

    wxString err;
    ProjectPtr project = workspace->FindProjectByName(name, err);

    if(!project)
        return;

    // Find for project or create new one
    CMakeProjectSettingsMap* projectSettings = GetProjectSettings(name, true);
    wxASSERT(projectSettings);

    // Alias for map
    CMakeProjectSettingsMap& settingsMap = *projectSettings;

    // Load JSON string
    const wxString jsonStr = project->GetPluginData("CMakePlugin");

    // Create JSON object
    nlohmann::json json = nlohmann::json::parse(StringUtils::ToStdString(jsonStr), nullptr, false);

    // Unable to parse
    if (json.is_discarded())
        return;

    // Expected array with config names
    if (!json.is_array())
        return;

    // Foreach array
    for (const auto& item : json) {
        // Name
        const wxString name = wxString::FromUTF8(item["name"]);

        // (Create and) get settings
        CMakeProjectSettings& settings = settingsMap[name];
        settings.enabled = item["enabled"];
        settings.buildDirectory = wxString::FromUTF8(item.value("buildDirectory", "build"));
        settings.sourceDirectory = wxString::FromUTF8(item.value("sourceDirectory", "build"));
        settings.generator = wxString::FromUTF8(item["generator"]);
        settings.buildType = wxString::FromUTF8(item["buildType"]);
        settings.arguments = JsonUtils::ToArrayString(item["arguments"]);
        settings.parentProject = wxString::FromUTF8(item["parentProject"]);
    }
}

/* ************************************************************************ */
