#include "aboutdlg.h"

AboutDlg::AboutDlg( wxWindow* parent )
:
AboutDlgBase( parent )
{
	m_buttonOk->SetFocus();
	GetSizer()->Fit(this);
}

void AboutDlg::SetInfo(const wxString& info)
{
	m_staticTextInformation->SetLabel(info);
}

wxString AboutDlg::GetInfo() const
{
	return m_staticTextInformation->GetLabelText();
}
