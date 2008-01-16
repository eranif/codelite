///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __quickwatchbasedlg__
#define __quickwatchbasedlg__

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "localvarstree.h"
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class QuickWatchBaseDlg
///////////////////////////////////////////////////////////////////////////////
class QuickWatchBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel1;
		wxTextCtrl* m_textCtrl1;
		LocalVarsTree *m_localVarsTree;
		wxButton* m_buttonEvaluate;
		wxButton* m_buttonClose;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnReEvaluate( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonClose( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		QuickWatchBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Quick Watch"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 524,419 ), long style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__quickwatchbasedlg__
