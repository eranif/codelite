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

#include "ColoursAndFontsManager.h"
#include "clNotebookTheme.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "ieditor.h"
#include "lexer_configuration.h"
#include "plugin.h"
#include "plugin_general_wxcp.h"
#include <editor_config.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

#define Y_PADDING_BASE 3
#ifdef __WXMSW__
#define X_PADDING 12
#else
#define X_PADDING 10
#endif

#ifdef __WXMAC__
#include <wx/osx/private.h>
#endif

static int x_button_size = 12;
static int Y_PADDING = Y_PADDING_BASE;

clAuiMainNotebookTabArt::clAuiMainNotebookTabArt()
    : m_tabRadius(0.0)
{
    RefreshColours(0);
}

clAuiMainNotebookTabArt::~clAuiMainNotebookTabArt() {}

void clAuiMainNotebookTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
    dc.SetPen(m_bgColour);
    dc.SetBrush(m_bgColour);
    dc.DrawRectangle(rect);
    m_windowListButtonRect = wxRect();
}

#define DRAW_LINE(__p1, __p2) \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);  \
    dc.DrawLine(__p1, __p2);

void clAuiMainNotebookTabArt::DrawTab(wxDC& wxdc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect,
                                      int close_button_state, wxRect* out_tab_rect, wxRect* out_button_rect,
                                      int* x_extent)
{
#ifdef __WXMSW__
    wxGCDC dc;
    DrawingUtils::GetGCDC(wxdc, dc);
#else
    wxDC& dc = wxdc;
#endif

    if(in_rect.GetHeight() == 0) { return; } // Tabs are not visible
    int curx = 0;

    wxColour penColour = page.active ? m_activeTabPenColour : m_penColour;
    wxColour bgColour = page.active ? m_activeTabBgColour : m_tabBgColour;

    wxSize sz = GetTabSize(dc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent);
    if(sz.GetHeight() < in_rect.GetHeight()) { sz.SetHeight(in_rect.GetHeight()); }

    wxRect rr(in_rect.GetTopLeft(), sz);

    // AUI tab does not really "touching" the bottom rect
    // so we need to change the offsets a bit
    rr.y += 2;

    /// the tab start position (x)
    curx = rr.x + X_PADDING;

    // Set clipping region
    bool tabTruncated = false;
    if(!m_windowListButtonRect.IsEmpty() && (rr.GetTopRight().x >= m_windowListButtonRect.GetX())) {
        rr.SetWidth(rr.GetWidth() - (rr.GetTopRight().x - m_windowListButtonRect.GetX()));
        if(rr.GetWidth() < 0) { rr.SetWidth(0); }
        tabTruncated = true;
    }

    dc.SetClippingRegion(rr);
    dc.SetPen(penColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(rr);
    if(tabTruncated) {
        // Erase the right side line
        dc.SetPen(bgColour);
        dc.DrawLine(rr.GetTopRight(), rr.GetBottomRight());
        // Restore the pen
        dc.SetPen(penColour);
    }
    if(!page.active) {
        // Draw 2 lines at the bottom rect
        // one with the background colour of the active tab and the second
        // with the active tab pen colour
        wxPoint p1, p2;
        p1 = rr.GetBottomLeft();
        p2 = rr.GetBottomRight();

        // We reduce 2 pixels that we added earlier (see ~20 line above)
        p1.x += 1;
        p1.y -= 2;
        p2.x -= 1;
        p2.y -= 2;

        dc.SetPen(m_penColour);
        dc.DrawLine(p1, p2);
    } else {
        wxPoint p1, p2;
        p1 = rr.GetBottomLeft();
        p2 = rr.GetBottomRight();

        dc.SetPen(m_activeTabBgColour);
        for(int i = 0; i < 3; ++i) {
            DRAW_LINE(p1, p2);
            p1.y += 1;
            p2.y += 1;
        }

        // Mark the active tab
        p1 = rr.GetTopRight();
        p2 = rr.GetTopLeft();

        // Since the pen width is 3, start the drawing
        // One pixel below
        p1.y += 1;
        p2.y += 1;
        dc.SetPen(wxPen(m_markerColour, 3));
        dc.DrawLine(p1, p2);
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
        eButtonState btnState = eButtonState::kNormal;
        switch(close_button_state) {
        case wxAUI_BUTTON_STATE_HOVER:
            btnState = eButtonState::kHover;
            break;
        case wxAUI_BUTTON_STATE_PRESSED:
            btnState = eButtonState::kPressed;
            break;
        }
        int btny = (rr.y + (rr.height - x_button_size) / 2);
        btny += 2; // We add 2 more pixels here cause of the marker line
        
        wxRect xRect = wxRect(curx, btny, x_button_size, x_button_size);
        DrawingUtils::DrawButtonX(dc, wnd, xRect, m_markerColour, btnState);
        *out_button_rect = xRect;
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
    
    // if there's a bitmap, add space for it
    if(bitmap.IsOk()) {
        tab_width += bitmap.GetWidth();
        tab_width += X_PADDING; // right side bitmap padding
    }
    
    tab_width += measured_textx;
    tab_width += X_PADDING;
    
    // if the close button is showing, add space for it
    if(close_button_state != wxAUI_BUTTON_STATE_HIDDEN) { tab_width += x_button_size + X_PADDING; }

    if(m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
        tab_width = 100;
    }
    *x_extent = tab_width;
    return wxSize(tab_width, tab_height);
}

void clAuiMainNotebookTabArt::RefreshColours(long style)
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

    // Inactive tab colours
    m_tabTextColour = textColour;
    m_tabBgColour = faceColour.ChangeLightness(95);
    m_penColour = faceColour.ChangeLightness(85);
    m_markerColour = DrawingUtils::GetCaptionColour();
    m_bgColour = faceColour;

    if(style & kNotebook_DynamicColours) {
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
        if(lexer) {
            m_activeTabBgColour = lexer->GetProperty(0).GetBgColour();
            m_activeTabPenColour = m_activeTabBgColour;
        }
    }
    if(DrawingUtils::IsDark(m_activeTabBgColour)) { m_activeTabTextColour = *wxWHITE; }
    // Update the tab height based on the user settings
    Y_PADDING = Y_PADDING_BASE + EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight();
}

static int dropdown_button_size = 20;
void clAuiMainNotebookTabArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& in_rect, int bitmap_id,
                                         int button_state, int orientation, wxRect* out_rect)
{
    if(bitmap_id == wxAUI_BUTTON_WINDOWLIST) {
        m_windowListButtonRect = wxRect((in_rect.x + in_rect.GetWidth() - dropdown_button_size),
                                        in_rect.y + ((in_rect.height - dropdown_button_size) / 2), dropdown_button_size,
                                        dropdown_button_size);
        DrawingUtils::DrawDropDownArrow(wnd, dc, m_windowListButtonRect, m_markerColour);
        *out_rect = m_windowListButtonRect;
    } else {
        wxAuiDefaultTabArt::DrawButton(dc, wnd, in_rect, bitmap_id, button_state, orientation, out_rect);
    }
}
