//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_aui_notebook_art.cpp
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

#include "clAuiMainNotebookTabArt.h"

#include "clNotebookTheme.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "ieditor.h"
#include "plugin.h"
#include "plugin_general_wxcp.h"
#include <editor_config.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

#define Y_PADDING 8
#define X_PADDING 5

#ifdef __WXMAC__
#include <wx/osx/private.h>
#endif

static int x_button_size = 16;
clAuiMainNotebookTabArt::clAuiMainNotebookTabArt(long style)
    : m_style(style)
    , m_tabRadius(0.0)
{
    m_tabRadius = 0.0;

    // Default buttons
    m_bmpClose = wxXmlResource::Get()->LoadBitmap("tab_x_close");
    m_bmpCloseHover = wxXmlResource::Get()->LoadBitmap("tab_x_close_hover");
    m_bmpClosePressed = wxXmlResource::Get()->LoadBitmap("tab_x_close_pressed");

    x_button_size = m_bmpClose.GetScaledWidth();
    RefreshColours();
}

clAuiMainNotebookTabArt::~clAuiMainNotebookTabArt() {}

void clAuiMainNotebookTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
    dc.SetPen(m_bgColour);
    dc.SetBrush(m_bgColour);
    dc.DrawRectangle(rect);
}

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

void clAuiMainNotebookTabArt::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect,
                                      int close_button_state, wxRect* out_tab_rect, wxRect* out_button_rect,
                                      int* x_extent)
{
    if(in_rect.GetHeight() == 0) { return; } // Tabs are not visible
    int curx = 0;

    wxColour penColour = page.active ? m_activeTabPenColour : m_penColour;
    dc.SetPen(penColour);

    wxSize sz = GetTabSize(dc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent);

    wxRect rr(in_rect.GetTopLeft(), sz);
    
    
    // AUI tab does not really "touching" the bottom rect
    // so we need to change the offsets a bit
    rr.y += 2;
    
    /// the tab start position (x)
    curx = rr.x + X_PADDING;

    // Set clipping region
    int clip_width = rr.width;
    if(rr.x + clip_width > in_rect.x + in_rect.width) { clip_width = (in_rect.x + in_rect.width) - rr.x; }

    dc.SetClippingRegion(rr);
    dc.SetBrush(m_bgColour);

    if(page.active) {
        dc.SetBrush(m_activeTabBgColour);
        dc.DrawRectangle(rr);

    } else {
        dc.SetBrush(m_bgColour);
        dc.DrawRectangle(rr);
    }

    if(!page.active) {
        // Draw 2 lines at the bottom rect
        // one with the background colour of the active tab and the second
        // with the active tab pen colour
        wxPoint p1, p2;
        p1 = in_rect.GetBottomLeft();
        p1.x -= 1;
        p2 = in_rect.GetBottomRight();
        dc.SetPen(m_activeTabBgColour);
        dc.DrawLine(p1, p2);

        p1.y -= 1;
        p2.y -= 1;
        dc.SetPen(m_activeTabPenColour);
        dc.DrawLine(p1, p2);

    } else {
        wxPoint p1, p2;
        p1 = in_rect.GetBottomLeft();
        p2 = in_rect.GetBottomRight();

        dc.SetPen(m_activeTabBgColour);
        for(int i = 0; i < 3; ++i) {
            DRAW_LINE(p1, p2);
            p1.y += 1;
            p2.y += 1;
        }
    }

    wxString caption = page.caption;
    if(caption.IsEmpty()) { caption = "Tp"; }

    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    dc.SetFont(fnt);
    wxSize ext = dc.GetTextExtent(caption);
    if(caption == "Tp") { caption.Clear(); }

    /// Draw the bitmap
    if(page.bitmap.IsOk()) {
        int bmpy = (rr.y + (rr.height - page.bitmap.GetScaledHeight()) / 2);
        dc.DrawBitmap(page.bitmap, curx, bmpy);
        curx += page.bitmap.GetScaledWidth();
        curx += X_PADDING;
    }

    /// Draw the text
    wxColour textColour = page.active ? m_activeTabTextColour : m_tabTextColour;
    dc.SetTextForeground(textColour);
    wxDouble textYOffCorrd = (rr.y + (rr.height - ext.y) / 2);
    dc.DrawText(page.caption, curx, textYOffCorrd);

    // advance the X offset
    curx += ext.x;
    curx += X_PADDING;

    /// Draw the X button on the tab
    if(close_button_state != wxAUI_BUTTON_STATE_HIDDEN) {
        curx += X_PADDING;
        wxBitmap xBmp = m_bmpClose;
        switch(close_button_state) {
        case wxAUI_BUTTON_STATE_HOVER:
            xBmp = m_bmpCloseHover;
            break;
        case wxAUI_BUTTON_STATE_PRESSED:
            xBmp = m_bmpClosePressed;
            break;
        }
        int btny = (rr.y + (rr.height - x_button_size) / 2);
        dc.DrawBitmap(xBmp, curx, btny);
        *out_button_rect = wxRect(curx, btny, x_button_size, x_button_size);
        curx += x_button_size;
        curx += X_PADDING;
    }

    *out_tab_rect = rr;
    dc.DestroyClippingRegion();
}

wxSize clAuiMainNotebookTabArt::GetTabSize(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxString& caption,
                                           const wxBitmap& bitmap, bool active, int close_button_state, int* x_extent)
{
    wxCoord measured_textx;
    wxCoord measured_texty;

    // +------------------+
    // |.[bmp].[text].[x].|
    // +------------------+

    wxFont f = DrawingUtils::GetDefaultGuiFont();
    dc.SetFont(f);
    dc.GetTextExtent(caption, &measured_textx, &measured_texty);

    // add padding around the text
    wxCoord tab_width = X_PADDING;
    wxCoord tab_height = measured_texty;
    tab_height += (2 * Y_PADDING);

    // if the close button is showing, add space for it
    if(close_button_state != wxAUI_BUTTON_STATE_HIDDEN) { tab_width += x_button_size + X_PADDING; }

    tab_width += measured_textx;
    tab_width += X_PADDING;

    // if there's a bitmap, add space for it
    if(bitmap.IsOk()) {
        tab_width += bitmap.GetWidth();
        tab_width += X_PADDING; // right side bitmap padding
    }

    if(m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
        tab_width = 100;
    } else {
        tab_width += X_PADDING;
    }
    *x_extent = tab_width;
    return wxSize(tab_width, tab_height);
}

void clAuiMainNotebookTabArt::RefreshColours()
{
    // Base colours
    wxColour faceColour = DrawingUtils::GetPanelBgColour();
    wxColour textColour = DrawingUtils::GetPanelTextColour();

    // Active tab colours
    m_activeTabTextColour = textColour;
    if(DrawingUtils::IsDark(faceColour)) {
        // Make the active tab darker
        m_activeTabBgColour = faceColour.ChangeLightness(60);
        m_activeTabPenColour = m_activeTabBgColour;

    } else {
        // Make it lighter
        m_activeTabBgColour = faceColour.ChangeLightness(150);
        m_activeTabPenColour = faceColour.ChangeLightness(70);
    }

    if(DrawingUtils::IsDark(m_activeTabBgColour)) { m_activeTabTextColour = *wxWHITE; }

    // Inactive tab colours
    m_tabTextColour = textColour;
    m_tabBgColour = faceColour.ChangeLightness(95);
    m_penColour = faceColour.ChangeLightness(85);
    m_markerColour = DrawingUtils::GetCaptionColour();
    m_bgColour = faceColour;
}

clAuiNotebookArtHelper::clAuiNotebookArtHelper(clAuiMainNotebookTabArt* art)
    : m_art(art)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clAuiNotebookArtHelper::OnThemeChanged, this);
}

clAuiNotebookArtHelper::~clAuiNotebookArtHelper()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clAuiNotebookArtHelper::OnThemeChanged, this);
}
void clAuiNotebookArtHelper::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    m_art->RefreshColours();
}
