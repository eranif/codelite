#include "newversiondlg.h"

NewVersionDlg::NewVersionDlg( wxWindow* parent )
		: NewVersionBaseDlg( parent )
{
	m_buttonOk->SetFocus();
}

void NewVersionDlg::OnOk( wxCommandEvent& event )
{
	wxUnusedVar( event );
	EndModal(wxID_OK);
}

void NewVersionDlg::OnCancel( wxCommandEvent& event )
{
	wxUnusedVar( event );
	EndModal(wxID_CANCEL);
}

void NewVersionDlg::SetMessage(const wxString& message)
{
	m_staticText->SetLabel(message);
	GetSizer()->Fit(this);
	GetSizer()->Layout();
}

void NewVersionDlg::SetReleaseNotesURL(const wxString& url)
{
	m_hyperlink1->SetURL(url);
}
