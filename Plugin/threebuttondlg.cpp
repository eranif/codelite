#include "threebuttondlg.h"

ThreeButtonDlg::ThreeButtonDlg( wxWindow* parent, const wxString &msg, const wxString &caption )
: ThreeButtonBaseDlg( parent )
{
	m_message->SetLabel(msg);
	m_buttonYes->SetFocus();
	SetLabel(caption);
	GetSizer()->Fit(this);
	GetSizer()->Layout();
}

void ThreeButtonDlg::OnButtonYes(wxCommandEvent &e)
{
	EndModal(wxID_OK);
}

void ThreeButtonDlg::OnButtonNo(wxCommandEvent &e)
{
	EndModal(wxID_NO);
}

void ThreeButtonDlg::OnButtonCancel(wxCommandEvent &e)
{
	EndModal(wxID_CANCEL);
}
