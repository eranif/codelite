///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __console_frame__
#define __console_frame__

#include <wx/intl.h>

#include <wx/sizer.h>
#include "wxterminal.h"
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConsoleFrame
///////////////////////////////////////////////////////////////////////////////
class ConsoleFrame : public wxFrame
{
private:
	wxTerminal *m_terminal;
protected:

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose( wxCloseEvent& event );
	virtual void OnDebuggerEnded(wxCommandEvent &e);
	virtual void OnEditorGotControl(wxCommandEvent &e);
	virtual void OnEditorLostControl(wxCommandEvent &e);
public:

	ConsoleFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxFRAME_TOOL_WINDOW|wxRESIZE_BORDER|wxSTAY_ON_TOP|wxTAB_TRAVERSAL|wxFRAME_FLOAT_ON_PARENT );
	~ConsoleFrame();

	wxString StartTTY();
};

#endif //__console_frame__
