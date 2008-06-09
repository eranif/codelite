///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __aboutdlg__
#define __aboutdlg__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AboutDlg
///////////////////////////////////////////////////////////////////////////////
class AboutDlg : public wxDialog
{
private:

protected:
	wxStaticBitmap* m_bitmap;
	wxStaticLine* m_staticline;
	wxButton* m_buttonOk;

public:
	AboutDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About CodeLite"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 645,448 ), long style = wxDEFAULT_DIALOG_STYLE );
	~AboutDlg();

};

#endif //__aboutdlg__
