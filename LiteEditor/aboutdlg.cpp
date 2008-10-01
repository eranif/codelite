#include <wx/dcmemory.h>
#include <wx/xrc/xmlres.h>
#include "aboutdlg.h"
#include "contributers.h"

static void DrawBitmap(	wxBitmap& bitmap, const wxString &mainTitle)
{
    wxMemoryDC dcMem;

    dcMem.SelectObject(bitmap);
	
	//write the main title
	wxCoord w, h;
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont smallfont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetPointSize(12);
	smallfont.SetPointSize(10);
	dcMem.SetFont(font);
	dcMem.GetMultiLineTextExtent(mainTitle, &w, &h);
	wxCoord bmpW = bitmap.GetWidth();
	
	//draw shadow 
	dcMem.SetTextForeground(wxT("LIGHT GRAY"));
	
	dcMem.DrawText(mainTitle, bmpW - w - 9, 11);
	//draw the text	
	dcMem.SetTextForeground(wxT("BLACK"));
	dcMem.SetFont(font);
	
	//draw the main title
	wxCoord textX = bmpW - w - 10;
	wxCoord textY = 10;
	dcMem.DrawText(mainTitle, textX, textY);
	dcMem.SelectObject(wxNullBitmap);
}

AboutDlg::AboutDlg( wxWindow* parent, const wxString &mainTitle )
		: AboutDlgBase( parent )
{
	m_bmp = wxXmlResource::Get()->LoadBitmap(wxT("About"));
	DrawBitmap(m_bmp, mainTitle);
	m_bitmap->SetBitmap(m_bmp);

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
