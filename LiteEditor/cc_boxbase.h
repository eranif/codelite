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
#include <wx/pen.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/popupwin.h>

///////////////////////////////////////////////////////////////////////////

#define BOX_HEIGHT 250
#define BOX_WIDTH  400

///////////////////////////////////////////////////////////////////////////////
/// Class CCBoxBase
///////////////////////////////////////////////////////////////////////////////
class CCBoxBase : public wxPopupWindow
{
protected:
    CCVirtualListCtrl* m_listCtrl;
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

public:
    CCBoxBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 486,300 ), long style = wxRAISED_BORDER|wxTAB_TRAVERSAL );
    ~CCBoxBase();
};

#endif //__cc_boxbase__
