//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include <wx/dcmemory.h>
#include <wx/settings.h>

CLMainAuiTBArt::CLMainAuiTBArt(bool isMainBook)
    : m_isMainBook(isMainBook)
{
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CLMainAuiTBArt::OnThemeChanged), NULL,
                                  this);
}

CLMainAuiTBArt::~CLMainAuiTBArt()
{
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(CLMainAuiTBArt::OnThemeChanged),
                                     NULL, this);
}

void CLMainAuiTBArt::DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
    wxBitmap bmp(rect.GetSize());
    wxMemoryDC dcMem;
    dcMem.SelectObject(bmp);
    dcMem.SetPen(DrawingUtils::GetPanelBgColour());
    dcMem.SetBrush(DrawingUtils::GetPanelBgColour());
    dcMem.DrawRectangle(rect);
    dcMem.SelectObject(wxNullBitmap);
    dc.DrawBitmap(bmp, wxPoint(0, 0));
}

void CLMainAuiTBArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) { DrawPlainBackground(dc, wnd, rect); }

void CLMainAuiTBArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
#ifdef __WXGTK3__
    if(!item.GetBitmap().IsOk()) { return; }
#endif
    wxAuiDefaultToolBarArt::DrawButton(dc, wnd, item, rect);
}

void CLMainAuiTBArt::DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
#ifdef __WXGTK3__
    if(!item.GetBitmap().IsOk()) { return; }
#endif
    wxAuiDefaultToolBarArt::DrawDropDownButton(dc, wnd, item, rect);
}

void CLMainAuiTBArt::OnThemeChanged(wxCommandEvent& event) { event.Skip(); }

void CLMainAuiTBArt::DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(dc);
    wxUnusedVar(wnd);
    wxUnusedVar(rect);
}
