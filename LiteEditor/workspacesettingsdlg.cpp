#include "workspacesettingsdlg.h"
#include "windowattrmanager.h"
#include <wx/dirdlg.h>

WorkspaceSettingsDlg::WorkspaceSettingsDlg( wxWindow* parent, const wxArrayString &includePaths, const wxArrayString &excludePaths  )
		: WorkspaceSettingsBase( parent )
{
	m_listBoxExcludePaths->Append(excludePaths);
	m_listBoxSearchPaths->Append(includePaths);
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
	if(new_path.IsEmpty() == false){
		if(m_listBoxSearchPaths->GetStrings().Index(new_path) == wxNOT_FOUND) {
			m_listBoxSearchPaths->Append(new_path);
		}
	}
}

void WorkspaceSettingsDlg::OnRemoveIncludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	int sel = m_listBoxSearchPaths->GetSelection();
	if( sel != wxNOT_FOUND) {
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
	if(new_path.IsEmpty() == false){
		if(m_listBoxExcludePaths->GetStrings().Index(new_path) == wxNOT_FOUND) {
			m_listBoxExcludePaths->Append(new_path);
		}
	}
}

void WorkspaceSettingsDlg::OnRemoveExcludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	int sel = m_listBoxExcludePaths->GetSelection();
	if( sel != wxNOT_FOUND) {
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
