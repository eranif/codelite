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
    CentreOnParent();
    SetName("ConfigurationManagerDlg");
    WindowAttrManager::Load(this);
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_CHOICE_BUTTON, &ConfigurationManagerDlg::OnShowConfigList, this);
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_CHOICE, &ConfigurationManagerDlg::OnValueChanged, this);
}

wxArrayString ConfigurationManagerDlg::GetChoicesForProject(const wxString& projectName,
                                                            const wxString& workspaceConfig, size_t& index)
{
    wxArrayString choices;
    ProjectPtr project = ManagerST::Get()->GetProject(projectName);
    if(!project) { return choices; }

    // Get the workspace configuration
    wxString projectConfig =
        clCxxWorkspaceST::Get()->GetBuildMatrix()->GetProjectSelectedConf(workspaceConfig, projectName);
    if(projectConfig.IsEmpty()) { return choices; }

    // Get all configuration of the project
    ProjectSettingsPtr settings = project->GetSettings();
    size_t counter = 0;
    if(settings) {
        ProjectSettingsCookie cookie;
        BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
        while(bldConf) {
            choices.Add(bldConf->GetName());
            if(projectConfig == bldConf->GetName()) { index = counter; }
            bldConf = settings->GetNextBuildConfiguration(cookie);
            ++counter;
        }
    }

    choices.Add(wxGetTranslation(clCMD_NEW));
    choices.Add(wxGetTranslation(clCMD_EDIT));
    return choices;
}

void ConfigurationManagerDlg::PopulateConfigurations()
{
    // popuplate the configurations
    BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
    if(!matrix) { return; }

    wxWindowUpdateLocker locker(this);
    m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) {
        wxArrayString* choices = (wxArrayString*)d;
        wxDELETE(choices);
    });

    std::list<WorkspaceConfigurationPtr> configs = matrix->GetConfigurations();
    m_choiceConfigurations->Clear();
    for(WorkspaceConfigurationPtr config : configs) {
        m_choiceConfigurations->Append(config->GetName());
    }

    // append the 'New' & 'Delete' commands
    m_choiceConfigurations->Append(wxGetTranslation(clCMD_NEW));
    m_choiceConfigurations->Append(wxGetTranslation(clCMD_EDIT));

    int sel = m_choiceConfigurations->FindString(m_currentWorkspaceConfiguration.IsEmpty()
                                                     ? matrix->GetSelectedConfigurationName()
                                                     : m_currentWorkspaceConfiguration);
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
    for(size_t i = 0; i < projects.GetCount(); ++i) {
        size_t index = wxString::npos;
        wxArrayString choices = GetChoicesForProject(projects[i], m_currentWorkspaceConfiguration, index);
        clDataViewChoice c(index != wxString::npos ? choices[index] : "");
        wxVariant v;
        v << c;
        wxVector<wxVariant> cols;
        cols.push_back(projects[i]);
        cols.push_back(v);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new wxArrayString(choices));
    }
}

void ConfigurationManagerDlg::LoadWorkspaceConfiguration(const wxString& confName)
{
    m_choiceConfigurations->SetStringSelection(confName);
    PopulateConfigurations();
}

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
                   _("CodeLite"), wxYES | wxCANCEL | wxYES_DEFAULT | wxICON_QUESTION) != wxYES) {
                return;
            }
            SaveCurrentSettings();
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
            if(!matrix) { return; }

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

    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        wxString projectName = m_dvListCtrl->GetItemText(item, 0);
        wxString configName = m_dvListCtrl->GetItemText(item, 1);
        if((configName != wxGetTranslation(clCMD_NEW)) && (configName != wxGetTranslation(clCMD_EDIT))) {
            ConfigMappingEntry entry(projectName, configName);
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
    if(!matrix) { return; }

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

ConfigurationManagerDlg::~ConfigurationManagerDlg()
{
    m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) {
        wxArrayString* choices = (wxArrayString*)d;
        wxDELETE(choices);
    });
}

void ConfigurationManagerDlg::OnValueChanged(wxDataViewEvent& event)
{
    wxString projectName = m_dvListCtrl->GetItemText(event.GetItem(), 0);
    wxString selection = event.GetString(); // the new selection

    if(selection == wxGetTranslation(clCMD_NEW)) {
        // popup the 'New Configuration' dialog
        NewConfigurationDlg dlg(this, projectName);
        if(dlg.ShowModal() == wxID_OK) {

            // clCMD_NEW does not mark the page as dirty !
            PopulateConfigurations();
        }
        event.Veto(); // prevent the change from taking place
    } else if(selection == wxGetTranslation(clCMD_EDIT)) {
        EditConfigurationDialog dlg(this, projectName);
        if(dlg.ShowModal() == wxID_OK) {
            m_dirty = true;
            PopulateConfigurations();
        }
        event.Veto(); // prevent the change from taking place
    } else {
        // just mark the page as dirty
        m_dirty = true;
    }
}

void ConfigurationManagerDlg::OnShowConfigList(wxDataViewEvent& event)
{
    event.Skip();
    wxDataViewItem item = event.GetItem();
    wxArrayString* choices = reinterpret_cast<wxArrayString*>(m_dvListCtrl->GetItemData(item));
    if(!choices) { return; }

    m_dvListCtrl->ShowStringSelectionMenu(item, *choices, event.GetColumn());
}
