//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cl_aui_tb_are.cpp
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

#include "cl_aui_tb_are.h"
#include <wx/settings.h>
#include "editor_config.h"

#if USE_AUI_TOOLBAR

CLMainAuiTBArt::CLMainAuiTBArt()
{
}

CLMainAuiTBArt::~CLMainAuiTBArt()
{
}

void CLMainAuiTBArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    wxColour borderUp, borderDown;

    // Determine the pen colour
    if ( !DrawingUtils::IsDark(bgColour)) {
        wxAuiDefaultToolBarArt::DrawBackground(dc, wnd, rect);
        return;
    }
    
    // Dark theme
    borderUp   = bgColour;
    borderDown = bgColour;
    wxColour bgColour2 = bgColour;
    dc.SetPen(bgColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(rect);
    dc.GradientFillLinear(rect, bgColour2, bgColour, wxSOUTH);
    
    dc.SetPen(borderUp);
    dc.DrawLine(rect.GetLeftBottom(), rect.GetLeftTop());
    dc.DrawLine(rect.GetTopLeft(), rect.GetTopRight());
    
    dc.SetPen(borderDown);
    dc.DrawLine(rect.GetTopRight(), rect.GetBottomRight());
    dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
}
#endif
