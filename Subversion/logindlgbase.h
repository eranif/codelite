///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __logindlgbase__
#define __logindlgbase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LoginBaseDialog
///////////////////////////////////////////////////////////////////////////////
class LoginBaseDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlUsername;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlPassword;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
	
	public:
		LoginBaseDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Login:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 308,155 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__logindlgbase__
