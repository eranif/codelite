#include "newworkspacedlg.h"
#include "wx/msgdlg.h"
#include "wx/dirdlg.h"
#include "wx/filename.h"

NewWorkspaceDlg::NewWorkspaceDlg( wxWindow* parent )
		:
		NewWorkspaceBase( parent )
{

}

void NewWorkspaceDlg::OnWorkspacePathUpdated( wxCommandEvent& event )
{
	//update the static text control with the actual path

	wxString workspacePath;
	workspacePath << m_textCtrlWorkspacePath->GetValue();

	workspacePath = workspacePath.Trim().Trim(false);

	wxString tmpSep( wxFileName::GetPathSeparator() );
	if ( !workspacePath.EndsWith(tmpSep) && workspacePath.IsEmpty() == false ) {
		workspacePath << wxFileName::GetPathSeparator();
	}
	
	if( m_textCtrlWorkspaceName->GetValue().Trim().Trim(false).IsEmpty() ) {
		m_staticTextWorkspaceFileName->SetLabel(wxEmptyString);
		return;
	}
	
	workspacePath << m_textCtrlWorkspaceName->GetValue();
	
	workspacePath << wxT(".workspace");

	m_staticTextWorkspaceFileName->SetLabel( workspacePath );
}

void NewWorkspaceDlg::OnWorkspaceDirPicker( wxCommandEvent& event )
{
	const wxString& dir = wxDirSelector(wxT("Choose a folder:"));
	if ( !dir.empty() ) {
		m_textCtrlWorkspacePath->SetValue( dir );
	}
}

void NewWorkspaceDlg::OnButtonCreate( wxCommandEvent& event )
{
	//validate that the path part is valid
	m_workspacePath = m_staticTextWorkspaceFileName->GetLabel();
	wxFileName fn(m_workspacePath);

	if ( !wxDirExists(fn.GetPath()) ) {
		wxMessageBox(wxT("Invalid path: ") + fn.GetPath(), wxT("Error"), wxOK | wxICON_HAND);
		return;
	}

	EndModal(wxID_OK);
}
