///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cc_boxbase__
#define __cc_boxbase__

#include <wx/listctrl.h>
#include "ccvirtuallistctrl.h"
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define TOOL_SHOW_PRIVATE_MEMBERS 1000
#define TOOL_SHOW_ITEM_COMMENTS 1001

#ifdef __WXMAC__
#    define CC_USES_POPUPWIN  0
#else
#    define CC_USES_POPUPWIN  1
#endif

#if CC_USES_POPUPWIN
#ifdef __WXGTK__
#    define CCBoxParent wxPopupWindow
#    define CC_STYLE    wxBORDER_SIMPLE
#else
#    define CCBoxParent wxPopupTransientWindow
#    define CC_STYLE    wxBORDER_SIMPLE
#endif
#    include <wx/popupwin.h>
#else
#    define CCBoxParent wxPanel
#endif

#define BOX_HEIGHT 250
#define BOX_WIDTH  250

///////////////////////////////////////////////////////////////////////////////
/// Class CCBoxBase
///////////////////////////////////////////////////////////////////////////////
class CCBoxBase : public CCBoxParent
{
private:

protected:
	CCVirtualListCtrl* m_listCtrl;
	wxToolBar*         m_toolBar1;
	wxPanel*           m_mainPanel;

	// Virtual event handlers, overide them in your derived class
	virtual void OnItemActivated( wxListEvent& event ) {
		event.Skip();
	}
	virtual void OnItemDeSelected( wxListEvent& event ) {
		event.Skip();
	}
	virtual void OnItemSelected( wxListEvent& event ) {
		event.Skip();
	}
	virtual void OnKeyDown( wxListEvent& event ) {
		event.Skip();
	}
	virtual void OnShowPublicItems( wxCommandEvent& event ) {
		event.Skip();
	}
	virtual void OnShowComments( wxCommandEvent& event ) {
		event.Skip();
	}


public:

	CCBoxBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 486,300 ), long style = wxRAISED_BORDER|wxTAB_TRAVERSAL );
	~CCBoxBase();

};

#endif //__cc_boxbase__
