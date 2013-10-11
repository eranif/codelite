//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cc_boxbase.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
#define BOX_WIDTH  600

#if defined(__WXMSW__)
#   define CCBOX_USE_POPUP 1
#elif defined(__WXMAC__)
#   define CCBOX_USE_POPUP 0
#elif defined(__WXGTK__)
#   define CCBOX_USE_POPUP 1
#endif

#if CCBOX_USE_POPUP
#   define CCBoxParent wxPopupWindow
#else
#   define CCBoxParent wxPanel
#endif

///////////////////////////////////////////////////////////////////////////////
/// Class CCBoxBase
///////////////////////////////////////////////////////////////////////////////
class CCBoxBase : public CCBoxParent
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
    CCBoxBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 486,300 ), long style = wxBORDER_NONE|wxTAB_TRAVERSAL );
    ~CCBoxBase();
};

#endif //__cc_boxbase__
