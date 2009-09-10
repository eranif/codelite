///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __debuggerasciiviewerbase__
#define __debuggerasciiviewerbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/wxscintilla.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerAsciiViewerBase
///////////////////////////////////////////////////////////////////////////////
class DebuggerAsciiViewerBase : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrlExpression;
		wxScintilla *m_textView;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnEnter( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		DebuggerAsciiViewerBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~DebuggerAsciiViewerBase();
	
};

#endif //__debuggerasciiviewerbase__
