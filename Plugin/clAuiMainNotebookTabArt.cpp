//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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

#include "editor_config.h"
#include <wx/dcgraph.h>
#include "plugin_general_wxcp.h"
#include <wx/dcmemory.h>
#include <editor_config.h>
#include "globals.h"
#include "cl_command_event.h"
#include "plugin.h"
#include "event_notifier.h"
#include <wx/stc/stc.h>
#include "ieditor.h"
#include "clNotebookTheme.h"
#include <wx/xrc/xmlres.h>

static const wxDouble X_RADIUS = 6.0;
static const wxDouble X_DIAMETER = 2 * X_RADIUS;

#ifdef __WXMAC__
#include <wx/osx/private.h>
#define TAB_HEIGHT_SPACER 10
#define TAB_Y_OFFSET 2
#define TEXT_Y_SPACER 0
#define BMP_Y_SPACER 0
#elif defined(__WXMSW__)
#define TAB_HEIGHT_SPACER 10
#define TAB_Y_OFFSET 3
#define TEXT_Y_SPACER 0
#define BMP_Y_SPACER 0
#else // GTK/FreeBSD
#define TAB_Y_OFFSET 4
#define TEXT_Y_SPACER 2
#define BMP_Y_SPACER 2
#endif

static int x_button_height = 16;
clAuiMainNotebookTabArt::clAuiMainNotebookTabArt(IManager* manager)
    : m_manager(manager)
    , m_tabRadius(0.0)
{
#ifdef __WXMSW__
    m_tabRadius = 0.0;
#elif defined(__WXGTK__)
    m_tabRadius = 0.0;
#else
    m_tabRadius = 0.0;
#endif

    // Default buttons
    m_bmpClose = wxXmlResource::Get()->LoadBitmap("tab_x_close");
    m_bmpCloseHover = wxXmlResource::Get()->LoadBitmap("tab_x_close_hover");
    m_bmpClosePressed = wxXmlResource::Get()->LoadBitmap("tab_x_close_pressed");

    x_button_height = m_bmpClose.GetHeight();
}

clAuiMainNotebookTabArt::~clAuiMainNotebookTabArt() {}

void clAuiMainNotebookTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
#ifdef __WXGTK__
    wxDC& gdc = dc;
#else
    wxGCDC gdc;
    if(!DrawingUtils::GetGCDC(dc, gdc)) return;
#endif

    DoSetColours();
    gdc.SetPen(m_activeTabPenColour);
    wxBrush brush = DrawingUtils::GetStippleBrush();
    brush.SetColour(m_bgColour);
    gdc.SetBrush(brush);
    gdc.GradientFillLinear(rect, m_bgColour, m_bgColour, wxSOUTH);

    wxPoint ptBottomLeft = rect.GetBottomLeft();
    wxPoint ptBottomRight = rect.GetBottomRight();
    gdc.DrawLine(ptBottomLeft, ptBottomRight);
}

void clAuiMainNotebookTabArt::DrawTab(wxDC& dc,
                                      wxWindow* wnd,
                                      const wxAuiNotebookPage& page,
                                      const wxRect& in_rect,
                                      int close_button_state,
                                      wxRect* out_tab_rect,
                                      wxRect* out_button_rect,
                                      int* x_extent)
{
    if(in_rect.GetHeight() == 0) return; // Tabs are not visible
    int curx = 0;
    wxGCDC gdc;
    if(!DrawingUtils::GetGCDC(dc, gdc)) return;

    wxColour penColour = page.active ? m_activeTabPenColour : m_penColour;
    wxGraphicsPath path = gdc.GetGraphicsContext()->CreatePath();
    gdc.SetPen(penColour);

    wxSize sz = GetTabSize(gdc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent);

    wxRect rr(in_rect.GetTopLeft(), sz);
    rr.y += TAB_Y_OFFSET;
    rr.width -= 1;

#ifndef __WXMAC__
    if(page.active) {
        rr.y -= 2;
        rr.height += 2;
    }
#endif

#ifndef __WXGTK__
    rr.height += 4;
#endif

    /// the tab start position (x)
    curx = rr.x + 7;

    // Set clipping region
    int clip_width = rr.width;
    if(rr.x + clip_width > in_rect.x + in_rect.width) clip_width = (in_rect.x + in_rect.width) - rr.x;

    gdc.SetClippingRegion(rr.x, rr.y, clip_width, rr.height);
    gdc.SetBrush(m_bgColour);
    gdc.SetPen(penColour);
    
    if(page.active) {
        gdc.SetBrush(m_activeTabBgColour);
        path.AddRoundedRectangle(rr.x, rr.y, rr.width - 1, rr.height, m_tabRadius);
        gdc.GetGraphicsContext()->FillPath(path);
        gdc.GetGraphicsContext()->StrokePath(path);

    } else {
        wxGraphicsPath outerPath = gdc.GetGraphicsContext()->CreatePath();
        gdc.SetPen(penColour);
        outerPath.AddRoundedRectangle(rr.x, rr.y, rr.width - 1, rr.height, m_tabRadius);
        gdc.GetGraphicsContext()->StrokePath(outerPath);

        gdc.SetPen(m_innerPenColour);
        path.AddRoundedRectangle(rr.x + 1, rr.y + 1, rr.width - 3, rr.height - 1, m_tabRadius);
        gdc.GetGraphicsContext()->StrokePath(path);

        gdc.SetBrush(m_tabBgColour);
        gdc.GetGraphicsContext()->StrokePath(path);
        gdc.GetGraphicsContext()->FillPath(path);
        gdc.SetPen(penColour);
    }

    if(!page.active) {
        // Draw a line at the bottom rect
        gdc.SetPen(m_activeTabPenColour);
        gdc.DrawLine(in_rect.GetBottomLeft(), in_rect.GetBottomRight());
    }

    wxString caption = page.caption;
    if(caption.IsEmpty()) {
        caption = "Tp";
    }

    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    gdc.SetFont(fnt);
    wxSize ext = gdc.GetTextExtent(caption);
    if(caption == "Tp") caption.Clear();

    /// Draw the bitmap
    if(page.bitmap.IsOk()) {
        int bmpy = (rr.y + (rr.height - page.bitmap.GetHeight()) / 2) - TAB_Y_OFFSET + BMP_Y_SPACER;
        gdc.GetGraphicsContext()->DrawBitmap(page.bitmap, curx, bmpy, page.bitmap.GetWidth(), page.bitmap.GetHeight());
        curx += page.bitmap.GetWidth();
        curx += 3;
    }

    /// Draw the text
    wxColour textColour = page.active ? m_activeTabTextColour : m_tabTextColour;
    gdc.SetTextForeground(textColour);
    wxDouble textYOffCorrd = (rr.y + (rr.height - ext.y) / 2) - TAB_Y_OFFSET + TEXT_Y_SPACER;
    gdc.GetGraphicsContext()->DrawText(page.caption, curx, textYOffCorrd);

    // advance the X offset
    curx += ext.x;
    curx += 3;

    /// Draw the X button on the tab
    if(close_button_state != wxAUI_BUTTON_STATE_HIDDEN) {
        curx += 1;
        wxBitmap xBmp = m_bmpClose;
        switch(close_button_state) {
        case wxAUI_BUTTON_STATE_HOVER:
            xBmp = m_bmpCloseHover;
            break;
        case wxAUI_BUTTON_STATE_PRESSED:
            xBmp = m_bmpClosePressed;
            break;
        }
        int btny = (rr.y + (rr.height - x_button_height) / 2) - TAB_Y_OFFSET + BMP_Y_SPACER;
        gdc.GetGraphicsContext()->DrawBitmap(xBmp, curx, btny, x_button_height, x_button_height);
        *out_button_rect = wxRect(curx, btny, x_button_height, x_button_height);
        curx += x_button_height;
    }

    *out_tab_rect = rr;
    gdc.DestroyClippingRegion();
}

wxSize clAuiMainNotebookTabArt::GetTabSize(wxDC& dc,
                                           wxWindow* WXUNUSED(wnd),
                                           const wxString& caption,
                                           const wxBitmap& bitmap,
                                           bool active,
                                           int close_button_state,
                                           int* x_extent)
{
    static wxCoord measured_texty(wxNOT_FOUND);

    wxCoord measured_textx;
    wxCoord tmp;

    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    // if(active) {
    //     f.SetWeight(wxFONTWEIGHT_BOLD);
    // }

    dc.SetFont(f);
    dc.GetTextExtent(caption, &measured_textx, &tmp);

    // do it once
    if(measured_texty == wxNOT_FOUND) dc.GetTextExtent(wxT("ABCDEFXj"), &tmp, &measured_texty);

    // add padding around the text
    wxCoord tab_width = measured_textx;
    wxCoord tab_height = measured_texty;

#ifdef __WXGTK__
    tab_height = TAB_CTRL_HEIGHT;
#endif

    if(tab_height < 16) tab_height = 16;

    // if the close button is showing, add space for it
    if(close_button_state != wxAUI_BUTTON_STATE_HIDDEN) tab_width += X_DIAMETER + 3;

// if there's a bitmap, add space for it

// NOTE: we only support 16 pixels bitmap (or smaller)
// so there is no need to adjust the tab height!
#ifndef __WXGTK__
    tab_height += TAB_HEIGHT_SPACER;
#endif

    if(bitmap.IsOk()) {
        tab_width += bitmap.GetWidth();
        tab_width += 3; // right side bitmap padding
    }

// add padding
#ifdef __WXMAC__
    tab_width += 16;
#else
    tab_width += 16;
#endif

    if(m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
        tab_width = 80;
    }

    *x_extent = tab_width;

    return wxSize(tab_width, tab_height);
}

void clAuiMainNotebookTabArt::DoSetColours()
{
    // Set the colours
    // based on the selected book theme
    if(!m_bgColour.IsOk()) {
        DoInitializeColoursFromTheme();
    }

    // If we have an active editor, update the colours, if not - keep the old ones
    IEditor* editor = m_manager->GetActiveEditor();

    // Default buttons
    m_bmpClose = wxXmlResource::Get()->LoadBitmap("tab_x_close");
    m_bmpCloseHover = wxXmlResource::Get()->LoadBitmap("tab_x_close_hover");
    m_bmpClosePressed = wxXmlResource::Get()->LoadBitmap("tab_x_close_pressed");

    // We use the colour theme based on the active editor
    if(editor) {
        // Change lightness ranges between 0-200
        // 0 would be completely black, 200 completely white an ialpha of 100 returns the same colour.
        m_activeTabBgColour = editor->GetSTC()->StyleGetBackground(0);
        if(DrawingUtils::IsDark(m_activeTabBgColour)) {

            // Adjust the button colours
            m_bmpClose = wxXmlResource::Get()->LoadBitmap("tab_x_close_dark");
            m_bmpCloseHover = wxXmlResource::Get()->LoadBitmap("tab_x_close_dark_hover");
            m_bmpClosePressed = wxXmlResource::Get()->LoadBitmap("tab_x_close_dark_pressed");

            // adjust some colours
            m_activeTabTextColour = *wxWHITE;
            m_tabTextColour = m_activeTabTextColour.ChangeLightness(70);
            m_activeTabPenColour = m_activeTabBgColour.ChangeLightness(80);
            m_tabBgColour = m_activeTabBgColour.ChangeLightness(110);
#ifdef __WXMAC__
            m_bgColour = m_activeTabBgColour.ChangeLightness(150);
#else
            m_bgColour = m_activeTabBgColour.ChangeLightness(130);
#endif
            m_penColour = m_activeTabPenColour.ChangeLightness(110);
            m_innerPenColour = m_penColour.ChangeLightness(115);

        } else {
            wxColour tmp = m_activeTabBgColour;
            DoInitializeColoursFromTheme();
            m_activeTabBgColour = tmp;
            m_activeTabTextColour = *wxBLACK;
            m_tabTextColour = m_activeTabTextColour.ChangeLightness(130);
#ifdef __WXOSX__
            // use a bit darker colour on OSX
            m_tabBgColour = m_tabBgColour.ChangeLightness(90);
            m_innerPenColour = m_tabBgColour.ChangeLightness(110);
            m_penColour = m_innerPenColour.ChangeLightness(90);
#else
            m_tabBgColour = m_tabBgColour.ChangeLightness(120);
            m_penColour = m_innerPenColour.ChangeLightness(90);
#endif
        }
    } else {
        m_activeTabBgColour = DrawingUtils::GetPanelBgColour();
        m_activeTabTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    }
}

void clAuiMainNotebookTabArt::DoInitializeColoursFromTheme()
{
    clNotebookTheme theme = clNotebookTheme::GetTheme(clNotebookTheme::kDefault);
    m_activeTabPenColour = theme.GetActiveTabPenColour();
    m_tabBgColour = theme.GetTabBgColour().ChangeLightness(150);
    m_penColour = m_activeTabPenColour.ChangeLightness(130);
    m_innerPenColour = m_tabBgColour.ChangeLightness(150);
    m_bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    m_activeTabTextColour = theme.GetActiveTabTextColour();
    m_tabTextColour = theme.GetTabTextColour();
    m_activeTabBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
}
