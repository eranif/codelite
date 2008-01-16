#include "configuration_manager_dlg.h"
#include "manager.h"
#include "new_configuration_dlg.h"
#include "edit_configuration.h"
#include "edit_workspace_conf_dlg.h"
#include "macros.h"

//----------------------------------------------------------------------------
// Configuration Manager
//----------------------------------------------------------------------------


ConfigurationManagerDlg::ConfigurationManagerDlg( wxWindow* parent )
:
ConfigManagerBaseDlg( parent )
{
	PopulateConfigurations();
	InitDialog();
}


void ConfigurationManagerDlg::AddEntry(const wxString &projectName, const wxString &selectedConf)
{
	wxFlexGridSizer *mainSizer = dynamic_cast<wxFlexGridSizer*>(m_scrolledWindow->GetSizer());
	if(!mainSizer) return;

	wxArrayString choices;
	wxChoice *choiceConfig = new wxChoice( m_scrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices, 0 );

	// Get all configuration of the project
	ProjectSettingsPtr settings = ManagerST::Get()->GetProjectSettings(projectName);
	if(settings){
		ProjectSettingsCookie cookie;
		BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
		while(bldConf){
			choiceConfig->Append(bldConf->GetName());
			bldConf = settings->GetNextBuildConfiguration(cookie);
		}
	}
	choiceConfig->Append(clCMD_NEW);
	choiceConfig->Append(clCMD_EDIT);
	ConnectChoice(choiceConfig, ConfigurationManagerDlg::OnConfigSelected);
	wxStaticText *text = new wxStaticText( m_scrolledWindow, wxID_ANY, projectName, wxDefaultPosition, wxDefaultSize, 0 );

	int where = choiceConfig->FindString(selectedConf);
	if(where == wxNOT_FOUND){
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
	if(!matrix){
		return;
	}

	wxFlexGridSizer *mainSizer = dynamic_cast<wxFlexGridSizer*>(m_scrolledWindow->GetSizer());
	if(!mainSizer) return;

	Freeze();
	// remove old entries from the configuration table
	wxSizerItemList list = mainSizer->GetChildren();
	for ( wxSizerItemList::Node *node = list.GetFirst(); node; node = node->GetNext() ){
		wxSizerItem *current = node->GetData();
		current->GetWindow()->Destroy();
	}
	m_projSettingsMap.clear();
	
	std::list<WorkspaceConfigurationPtr> configs = matrix->GetConfigurations();
	std::list<WorkspaceConfigurationPtr>::iterator iter = configs.begin();
	
	m_choiceConfigurations->Clear();
	for(; iter != configs.end(); iter++){
		m_choiceConfigurations->Append((*iter)->GetName());
	}

	// append the 'New' & 'Delete' commands
	m_choiceConfigurations->Append(clCMD_NEW);
	m_choiceConfigurations->Append(clCMD_EDIT);

	int sel = m_choiceConfigurations->FindString(matrix->GetSelectedConfigurationName());
	if(sel != wxNOT_FOUND){
		m_choiceConfigurations->SetSelection(sel);
	}else if(m_choiceConfigurations->GetCount() > 2){
		m_choiceConfigurations->SetSelection(2);
	}else{
		m_choiceConfigurations->Append(wxT("Debug"));
		m_choiceConfigurations->SetSelection(2);
	}
	
	wxArrayString projects;
	ManagerST::Get()->GetProjectList(projects);
	
	for(size_t i=0; i<projects.GetCount(); i++){
		wxString selConf = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(),  projects.Item(i));
		AddEntry(projects.Item(i), selConf);
	}

	mainSizer->Fit(m_scrolledWindow);
	Layout();
	Thaw();
}

void ConfigurationManagerDlg::LoadWorkspaceConfiguration(const wxString &confName)
{
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	if(!matrix){
		return;
	}

	m_choiceConfigurations->SetStringSelection(confName);
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.begin();
	for(; iter != m_projSettingsMap.end(); iter++){
		wxString selConf = matrix->GetProjectSelectedConf(confName, iter->second.project);
		iter->second.choiceControl->SetStringSelection(selConf);
	}
}

void ConfigurationManagerDlg::LoadProjectConfiguration(const wxString &projectName)
{
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.begin();
	for(; iter != m_projSettingsMap.end(); iter++){
		if(iter->second.project == projectName){
			iter->second.choiceControl->Clear();

			ProjectSettingsPtr proSet = ManagerST::Get()->GetProjectSettings(projectName);
			if(proSet){
				ProjectSettingsCookie cookie;
				BuildConfigPtr bldConf = proSet->GetFirstBuildConfiguration(cookie);
				while(bldConf){
					iter->second.choiceControl->Append(bldConf->GetName());
					bldConf = proSet->GetNextBuildConfiguration(cookie);
				}

				//append the EDIT & NEW commands
				iter->second.choiceControl->Append(clCMD_EDIT);
				iter->second.choiceControl->Append(clCMD_NEW);

				//select the build configuration according to the build matrix
				BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
				if(!matrix){
					return;
				}

				wxString configName = matrix->GetProjectSelectedConf(m_choiceConfigurations->GetStringSelection(), projectName);
				int match = iter->second.choiceControl->FindString(configName);
				if(match != wxNOT_FOUND){
					iter->second.choiceControl->SetStringSelection(configName);
				}else{
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
	ConnectChoice(m_choiceConfigurations, ConfigurationManagerDlg::OnWorkspaceConfigSelected);
}

void ConfigurationManagerDlg::OnWorkspaceConfigSelected(wxCommandEvent &event)
{
	if(event.GetString() == clCMD_NEW){
		OnButtonNew(event);
	}else if(event.GetString() == clCMD_EDIT){
		//popup the delete dialog for configurations
		EditWorkspaceConfDlg *dlg = new EditWorkspaceConfDlg(this);
		dlg->ShowModal();
		dlg->Destroy();

		//once done, restore dialog
		PopulateConfigurations();
	}else{
		LoadWorkspaceConfiguration(event.GetString());
	}
}

void ConfigurationManagerDlg::OnConfigSelected(wxCommandEvent &event)
{
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.find(event.GetId());
	if(iter == m_projSettingsMap.end())
		return;

	wxString selection = event.GetString();
	if(selection == clCMD_NEW){
		// popup the 'New Configuration' dialog
		NewConfigurationDlg *dlg = new NewConfigurationDlg(this, iter->second.project);
		dlg->ShowModal();
		dlg->Destroy();
		
		// repopulate the configurations
		LoadProjectConfiguration(iter->second.project);

	}else if(selection == clCMD_EDIT){
		EditConfigurationDialog *dlg = new EditConfigurationDialog(this, iter->second.project);
		dlg->ShowModal();
		dlg->Destroy();

		// repopulate the configurations
		LoadProjectConfiguration(iter->second.project);

	}else{
		// do nothing
	}
}

void ConfigurationManagerDlg::OnButtonNew(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Enter New Configuration Name:"), wxT("New Configuration"));
	if(dlg->ShowModal() == wxID_OK){
		wxString value = dlg->GetValue();
		TrimString(value);
		if(value.IsEmpty() == false){
			BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
			if(!matrix){
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
	ManagerST::Get()->DoUpdateConfigChoiceControl();
}


WorkspaceConfiguration::ConfigMappingList ConfigurationManagerDlg::GetCurrentSettings()
{
	//return the current settings as described by the dialog
	WorkspaceConfiguration::ConfigMappingList list;
	
	std::map<int, ConfigEntry>::iterator iter = m_projSettingsMap.begin();
	for(; iter != m_projSettingsMap.end(); iter++){

		wxString value = iter->second.choiceControl->GetStringSelection();
		if(value != clCMD_NEW && value != clCMD_EDIT){
			ConfigMappingEntry entry(iter->second.project, value);
			list.push_back(entry);
		}
	}
	return list;
}

void ConfigurationManagerDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SaveCurrentSettings();
	ManagerST::Get()->DoUpdateConfigChoiceControl();
	EndModal(wxID_OK);
}

void ConfigurationManagerDlg::SaveCurrentSettings()
{
	wxString sel = m_choiceConfigurations->GetStringSelection();
	TrimString(sel);

	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	if(!matrix){
		return;
	}

	matrix->SetSelectedConfigurationName(sel);

	WorkspaceConfigurationPtr conf = matrix->GetConfigurationByName(sel);
	if(!conf){
		//create new configuration
		conf = new WorkspaceConfiguration(NULL);
		conf->SetName(sel);
		matrix->SetConfiguration(conf);
	}

	conf->SetConfigMappingList(GetCurrentSettings());

	//save changes
	ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
}
