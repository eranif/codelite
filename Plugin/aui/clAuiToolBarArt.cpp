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

#include "clAuiToolBarArt.h"

#include "clSystemSettings.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"

#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>

clAuiToolBarArt::clAuiToolBarArt()
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clAuiToolBarArt::OnThemeChanged, this);
}

clAuiToolBarArt::~clAuiToolBarArt()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clAuiToolBarArt::OnThemeChanged, this);
}

void clAuiToolBarArt::DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
    wxBitmap bmp(rect.GetSize());
    wxMemoryDC dcMem;
    dcMem.SelectObject(bmp);
    dcMem.SetPen(clSystemSettings::GetDefaultPanelColour());
    dcMem.SetBrush(clSystemSettings::GetDefaultPanelColour());
    dcMem.DrawRectangle(rect);
    dcMem.SelectObject(wxNullBitmap);
    dc.DrawBitmap(bmp, wxPoint(0, 0));
}

void clAuiToolBarArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    DrawPlainBackground(dc, wnd, rect);
}

void clAuiToolBarArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
    wxAuiDefaultToolBarArt::DrawButton(dc, wnd, item, rect);
}

void clAuiToolBarArt::DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect)
{
    wxAuiDefaultToolBarArt::DrawDropDownButton(dc, wnd, item, rect);
}

void clAuiToolBarArt::OnThemeChanged(wxCommandEvent& event) { event.Skip(); }

void clAuiToolBarArt::DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxAuiDefaultToolBarArt::DrawGripper(dc, wnd, rect);
}

void clAuiToolBarArt::AddTool(wxAuiToolBar* toolbar,
                              int toolId,
                              const wxString& label,
                              const wxBitmap& bitmap,
                              const wxString& shortHelpString,
                              wxItemKind item_kind)
{
    size_t brightness = DrawingUtils::IsThemeDark() ? 0 : 255;
    auto disabled_bmp = bitmap.ConvertToDisabled(brightness);
    wxBitmap::Rescale(disabled_bmp, wxSize(16, 16));
    wxString help_string = shortHelpString.empty() ? label : shortHelpString;
    toolbar->AddTool(toolId, label, bitmap, disabled_bmp, item_kind, help_string, help_string, nullptr);
}

void clAuiToolBarArt::Finalise(wxAuiToolBar* toolbar) { wxUnusedVar(toolbar); }
