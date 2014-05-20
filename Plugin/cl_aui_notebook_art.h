//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_aui_notebook_art.h
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

#ifndef CLAUI_TAB_ART_H
#define CLAUI_TAB_ART_H

#include <wx/aui/auibook.h>
#include <wx/pen.h>
#include "codelite_exports.h"
#include <wx/dcgraph.h>
#include <wx/colour.h>

// -- GUI helper classes and functions --

class WXDLLIMPEXP_SDK clAuiGlossyTabArt : public wxAuiDefaultTabArt
{
    wxColour m_bgColour;
    wxColour m_penColour;
    
protected:
    void DoDrawInactiveTabSeparator(wxGCDC& gdc, const wxRect& tabRect);
    void DoGetTabAreaBackgroundColour(wxColour& bgColour, wxColour& penColour);
    
public:
    clAuiGlossyTabArt();
    virtual ~clAuiGlossyTabArt();

    virtual wxAuiTabArt* Clone() {
        return new clAuiGlossyTabArt(*this);
    }
    virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) ;
    virtual void DrawTab(wxDC& dc,
                         wxWindow* wnd,
                         const wxAuiNotebookPage& page,
                         const wxRect& in_rect,
                         int close_button_state,
                         wxRect* out_tab_rect,
                         wxRect* out_button_rect,
                         int* x_extent);
    virtual wxSize GetTabSize(wxDC& dc, wxWindow* wnd, const wxString& caption, const wxBitmap& bitmap, bool active, int closeButtonState, int* xExtent) ;
};

#endif // CLAUI_TAB_ART_H
