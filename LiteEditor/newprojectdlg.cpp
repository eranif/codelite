#include "newprojectdlg.h"
#include "workspace.h"
#include "build_settings_config.h"
#include "manager.h"

NewProjectDlg::NewProjectDlg( wxWindow* parent )
		:
		NewProjectBaseDlg( parent )
{
	//get list of project templates
	ManagerST::Get()->GetProjectTemplateList(m_list);
	std::list<ProjectPtr>::iterator iter = m_list.begin();
	for (; iter != m_list.end(); iter++) {
		m_projTypes->Append((*iter)->GetName());
	}

	iter = m_list.begin();
	if ( iter != m_list.end() ) {
		m_projTypes->SetStringSelection((*iter)->GetName());
		m_projectData.m_srcProject = (*iter);
	}

	//append list of compilers
	wxArrayString choices;
	//get list of compilers from configuration file
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	while (cmp) {
		choices.Add(cmp->GetName());
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
	}

	m_choiceCompilerType->Append( choices );
	if (choices.IsEmpty() == false) {
		m_choiceCompilerType->SetSelection(0);
	}

	m_textCtrlProjName->SetFocus();
}

void NewProjectDlg::OnProjectPathUpdated( wxCommandEvent& event )
{
	wxString projectPath;
	projectPath << m_textCtrlProjectPath->GetValue();

	projectPath = projectPath.Trim().Trim(false);

	wxString tmpSep( wxFileName::GetPathSeparator() );
	if ( !projectPath.EndsWith(tmpSep) && projectPath.IsEmpty() == false ) {
		projectPath << wxFileName::GetPathSeparator();
	}

	projectPath << m_textCtrlProjName->GetValue();
	projectPath << wxT(".project");

	m_staticTextProjectFileFullPath->SetLabel( projectPath );
}

void NewProjectDlg::OnProjectPathPicker( wxCommandEvent& event )
{
	const wxString& dir = wxDirSelector(wxT("Choose a folder:"), WorkspaceST::Get()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
	if ( !dir.empty() ) {
		m_textCtrlProjectPath->SetValue( dir );
	}
}

void NewProjectDlg::OnButtonCreate(wxCommandEvent &e)
{
	//validate that the path part is valid
	wxString projFullPath = m_staticTextProjectFileFullPath->GetLabel();
	wxFileName fn(projFullPath);

	if ( !wxDirExists(fn.GetPath()) ) {
		wxMessageBox(wxT("Invalid path: ") + fn.GetPath(), wxT("Error"), wxOK | wxICON_HAND);
		return;
	}

	m_projectData.m_name = m_textCtrlProjName->GetValue();
	m_projectData.m_path = m_textCtrlProjectPath->GetValue();
	m_projectData.m_cmpType = m_choiceCompilerType->GetStringSelection();
	
	EndModal(wxID_OK);
}

void NewProjectDlg::OnItemSelected( wxCommandEvent& event )
{
	m_projectData.m_srcProject = FindProject( event.GetString() );
}

ProjectPtr NewProjectDlg::FindProject(const wxString &name)
{
	std::list<ProjectPtr>::iterator iter = m_list.begin();
	for (; iter != m_list.end(); iter++) {
		if ((*iter)->GetName() == name) {
			return (*iter);
		}
	}
	return NULL;
}
