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

#pragma once

#include "cl_defs.h"
#include "codelite_exports.h"
#include "drawingutils.h"

#include <wx/aui/auibar.h>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/pen.h>
#include <wx/sizer.h>

class WXDLLIMPEXP_SDK clAuiToolBarArt : public wxAuiDefaultToolBarArt
{

public:
    clAuiToolBarArt();
    ~clAuiToolBarArt() override;
    wxAuiToolBarArt* Clone() override { return new clAuiToolBarArt(); }
    void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;
    void DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;
    void DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect) override;
    void DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect) override;
    void DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;
    void OnThemeChanged(wxCommandEvent& event);
    static void Finalise(wxAuiToolBar* toobar);
    static void AddTool(wxAuiToolBar* toolbar,
                        int toolId,
                        const wxString& label,
                        const wxBitmap& bitmap,
                        const wxString& shortHelpString = wxEmptyString,
                        wxItemKind item_kind = wxITEM_NORMAL);
};
