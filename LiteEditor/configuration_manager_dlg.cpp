//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : configuration_manager_dlg.cpp
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
#include "windowattrmanager.h"
#include "configuration_manager_dlg.h"
#include "manager.h"
#include "new_configuration_dlg.h"
#include "edit_configuration.h"
#include "edit_workspace_conf_dlg.h"
#include "macros.h"
#include <algorithm>
#include <wx/wupdlock.h>

//----------------------------------------------------------------------------
// Configuration Manager
//----------------------------------------------------------------------------

static int wxStringCmpFunc(const wxString& item1, const wxString& item2) { return item1.CmpNoCase(item2); }

ConfigurationManagerDlg::ConfigurationManagerDlg(wxWindow* parent)
    : ConfigManagerBaseDlg(parent)
    , m_dirty(false)
{
    PopulateConfigurations();
    InitDialog();
    CentreOnParent();
    SetName("ConfigurationManagerDlg");
    WindowAttrManager::Load(this);
}

ConfigEntry ConfigurationManagerDlg::DoCreateChoicesForProject(
    const wxString& projectName, const wxString& selectedConf, wxPGProperty* prop)
{
    wxPGChoices choices;

    // Get all configuration of the project
    int defaultValue = 0;
    int counter = 0;
    ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(projectName);
    if(settings) {
        ProjectSettingsCookie cookie;
        BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
        while(bldConf) {
            choices.Add(bldConf->GetName());
            if(bldConf->GetName() == selectedConf) {
                defaultValue = counter;
            }
            bldConf = settings->GetNextBuildConfiguration(cookie);
            ++counter;
        }
    }

    choices.Add(wxGetTranslation(clCMD_NEW));
    choices.Add(wxGetTranslation(clCMD_EDIT));
    wxPGProperty* p = prop;
    if(!p) {
        p = m_pgMgr->Append(new wxEnumProperty(projectName, wxPG_LABEL, choices, defaultValue));
    } else {
        p->SetChoices(choices);
        p->SetValueFromString(selectedConf);
    }
    ConfigEntry entry;
    entry.project = projectName;
    entry.projectSettings = settings;
    entry.choiceControl = p;
    if(m_projectPropertiesMap.count(projectName)) {
        m_projectPropertiesMap.erase(projectName);
    }
    m_projectPropertiesMap.insert(std::make_pair(projectName, entry));
    if(m_projectPropertiesMap.size() % 2 == 0) {
        p->SetBackgroundColour("#E0E0E0");
    }
    return entry;
}

void ConfigurationManagerDlg::AddEntry(const wxString& projectName, const wxString& selectedConf)
{
    DoCreateChoicesForProject(projectName, selectedConf);
}

void ConfigurationManagerDlg::PopulateConfigurations()
{
    m_pgMgr->GetGrid()->Clear();
    // popuplate the configurations
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    if(!matrix) {
        return;
    }

    wxWindowUpdateLocker locker(this);
    m_projectPropertiesMap.clear();

    std::list<WorkspaceConfigurationPtr> configs = matrix->GetConfigurations();
    m_choiceConfigurations->Clear();
    std::for_each(configs.begin(), configs.end(),
        [&](WorkspaceConfigurationPtr config) { m_choiceConfigurations->Append(config->GetName()); });

    // append the 'New' & 'Delete' commands
    m_choiceConfigurations->Append(wxGetTranslation(clCMD_NEW));
    m_choiceConfigurations->Append(wxGetTranslation(clCMD_EDIT));

    int sel = m_choiceConfigurations->FindString(matrix->GetSelectedConfigurationName());
    if(sel != wxNOT_FOUND) {
        m_choiceConfigurations->SetSelection(sel);
    } else if(m_choiceConfigurations->GetCount() > 2) {
        m_choiceConfigurations->SetSelection(2);
    } else {
        m_choiceConfigurations->Append(_("Debug"));
        m_choiceConfigurations->SetSelection(2);
    }

    // keep the current workspace configuration
    m_currentWorkspaceConfiguration = m_choiceConfigurations->GetStringSelection();

    wxArrayString projects;
    ManagerST::Get()->GetProjectList(projects);
    projects.Sort(wxStringCmpFunc);
    for(size_t i = 0; i < projects.GetCount(); i++) {
        wxString selConf = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), projects.Item(i));
        AddEntry(projects.Item(i), selConf);
    }
}

void ConfigurationManagerDlg::LoadWorkspaceConfiguration(const wxString& confName)
{
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    if(!matrix) {
        return;
    }

    m_choiceConfigurations->SetStringSelection(confName);
    std::map<wxString, ConfigEntry>::iterator iter = m_projectPropertiesMap.begin();
    for(; iter != m_projectPropertiesMap.end(); ++iter) {
        wxString selConf = matrix->GetProjectSelectedConf(confName, iter->second.project);
        iter->second.choiceControl->SetValueFromString(selConf);
    }
}

void ConfigurationManagerDlg::LoadProjectConfiguration(const wxString& projectName)
{
    std::map<wxString, ConfigEntry>::iterator iter = m_projectPropertiesMap.begin();
    for(; iter != m_projectPropertiesMap.end(); iter++) {
        if(iter->second.project == projectName) {
            wxPGProperty* prop = iter->second.choiceControl;
            ProjectSettingsPtr proSet = ManagerST::Get()->GetProjectSettings(projectName);
            if(proSet) {
                // select the build configuration according to the build matrix
                BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
                if(!matrix) {
                    return;
                }
                wxString configName =
                    matrix->GetProjectSelectedConf(m_choiceConfigurations->GetStringSelection(), projectName);
                DoCreateChoicesForProject(projectName, configName, prop);
                return;
            }
        }
    }
}

void ConfigurationManagerDlg::InitDialog() {}

void ConfigurationManagerDlg::OnWorkspaceConfigSelected(wxCommandEvent& event)
{
    if(event.GetString() == wxGetTranslation(clCMD_NEW)) {
        OnButtonNew(event);
    } else if(event.GetString() == wxGetTranslation(clCMD_EDIT)) {
        // popup the delete dialog for configurations
        EditWorkspaceConfDlg* dlg = new EditWorkspaceConfDlg(this);
        dlg->ShowModal();
        dlg->Destroy();

        // once done, restore dialog
        PopulateConfigurations();
    } else {
        if(m_dirty) {
            if(wxMessageBox(
                   wxString::Format(
                       _("Settings for workspace configuration '%s' have changed, would you like to save them?"),
                       m_currentWorkspaceConfiguration.GetData()),
                   _("CodeLite"), wxYES_NO | wxICON_QUESTION) == wxYES) {
                SaveCurrentSettings();
            }
            m_dirty = false;
        }
        m_currentWorkspaceConfiguration = event.GetString();
        LoadWorkspaceConfiguration(event.GetString());
    }
}

void ConfigurationManagerDlg::OnButtonNew(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("Enter New Configuration Name:"), _("New Configuration"));
    if(dlg->ShowModal() == wxID_OK) {
        wxString value = dlg->GetValue();
        TrimString(value);
        if(value.IsEmpty() == false) {
            BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
            if(!matrix) {
                return;
            }

            WorkspaceConfigurationPtr conf(new WorkspaceConfiguration(NULL));
            conf->SetName(value);
            conf->SetConfigMappingList(GetCurrentSettings());
            matrix->SetConfiguration(conf);
            // save changes
            ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
        }
    }
    PopulateConfigurations();
    dlg->Destroy();
}

void ConfigurationManagerDlg::OnButtonApply(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SaveCurrentSettings();
}

WorkspaceConfiguration::ConfigMappingList ConfigurationManagerDlg::GetCurrentSettings()
{
    // return the current settings as described by the dialog
    WorkspaceConfiguration::ConfigMappingList list;

    std::map<wxString, ConfigEntry>::iterator iter = m_projectPropertiesMap.begin();
    for(; iter != m_projectPropertiesMap.end(); ++iter) {

        wxString value = iter->second.choiceControl->GetValueAsString();
        if(value != wxGetTranslation(clCMD_NEW) && value != wxGetTranslation(clCMD_EDIT)) {
            ConfigMappingEntry entry(iter->second.project, value);
            list.push_back(entry);
        }
    }
    return list;
}

void ConfigurationManagerDlg::OnButtonOK(wxCommandEvent& event)
{
    OnButtonApply(event);
    EndModal(wxID_OK);
}

void ConfigurationManagerDlg::SaveCurrentSettings()
{
    m_currentWorkspaceConfiguration = m_currentWorkspaceConfiguration.Trim().Trim(false);

    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    if(!matrix) {
        return;
    }

    matrix->SetSelectedConfigurationName(m_currentWorkspaceConfiguration);

    WorkspaceConfigurationPtr conf = matrix->GetConfigurationByName(m_currentWorkspaceConfiguration);
    if(!conf) {
        // create new configuration
        conf = new WorkspaceConfiguration(NULL);
        conf->SetName(m_currentWorkspaceConfiguration);
        matrix->SetConfiguration(conf);
    }

    conf->SetConfigMappingList(GetCurrentSettings());

    // save changes
    ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
    m_dirty = false;
}

void ConfigurationManagerDlg::OnButtonApplyUI(wxUpdateUIEvent& event) { event.Enable(m_dirty); }

ConfigurationManagerDlg::~ConfigurationManagerDlg() {}

void ConfigurationManagerDlg::OnValueChanged(wxPropertyGridEvent& event)
{
    wxString projectName = event.GetProperty()->GetLabel();
    wxString selection = event.GetProperty()->GetValueAsString();
    if(selection == wxGetTranslation(clCMD_NEW)) {
        // popup the 'New Configuration' dialog
        NewConfigurationDlg* dlg = new NewConfigurationDlg(this, projectName);
        dlg->ShowModal();
        dlg->Destroy();

        // repopulate the configurations
        LoadProjectConfiguration(projectName);

        // clCMD_NEW does not mark the page as dirty !

    } else if(selection == wxGetTranslation(clCMD_EDIT)) {
        EditConfigurationDialog* dlg = new EditConfigurationDialog(this, projectName);
        dlg->ShowModal();
        dlg->Destroy();

        // repopulate the configurations
        LoadProjectConfiguration(projectName);

        m_dirty = true;

    } else {
        // just mark the page as dirty
        m_dirty = true;
    }
}
