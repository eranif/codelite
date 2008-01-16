///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __openwindowspanelbase__
#define __openwindowspanelbase__

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class OpenWindowsPanelBase
///////////////////////////////////////////////////////////////////////////////
class OpenWindowsPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxListBox* m_fileList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChar( wxKeyEvent& event ){ event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnItemDClicked( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRightUp( wxMouseEvent& event ){ event.Skip(); }
		
	
	public:
		OpenWindowsPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,300 ), long style = wxTAB_TRAVERSAL|wxWANTS_CHARS );
	
};

#endif //__openwindowspanelbase__
