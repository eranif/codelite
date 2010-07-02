///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cc_boxbase__
#define __cc_boxbase__

#include <wx/string.h>
#include "ccvirtuallistctrl.h"
#include <wx/dialog.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define TOOL_SHOW_PRIVATE_MEMBERS 1000

///////////////////////////////////////////////////////////////////////////////
/// Class CCBoxBase
///////////////////////////////////////////////////////////////////////////////
class CCBoxBase : public wxFrame
{
	private:

	protected:
		wxPanel* m_mainPanel;
		CCVirtualListCtrl* m_listCtrl;
		wxToolBar* m_toolBar1;

		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemDeSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnKeyDown( wxListEvent& event ) { event.Skip(); }
		virtual void OnFocus( wxFocusEvent& event ) { event.Skip(); }
		virtual void OnShowPublicItems( wxCommandEvent& event ) { event.Skip(); }


	public:

		CCBoxBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxFRAME_FLOAT_ON_PARENT|wxSTAY_ON_TOP|wxFRAME_TOOL_WINDOW|wxRAISED_BORDER|wxTAB_TRAVERSAL );
		~CCBoxBase();

};

#endif //__cc_boxbase__
