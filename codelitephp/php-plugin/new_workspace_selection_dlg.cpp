#include "new_workspace_selection_dlg.h"

NewWorkspaceSelectionDlg::NewWorkspaceSelectionDlg( wxWindow* parent )
	: NewWorkspaceSelectionDlgBase( parent )
	, m_isPHPWorkspace(false)
{
	GetSizer()->Fit(this);
}

void NewWorkspaceSelectionDlg::OnStandardWorkspace( wxCommandEvent& event )
{
	m_isPHPWorkspace = false;
	EndModal(wxID_OK);
}

void NewWorkspaceSelectionDlg::OnPHPWorkspace( wxCommandEvent& event )
{
	m_isPHPWorkspace = true;
	EndModal(wxID_OK);
}

void NewWorkspaceSelectionDlg::OnClose(wxCloseEvent& event)
{
	EndModal(wxID_CANCEL);
}

void NewWorkspaceSelectionDlg::OnKeyDown(wxKeyEvent& event)
{
	if(event.GetKeyCode() == WXK_ESCAPE) {
		Close();
	} else 
		event.Skip();
}
