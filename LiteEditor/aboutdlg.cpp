#include <wx/xrc/xmlres.h>
#include "aboutdlg.h"
#include "contributers.h"

AboutDlg::AboutDlg( wxWindow* parent )
		:
		AboutDlgBase( parent )
{
	m_bitmap->SetBitmap(wxXmlResource::Get()->LoadBitmap(wxT("About")));

	// set the page content
	m_htmlWin3->SetPage(wxString::FromUTF8(about_hex));
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
