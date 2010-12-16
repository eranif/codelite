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

//----------------------------------------------------------------------------
// Configuration Manager
//----------------------------------------------------------------------------

static int wxStringCmpFunc(const wxString& item1, const wxString& item2) 
{
	return item1.CmpNoCase(item2);
}

ConfigurationManagerDlg::ConfigurationManagerDlg( wxWindow* parent )
		: ConfigManagerBaseDlg( parent )
		, m_dirty(false)
{
	PopulateConfigurations();
	InitDialog();
	
	WindowAttrManager::Load(this, wxT("ConfigurationManagerDlg"), NULL);
}


void ConfigurationManagerDlg::AddEntry(const wxString &projectName, const wxString &selectedConf)
{
	wxFlexGridSizer *mainSizer = dynamic_cast<wxFlexGridSizer*>(m_scrolledWindow->GetSizer());
	if (!mainSizer) return;

	wxArrayString choices;
	wxChoice *choiceConfig = new wxChoice( m_scrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices, 0 );

	// Get all configuration of the project
	ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(projectName);
	if (settings) {
		ProjectSettingsCookie cookie;
		BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
		while (bldConf) {
			choiceConfig->Append(bldConf->GetName());
			bldConf = settings->GetNextBuildConfiguration(cookie);
		}
	}
	choiceConfig->Append(wxGetTranslation(clCMD_NEW));
	choiceConfig->Append(wxGetTranslation(clCMD_EDIT));
	ConnectChoice(choiceConfig, ConfigurationManagerDlg::OnConfigSelected);
	wxStaticText *text = new wxStaticText( m_scrolledWindow, wxID_ANY, projectName, wxDefaultPosition, wxDefaultSize, 0 );

	int where = choiceConfig->FindString(selectedConf);
	if (where == wxNOT_FOUND) {
		where = 0;
	}
	choiceConfig->SetSelection(where);
	mainSizer->Add(text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	mainSizer->Add(choiceConfig, 1, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 5);

	ConfigEntry entry;
	entry.project = projectName;
	entry.projectSettings = settings;
	entry.choiceControl = choiceConfig;

	m_projSettingsMap[choiceConfig->GetId()] = entry;
}

void ConfigurationManagerDlg::PopulateConfigurations()
{
	//popuplate the configurations
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	if (!matrix) {
		return;
	}

	wxFlexGridSizer *mainSizer = dynamic_cast<wxFlexGridSizer*>(m_scrolledWindow->GetSizer());
	if (!mainSizer) return;

	Freeze();
	// remove old entries from the configuration table
	wxSizerItemList list = mainSizer->GetChildren();
	for ( wxSizerItemList::Node *node = list.GetFirst(); node; node = node->GetNext() ) {
		wxSizerItem *current = node->GetData();
		current->GetWindow()->Destroy();
	}
	m_projSettingsMap.clear();

	std::list<WorkspaceConfigurationPtr> configs = matrix->GetConfigurations();
	std::list<WorkspaceConfigurationPtr>::iterator iter = configs.begin();

	m_choiceConfigurations->Clear();
	for (; iter != configs.end(); iter++) {
		m_choiceConfigurations->Append((*iter)->GetName());
	}

	// append the 'New' & 'Delete' commands
	m_choiceConfigurations->Append(wxGetTranslation(clCMD_NEW));
	m_choiceConfigurations->Append(wxGetTranslation(clCMD_EDIT));

	int sel = m_choiceConfigurations->FindString(matrix->GetSelectedConfigurationName());
	if (sel != wxNOT_FOUND) {
		m_choiceConfigurations->SetSelection(sel);
	} else if (m_choiceConfigurations->GetCount() > 2) {
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
	
	for (size_t i=0; i<projects.GetCount(); i++) {
		wxString selConf = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(),  projects.Item(i));
		AddEntry(projects.Item(i), selConf);
	}

	Thaw();
	mainSizer->Fit(m_scrolledWindow);
	Layout();

}

void ConfigurationManagerDlg::LoadWorkspaceConfiguration(const wxString &confName)
{
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	if (!matrix) {
		return;
	}

	m_choiceConfigurations->SetStringSelection(confName);
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.begin();
	for (; iter != m_projSettingsMap.end(); iter++) {
		wxString selConf = matrix->GetProjectSelectedConf(confName, iter->second.project);
		iter->second.choiceControl->SetStringSelection(selConf);
	}
}

void ConfigurationManagerDlg::LoadProjectConfiguration(const wxString &projectName)
{
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.begin();
	for (; iter != m_projSettingsMap.end(); iter++) {
		if (iter->second.project == projectName) {
			iter->second.choiceControl->Clear();

			ProjectSettingsPtr proSet = ManagerST::Get()->GetProjectSettings(projectName);
			if (proSet) {
				ProjectSettingsCookie cookie;
				BuildConfigPtr bldConf = proSet->GetFirstBuildConfiguration(cookie);
				while (bldConf) {
					iter->second.choiceControl->Append(bldConf->GetName());
					bldConf = proSet->GetNextBuildConfiguration(cookie);
				}

				//append the EDIT & NEW commands
				iter->second.choiceControl->Append(wxGetTranslation(clCMD_EDIT));
				iter->second.choiceControl->Append(wxGetTranslation(clCMD_NEW));

				//select the build configuration according to the build matrix
				BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
				if (!matrix) {
					return;
				}

				wxString configName = matrix->GetProjectSelectedConf(m_choiceConfigurations->GetStringSelection(), projectName);
				int match = iter->second.choiceControl->FindString(configName);
				if (match != wxNOT_FOUND) {
					iter->second.choiceControl->SetStringSelection(configName);
				} else {
					iter->second.choiceControl->SetSelection(0);
				}

				return;
			}
		}
	}
}

void ConfigurationManagerDlg::InitDialog()
{
	// Connect events
	ConnectButton(m_buttonOK, ConfigurationManagerDlg::OnButtonOK);
	ConnectButton(m_buttonApply, ConfigurationManagerDlg::OnButtonApply);
	m_buttonApply->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ConfigurationManagerDlg::OnButtonApplyUI ), NULL, this );
	ConnectChoice(m_choiceConfigurations, ConfigurationManagerDlg::OnWorkspaceConfigSelected);
}

void ConfigurationManagerDlg::OnWorkspaceConfigSelected(wxCommandEvent &event)
{
	if (event.GetString() == wxGetTranslation(clCMD_NEW)) {
		OnButtonNew(event);
	} else if (event.GetString() == wxGetTranslation(clCMD_EDIT)) {
		//popup the delete dialog for configurations
		EditWorkspaceConfDlg *dlg = new EditWorkspaceConfDlg(this);
		dlg->ShowModal();
		dlg->Destroy();

		//once done, restore dialog
		PopulateConfigurations();
	} else {
		if (m_dirty) {
			if ( wxMessageBox(wxString::Format(_("Settings for workspace configuration '%s' have changed, would you like to save them?"), m_currentWorkspaceConfiguration.GetData()), _("CodeLite"), wxYES_NO|wxICON_QUESTION) == wxYES) {
				SaveCurrentSettings();
			}
			m_dirty = false;
		}
		m_currentWorkspaceConfiguration = event.GetString();
		LoadWorkspaceConfiguration(event.GetString());
	}
}

void ConfigurationManagerDlg::OnConfigSelected(wxCommandEvent &event)
{
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.find(event.GetId());
	if (iter == m_projSettingsMap.end())
		return;

	wxString selection = event.GetString();
	if (selection == wxGetTranslation(clCMD_NEW)) {
		// popup the 'New Configuration' dialog
		NewConfigurationDlg *dlg = new NewConfigurationDlg(this, iter->second.project);
		dlg->ShowModal();
		dlg->Destroy();

		// repopulate the configurations
		LoadProjectConfiguration(iter->second.project);
		
		// clCMD_NEW does not mark the page as dirty !
		
	} else if (selection == wxGetTranslation(clCMD_EDIT)) {
		EditConfigurationDialog *dlg = new EditConfigurationDialog(this, iter->second.project);
		dlg->ShowModal();
		dlg->Destroy();

		// repopulate the configurations
		LoadProjectConfiguration(iter->second.project);
		
		m_dirty = true;

	} else {
		// just mark the page as dirty
		m_dirty = true;
	}
}

void ConfigurationManagerDlg::OnButtonNew(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, _("Enter New Configuration Name:"), _("New Configuration"));
	if (dlg->ShowModal() == wxID_OK) {
		wxString value = dlg->GetValue();
		TrimString(value);
		if (value.IsEmpty() == false) {
			BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
			if (!matrix) {
				return;
			}

			WorkspaceConfigurationPtr conf(new WorkspaceConfiguration(NULL));
			conf->SetName(value);
			conf->SetConfigMappingList(GetCurrentSettings());
			matrix->SetConfiguration(conf);
			//save changes
			ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
		}
	}
	PopulateConfigurations();
	dlg->Destroy();
}

void ConfigurationManagerDlg::OnButtonApply(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SaveCurrentSettings();
}


WorkspaceConfiguration::ConfigMappingList ConfigurationManagerDlg::GetCurrentSettings()
{
	//return the current settings as described by the dialog
	WorkspaceConfiguration::ConfigMappingList list;

	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.begin();
	for (; iter != m_projSettingsMap.end(); iter++) {

		wxString value = iter->second.choiceControl->GetStringSelection();
		if (value != wxGetTranslation(clCMD_NEW) && value != wxGetTranslation(clCMD_EDIT)) {
			ConfigMappingEntry entry(iter->second.project, value);
			list.push_back(entry);
		}
	}
	return list;
}

void ConfigurationManagerDlg::OnButtonOK(wxCommandEvent &event)
{
	OnButtonApply(event);
	EndModal(wxID_OK);
}

void ConfigurationManagerDlg::SaveCurrentSettings()
{
	m_currentWorkspaceConfiguration = m_currentWorkspaceConfiguration.Trim().Trim(false);

	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	if (!matrix) {
		return;
	}

	matrix->SetSelectedConfigurationName(m_currentWorkspaceConfiguration);

	WorkspaceConfigurationPtr conf = matrix->GetConfigurationByName(m_currentWorkspaceConfiguration);
	if (!conf) {
		//create new configuration
		conf = new WorkspaceConfiguration(NULL);
		conf->SetName(m_currentWorkspaceConfiguration);
		matrix->SetConfiguration(conf);
	}

	conf->SetConfigMappingList(GetCurrentSettings());

	//save changes
	ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
	m_dirty = false;
}

void ConfigurationManagerDlg::OnButtonApplyUI(wxUpdateUIEvent& event)
{
	event.Enable(m_dirty);
}

ConfigurationManagerDlg::~ConfigurationManagerDlg()
{
	WindowAttrManager::Save(this, wxT("ConfigurationManagerDlg"), NULL);
}
