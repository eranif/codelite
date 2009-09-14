///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrtDebuggerCommand;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
	
	public:
		DbgCommandBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Debugger Type"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 456,190 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DbgCommandBaseDlg();
	
};

#endif //__dbgcommandbasedlg__
