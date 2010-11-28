///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __newunittestbasedlg__
#define __newunittestbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NewUnitTestBaseDlg
///////////////////////////////////////////////////////////////////////////////
class NewUnitTestBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText30;
		wxTextCtrl* m_textCtrlTestName;
		wxStaticText* m_staticText32;
		wxTextCtrl* m_textCtrlFixtureName;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlOutputFile;
		wxStaticText* m_staticText3;
		wxChoice* m_choiceProjects;
		wxStaticLine* m_staticline2;
		wxButton* m_button24;
		wxButton* m_button23;
	
	public:
		
		NewUnitTestBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Unit Test"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~NewUnitTestBaseDlg();
	
};

#endif //__newunittestbasedlg__
