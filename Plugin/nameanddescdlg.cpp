#include "nameanddescdlg.h"

NameAndDescDlg::NameAndDescDlg( wxWindow* parent, const wxString &name )
		:NameAndDescBaseDlg( parent )
{
	m_textCtrlName->SetValue(name);
}

void NameAndDescDlg::OnButtonOK( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

void NameAndDescDlg::OnButtonCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}
