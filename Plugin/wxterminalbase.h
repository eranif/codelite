///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __wxterminalbase__
#define __wxterminalbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class wxTerminalBase
///////////////////////////////////////////////////////////////////////////////
class wxTerminalBase : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKey( wxKeyEvent& event ){ event.Skip(); }
		virtual void OnText( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEnter( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnURL( wxTextUrlEvent& event ){ event.Skip(); }
		
	
	public:
		wxTerminalBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~wxTerminalBase();
	
};

#endif //__wxterminalbase__
