///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ABOUTDLGBASE_H__
#define __ABOUTDLGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AboutDlgBase
///////////////////////////////////////////////////////////////////////////////
class AboutDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxStaticBitmap* m_bitmap;
		wxPanel* m_panel4;
		wxHtmlWindow* m_htmlWin3;
		wxPanel* m_panel3;
		wxTextCtrl* m_textCtrlLicense;
		wxStaticText* m_staticTextHomePage;
		wxHyperlinkCtrl* m_hyperlink1;
		wxStaticText* m_staticText2;
		wxHyperlinkCtrl* m_hyperlink2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticTextInformation;
		wxStaticBitmap* m_bitmapPayPal;
		wxHyperlinkCtrl* m_hyperlink3;
		wxButton* m_buttonOk;
	
	public:
		
		AboutDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About CodeLite"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~AboutDlgBase();
	
};

#endif //__ABOUTDLGBASE_H__
