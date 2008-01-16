///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __dbgcommandbasedlg__
#define __dbgcommandbasedlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DbgCommandBaseDlg
///////////////////////////////////////////////////////////////////////////////
class DbgCommandBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlName;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlCommand;
		wxCheckBox* m_checkBox1;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
	
	public:
		DbgCommandBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Debugger Command:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 456,160 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__dbgcommandbasedlg__
