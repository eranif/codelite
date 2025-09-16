//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_aui_tb_are.h
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

#ifndef CL_AUI_TOOLBAR_ART_H
#define CL_AUI_TOOLBAR_ART_H

#include "cl_defs.h"
#include "codelite_exports.h"
#include "drawingutils.h"

#include <wx/aui/auibar.h>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/pen.h>
#include <wx/sizer.h>

class WXDLLIMPEXP_SDK CLMainAuiTBArt : public wxAuiDefaultToolBarArt, public wxEvtHandler
{
    bool m_isMainBook;

public:
    CLMainAuiTBArt(bool isMainBook = false);
    virtual ~CLMainAuiTBArt();
    wxAuiToolBarArt* Clone() { return new CLMainAuiTBArt(); }
    virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);
    virtual void DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);
    virtual void DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect);
    virtual void DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect);
    virtual void DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect);
    void OnThemeChanged(wxCommandEvent& event);
};

#endif // CL_AUI_TOOLBAR_ART_H
