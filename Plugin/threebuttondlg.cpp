#include "threebuttondlg.h"

ThreeButtonDlg::ThreeButtonDlg( wxWindow* parent, const wxString &msg, const wxString &caption )
: ThreeButtonBaseDlg( parent )
{
	m_message->SetLabel(msg);
	SetLabel(caption);
	GetSizer()->Fit(this);
	GetSizer()->Layout();
}

void ThreeButtonDlg::OnButtonCancel(wxCommandEvent &e)
{
	EndModal(wxID_CANCEL);
}

void ThreeButtonDlg::OnButtonYes(wxCommandEvent &e)
{
	EndModal(wxID_OK);
}

void ThreeButtonDlg::OnButtonNo(wxCommandEvent &e)
{
	EndModal(wxID_NO);
}
