#include "workspacesettingsdlg.h"
#include "code_completion_page.h"
#include "localworkspace.h"
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "windowattrmanager.h"
#include <wx/dirdlg.h>
#include <map>
#include <wx/tokenzr.h>
#include "globals.h"

WorkspaceSettingsDlg::WorkspaceSettingsDlg( wxWindow* parent, LocalWorkspace *localWorkspace)
	: WorkspaceSettingsBase( parent )
	, m_localWorkspace(localWorkspace)
{
	m_ccPage = new CodeCompletionPage(m_notebook1, CodeCompletionPage::TypeWorkspace);
	m_notebook1->AddPage(m_ccPage, wxT("Code Completion"), true);

	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
	std::map<wxString, wxString> envSets = vars.GetEnvVarSets();
	wxString activePage = vars.GetActiveSet();
	m_choiceEnvSets->Clear();

	std::map<wxString, wxString>::iterator iter = envSets.begin();
	int useActiveSetIndex = m_choiceEnvSets->Append(wxGetTranslation(USE_GLOBAL_SETTINGS));

	for (; iter != envSets.end(); iter++) {
		m_choiceEnvSets->Append(iter->first);
	}

	// select the current workspace active set name
	wxString activeEnvSet;
	wxString tmpSet = localWorkspace->GetActiveEnvironmentSet();

	if(tmpSet == _("<Use Active Set>")) {
		tmpSet = wxGetTranslation(USE_GLOBAL_SETTINGS);
	}

	int where = m_choiceEnvSets->FindString(tmpSet);
	if (where == wxNOT_FOUND) {
		activeEnvSet = activePage;
		m_choiceEnvSets->SetSelection(useActiveSetIndex);

	} else {
		activeEnvSet = tmpSet;
		m_choiceEnvSets->SetSelection(where);
	}

	if(activeEnvSet.IsEmpty() == false) {
		vars.SetActiveSet(activeEnvSet);
		EnvironmentConfig::Instance()->SetSettings(vars);
	}

	WindowAttrManager::Load(this, wxT("WorkspaceSettingsDlg"), NULL);
}

WorkspaceSettingsDlg::~WorkspaceSettingsDlg()
{
	WindowAttrManager::Save(this, wxT("WorkspaceSettingsDlg"), NULL);
}

//void WorkspaceSettingsDlg::OnAddIncludePath( wxCommandEvent& event )
//{
//	wxUnusedVar(event);
//	wxString new_path = wxDirSelector(_("Add Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
//	if (new_path.IsEmpty() == false) {
//		wxString curpaths = m_textCtrlIncludePaths->GetValue();
//		curpaths.Trim().Trim(false);
//		if(curpaths.IsEmpty()) {
//			curpaths << wxT("\n");
//		}
//		curpaths << new_path;
//		m_textCtrlIncludePaths->SetValue(curpaths);
//	}
//}
//
//void WorkspaceSettingsDlg::OnAddExcludePath( wxCommandEvent& event )
//{
//	wxUnusedVar(event);
//	wxString new_path = wxDirSelector(_("Add Parser Exclude Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
//	if (new_path.IsEmpty() == false) {
//		wxString curpaths = m_textCtrlExcludePaths->GetValue();
//		curpaths.Trim().Trim(false);
//		if(curpaths.IsEmpty()) {
//			curpaths << wxT("\n");
//		}
//		curpaths << new_path;
//		m_textCtrlExcludePaths->SetValue(curpaths);
//	}
//}
//
wxArrayString WorkspaceSettingsDlg::GetExcludePaths() const
{
	return wxArrayString();
}

wxArrayString WorkspaceSettingsDlg::GetIncludePaths() const
{
	return m_ccPage->GetIncludePaths();
}

void WorkspaceSettingsDlg::OnButtonOK(wxCommandEvent& event)
{
	m_localWorkspace->SetParserPaths(GetIncludePaths(), GetExcludePaths());
	m_localWorkspace->SetActiveEnvironmentSet(m_choiceEnvSets->GetStringSelection());
	event.Skip();
}
