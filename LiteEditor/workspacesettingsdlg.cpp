#include "workspacesettingsdlg.h"
#include "localworkspace.h"
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "windowattrmanager.h"
#include <wx/dirdlg.h>
#include <map>

WorkspaceSettingsDlg::WorkspaceSettingsDlg( wxWindow* parent, LocalWorkspace *localWorkspace)
: WorkspaceSettingsBase( parent )
, m_localWorkspace(localWorkspace)
{
	wxArrayString excludePaths, includePaths;
	localWorkspace->GetParserPaths(includePaths, excludePaths);

	m_listBoxExcludePaths->Append(excludePaths);
	m_listBoxSearchPaths->Append(includePaths);

	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
	std::map<wxString, wxString> envSets = vars.GetEnvVarSets();
	wxString activePage = vars.GetActiveSet();
	m_choiceEnvSets->Clear();

	std::map<wxString, wxString>::iterator iter = envSets.begin();
	int useActiveSetIndex = m_choiceEnvSets->Append(USE_GLOBAL_SETTINGS);

	for (; iter != envSets.end(); iter++) {
		m_choiceEnvSets->Append(iter->first);
	}

	// select the current workspace active set name
	wxString activeEnvSet;
	int where = m_choiceEnvSets->FindString(localWorkspace->GetActiveEnvironmentSet());
	if (where == wxNOT_FOUND) {
		activeEnvSet = activePage;
		m_choiceEnvSets->SetSelection(useActiveSetIndex);

	} else {
		activeEnvSet = localWorkspace->GetActiveEnvironmentSet();
		m_choiceEnvSets->SetSelection(where);
	}

	if(activeEnvSet.IsEmpty() == false){
		vars.SetActiveSet(activeEnvSet);
		EnvironmentConfig::Instance()->SetSettings(vars);
	}

	WindowAttrManager::Load(this, wxT("WorkspaceSettingsDlg"), NULL);
}

WorkspaceSettingsDlg::~WorkspaceSettingsDlg()
{
	WindowAttrManager::Save(this, wxT("WorkspaceSettingsDlg"), NULL);
}

void WorkspaceSettingsDlg::OnAddIncludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_path = wxDirSelector(wxT("Add Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
		if (m_listBoxSearchPaths->GetStrings().Index(new_path) == wxNOT_FOUND) {
			m_listBoxSearchPaths->Append(new_path);
		}
	}
}

void WorkspaceSettingsDlg::OnRemoveIncludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	int sel = m_listBoxSearchPaths->GetSelection();
	if ( sel != wxNOT_FOUND) {
		m_listBoxSearchPaths->Delete((unsigned int)sel);
	}
}

void WorkspaceSettingsDlg::OnRemoveIncludePathUI( wxUpdateUIEvent& event )
{
	event.Enable( m_listBoxSearchPaths->GetSelection() != wxNOT_FOUND );
}

void WorkspaceSettingsDlg::OnClearAllIncludePaths( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_listBoxSearchPaths->Clear();
}

void WorkspaceSettingsDlg::OnClearAllIncludePathUI( wxUpdateUIEvent& event )
{
	event.Enable( m_listBoxSearchPaths->IsEmpty() == false );
}

void WorkspaceSettingsDlg::OnAddExcludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_path = wxDirSelector(wxT("Add Parser Exclude Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
		if (m_listBoxExcludePaths->GetStrings().Index(new_path) == wxNOT_FOUND) {
			m_listBoxExcludePaths->Append(new_path);
		}
	}
}

void WorkspaceSettingsDlg::OnRemoveExcludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	int sel = m_listBoxExcludePaths->GetSelection();
	if ( sel != wxNOT_FOUND) {
		m_listBoxExcludePaths->Delete((unsigned int)sel);
	}
}

void WorkspaceSettingsDlg::OnRemoveExcludePathUI( wxUpdateUIEvent& event )
{
	event.Enable( m_listBoxExcludePaths->GetSelection() != wxNOT_FOUND );
}

void WorkspaceSettingsDlg::OnClearAllExcludePaths( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_listBoxExcludePaths->Clear();
}

void WorkspaceSettingsDlg::OnClearAllExcludePathsUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBoxExcludePaths->IsEmpty() == false);
}

wxArrayString WorkspaceSettingsDlg::GetExcludePaths() const
{
	return m_listBoxExcludePaths->GetStrings();
}

wxArrayString WorkspaceSettingsDlg::GetIncludePaths() const
{
	return m_listBoxSearchPaths->GetStrings();
}

void WorkspaceSettingsDlg::OnButtonOK(wxCommandEvent& event)
{
	m_localWorkspace->SetParserPaths(GetIncludePaths(), GetExcludePaths());
	m_localWorkspace->SetActiveEnvironmentSet(m_choiceEnvSets->GetStringSelection());
	event.Skip();
}
