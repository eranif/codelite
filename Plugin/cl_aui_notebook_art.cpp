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

#include "cl_aui_notebook_art.h"

#include "editor_config.h"
#include <wx/dcgraph.h>
#include "plugin_general_wxcp.h"
#include <wx/dcmemory.h>
#include <editor_config.h>
#include "globals.h"
#include "cl_command_event.h"
#include "plugin.h"
#include "event_notifier.h"

static const wxDouble X_RADIUS = 6.0;
static const wxDouble X_DIAMETER = 2 * X_RADIUS;

#ifdef __WXMAC__
#   include <wx/osx/private.h>
#   define TAB_HEIGHT_SPACER 10
#   define TAB_Y_OFFSET      2
#   define TEXT_Y_SPACER     0
#elif defined(__WXMSW__)
#   define TAB_HEIGHT_SPACER 10
#   define TAB_Y_OFFSET      3
#   define TEXT_Y_SPACER     0
#else // GTK/FreeBSD
#   define TAB_Y_OFFSET      5
#   define TAB_HEIGHT_SPACER TAB_Y_OFFSET + 6
#   define TEXT_Y_SPACER     2
#endif

clAuiGlossyTabArt::clAuiGlossyTabArt()
{
}

clAuiGlossyTabArt::~clAuiGlossyTabArt()
{
}

void clAuiGlossyTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
    wxUnusedVar(wnd);
#ifdef __WXGTK__
    wxDC &gdc = dc;
#else
    wxGCDC gdc;
    if ( !DrawingUtils::GetGCDC(dc, gdc) )
        return;
#endif

    wxColour bgColour, penColour;
    DoGetTabAreaBackgroundColour(bgColour, penColour);
    m_bgColour = bgColour;
    
    // Allow the plugins to override the default colours
    clColourEvent colourEvent( wxEVT_GET_TAB_BORDER_COLOUR );
    if ( EventNotifier::Get()->ProcessEvent( colourEvent ) ) {
        penColour = colourEvent.GetBorderColour();
    }
    
    gdc.SetPen(bgColour);
    gdc.SetBrush( bgColour );
    gdc.GradientFillLinear(rect, bgColour, bgColour, wxSOUTH);
    gdc.SetPen( penColour );
    
    wxPoint ptBottomLeft  = rect.GetBottomLeft();
    wxPoint ptBottomRight = rect.GetBottomRight();
    gdc.DrawLine(ptBottomLeft, ptBottomRight);
}

void clAuiGlossyTabArt::DrawTab(wxDC& dc,
                          wxWindow* wnd,
                          const wxAuiNotebookPage& page,
                          const wxRect& in_rect,
                          int close_button_state,
                          wxRect* out_tab_rect,
                          wxRect* out_button_rect,
                          int* x_extent)
{
    wxColour bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    wxColour penColour;
    wxColour textColour;
    bool isBgColourDark = DrawingUtils::IsDark(bgColour);
    if ( isBgColourDark ) {
        penColour = DrawingUtils::LightColour(bgColour, 4.0);
    } else {
        bgColour = *wxWHITE;
        penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    }
    textColour = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    int curx = 0;
    
    // Allow the plugins to override the default colours
    clColourEvent colourEvent( wxEVT_COLOUR_TAB );
    colourEvent.SetIsActiveTab( page.active );
    colourEvent.SetPage( page.window );
    if ( EventNotifier::Get()->ProcessEvent( colourEvent ) ) {
        bgColour = colourEvent.GetBgColour();
        textColour = colourEvent.GetFgColour();
    }
    
    // Allow the plugins to override the border colour
    wxColour originalPenColour = penColour;
    clColourEvent borderColourEvent( wxEVT_GET_TAB_BORDER_COLOUR );
    if ( EventNotifier::Get()->ProcessEvent( borderColourEvent ) ) {
        penColour = borderColourEvent.GetBorderColour();
    }
    
    wxGCDC gdc;
    if ( !DrawingUtils::GetGCDC(dc, gdc) )
        return;
    
    m_penColour = penColour;
    
    wxGraphicsPath path = gdc.GetGraphicsContext()->CreatePath();
    gdc.SetPen( penColour );
    
    wxSize sz = GetTabSize(gdc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent);
    
    wxRect rr (in_rect.GetTopLeft(), sz);
    rr.y += TAB_Y_OFFSET;
    rr.width -= 1;

#ifndef __WXMAC__
    if ( page.active ) {
        rr.y -= 2;
        rr.height += 2;
    }
#endif

#ifdef __WXGTK__
    rr.height += TAB_HEIGHT_SPACER;
#else
    rr.height += 4;
#endif

    /// the tab start position (x)
    curx = rr.x + 7;
    
    // Set clipping region
    int clip_width = rr.width;
    if (rr.x + clip_width > in_rect.x + in_rect.width)
        clip_width = (in_rect.x + in_rect.width) - rr.x;
    
    // since the above code above doesn't play well with WXDFB or WXCOCOA,
    // we'll just use a rectangle for the clipping region for now --
    gdc.SetClippingRegion(rr.x, rr.y, clip_width, rr.height);
    gdc.SetBrush( bgColour );
    gdc.SetPen( penColour );
    
    if ( page.active ) {
        path.AddRoundedRectangle(rr.x, rr.y, rr.width-1, rr.height, 6.5);
        gdc.GetGraphicsContext()->FillPath( path );
        gdc.GetGraphicsContext()->StrokePath( path );
        
    } else {
        if ( !m_bgColour.IsOk() ) {
            wxColour b, p;
            DoGetTabAreaBackgroundColour(b, p);
            m_bgColour = b;
        }
        DoDrawInactiveTabSeparator(gdc, rr);
    }
    
    if ( !page.active ) {
        // Draw a line at the bottom rect
        gdc.SetPen(penColour);
        gdc.DrawLine(in_rect.GetBottomLeft(), in_rect.GetBottomRight());
        
    }
    
    wxString caption = page.caption;
    if ( caption.IsEmpty() ) {
        caption = "Tp";
    }
    
    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    if ( page.active ) {
        fnt.SetWeight(wxFONTWEIGHT_BOLD);
    }
    gdc.SetFont( fnt );
    wxSize ext = gdc.GetTextExtent( caption );
    if ( caption == "Tp" )
        caption.Clear();
    
    /// Draw the bitmap
    if ( page.bitmap.IsOk() ) {
        int bmpy = (rr.y + (rr.height - page.bitmap.GetHeight())/2)-TAB_Y_OFFSET;
        gdc.GetGraphicsContext()->DrawBitmap( page.bitmap, curx, bmpy, page.bitmap.GetWidth(), page.bitmap.GetHeight());
        curx += page.bitmap.GetWidth();
        curx += 3;
    }
    
    /// Draw the text
    gdc.SetTextForeground( textColour );
    gdc.GetGraphicsContext()->DrawText( page.caption, curx, (rr.y + (rr.height - ext.y)/2)-TAB_Y_OFFSET+TEXT_Y_SPACER);
    
    // advance the X offset
    curx += ext.x;
    curx += 3;
    
    /// Draw the X button on the tab
    if ( close_button_state != wxAUI_BUTTON_STATE_HIDDEN ) {
        int btny = (rr.y + (rr.height/2))-TAB_Y_OFFSET+TEXT_Y_SPACER;
        if ( close_button_state == wxAUI_BUTTON_STATE_PRESSED ) {
            curx += 1;
            btny += 1;
        }
        
        /// Defines the rectangle surrounding the X button
        wxRect xRect = wxRect(curx, btny - X_RADIUS, X_DIAMETER, X_DIAMETER);
        *out_button_rect = xRect;
        
        /// Defines the 'x' inside the circle
        wxPoint circleCenter( curx + X_RADIUS, btny);
        wxDouble xx_width = ::sqrt( ::pow(X_DIAMETER, 2.0) /2.0 );
        wxDouble x_square = (circleCenter.x - (xx_width/2.0));
        wxDouble y_square = (circleCenter.y - (xx_width/2.0));
        
        wxPoint2DDouble ptXTopLeft(x_square, y_square);
        wxRect2DDouble insideRect(ptXTopLeft.m_x, ptXTopLeft.m_y, xx_width, xx_width);
        insideRect.Inset(1.0 , 1.0); // Shrink it by 1 pixle
        
        /// Draw the 'x' itself
        wxGraphicsPath xpath = gdc.GetGraphicsContext()->CreatePath();
        xpath.MoveToPoint( insideRect.GetLeftTop() );
        xpath.AddLineToPoint( insideRect.GetRightBottom());
        xpath.MoveToPoint( insideRect.GetRightTop() );
        xpath.AddLineToPoint( insideRect.GetLeftBottom() );
        gdc.SetPen( wxPen(textColour, 1) );
        gdc.GetGraphicsContext()->StrokePath( xpath  );
        
        curx += X_DIAMETER;
    }
    *out_tab_rect = rr;
    gdc.DestroyClippingRegion();
}

wxSize clAuiGlossyTabArt::GetTabSize(wxDC& dc,
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
    if ( active ) {
        f.SetWeight(wxFONTWEIGHT_BOLD);
    }
    
    dc.SetFont(f);
    dc.GetTextExtent(caption, &measured_textx, &tmp);

    // do it once
    if(measured_texty == wxNOT_FOUND)
        dc.GetTextExtent(wxT("ABCDEFXj"), &tmp, &measured_texty);

    // add padding around the text
    wxCoord tab_width  = measured_textx;
    wxCoord tab_height = measured_texty;
    
    if ( tab_height < 16 ) 
        tab_height = 16;
    
    // if the close button is showing, add space for it
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
        tab_width += X_DIAMETER + 3;

    // if there's a bitmap, add space for it
    
    // NOTE: we only support 16 pixels bitmap (or smaller)
    // so there is no need to adjust the tab height!
    tab_height += TAB_HEIGHT_SPACER;
    if (bitmap.IsOk()) {
        tab_width += bitmap.GetWidth();
        tab_width += 3; // right side bitmap padding
    }

    // add padding
#ifdef __WXMAC__
    tab_width += 16;
#else
    tab_width += 16;
#endif


    if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
        tab_width = 80;
    }

    *x_extent = tab_width;

    return wxSize(tab_width, tab_height);
}

void clAuiGlossyTabArt::DoDrawInactiveTabSeparator(wxGCDC& gdc, const wxRect& tabRect)
{
    wxRect rr = tabRect;
    rr.SetWidth(1);
    rr.SetHeight(tabRect.GetHeight()+2);
    rr.x = tabRect.GetTopRight().x - 1;
    rr.y = tabRect.GetTopRight().y - 2;
    
    wxColour sideColour;
    if ( DrawingUtils::IsThemeDark() ) {
        sideColour = m_bgColour.ChangeLightness(110);
    } else {
        sideColour = *wxWHITE;
    }
    // Draw 2 lines on the sides of the "main" line
    wxRect rectLeft, rectRight, rectCenter;
    wxPoint topPt = rr.GetTopLeft();
    topPt.x -= 1;
    rectLeft = wxRect(topPt, wxSize(1, rr.GetHeight()) );
    gdc.GradientFillLinear(rectLeft, sideColour, m_bgColour, wxNORTH);
    
    topPt.x += 1;
    rectCenter = wxRect(topPt, wxSize(1, rr.GetHeight()) );
    gdc.GradientFillLinear(rectCenter, m_penColour.ChangeLightness(80), m_bgColour, wxNORTH);
    
    topPt.x += 1;
    rectRight = wxRect(topPt, wxSize(1, rr.GetHeight()) );
    gdc.GradientFillLinear(rectRight, sideColour, m_bgColour, wxNORTH);
}

void clAuiGlossyTabArt::DoGetTabAreaBackgroundColour(wxColour& bgColour, wxColour& penColour)
{
    bgColour = wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    // Determine the pen colour
    if ( DrawingUtils::IsDark(bgColour)) {
        penColour = DrawingUtils::LightColour(bgColour, 4.0);
    } else {
        penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
   }
   
    // Now set the bg colour. It must be done after setting 
    // the pen colour
    bgColour = DrawingUtils::GetAUIPaneBGColour();
    
    // Allow the plugins to override the default colours
    clColourEvent colourEvent( wxEVT_GET_TAB_BORDER_COLOUR );
    if ( EventNotifier::Get()->ProcessEvent( colourEvent ) ) {
        penColour = colourEvent.GetBorderColour();
        bgColour = penColour.ChangeLightness(150);
    }
}
