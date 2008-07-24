#include "AboutDlg.h"

AboutDlg::AboutDlg( wxWindow* parent )
:
AboutDlgBase( parent )
{
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
