///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art_msw.cpp
// Purpose:     MSW style art provider for ribbon interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include <wx/frame.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ribbon/art.h"
#include "wx/ribbon/art_internal.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"

#include "art_metro.h"

#ifndef WX_PRECOMP
#include "wx/dcmemory.h"
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

static const char* const gallery_up_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  "     ",
  "  x  ",
  " xxx ",
  "xxxxx",
  "     "};

static const char* const gallery_down_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  "     ",
  "xxxxx",
  " xxx ",
  "  x  ",
  "     "};

static const char* const gallery_left_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  "   x ",
  "  xx ",
  " xxx ",
  "  xx ",
  "   x "};

static const char* const gallery_right_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  " x   ",
  " xx  ",
  " xxx ",
  " xx  ",
  " x   "};

static const char* const gallery_extension_xpm[] = {
  "5 5 2 1",
  "  c None",
  "x c #FF00FF",
  "xxxxx",
  "     ",
  "xxxxx",
  " xxx ",
  "  x  "};

static const char* const panel_extension_xpm[] = {
  "7 7 2 1",
  "  c None",
  "x c #FF00FF",
  "xxxxxx ",
  "x      ",
  "x      ",
  "x  x  x",
  "x   xxx",
  "x   xxx",
  "   xxxx"};

wxRibbonMetroArtProvider::wxRibbonMetroArtProvider(bool set_colour_scheme) : wxRibbonMSWArtProvider (set_colour_scheme)
{
    m_flags = 0;
    m_tab_label_font = *wxNORMAL_FONT;
    m_button_bar_label_font = m_tab_label_font;
    m_panel_label_font = m_tab_label_font;

    if(set_colour_scheme)
    {
        SetColourScheme(
            wxColour(255, 255, 255),
            wxColour(120, 174, 229),
            wxColour(  0,   0,   0));
    }

    m_cached_tab_separator_visibility = -10.0; // valid visibilities are in range [0, 1]
    m_tab_separation_size = 3;
    m_page_border_left = 2;
    m_page_border_top = 1;
    m_page_border_right = 2;
    m_page_border_bottom = 3;
    m_panel_x_separation_size = 2;
    m_panel_y_separation_size = 1;
    m_tool_group_separation_size = 3;
    m_gallery_bitmap_padding_left_size = 4;
    m_gallery_bitmap_padding_right_size = 4;
    m_gallery_bitmap_padding_top_size = 4;
    m_gallery_bitmap_padding_bottom_size = 4;
}

wxRibbonMetroArtProvider::~wxRibbonMetroArtProvider()
{
}
void wxRibbonMetroArtProvider::GetColourScheme(
                         wxColour* primary,
                         wxColour* secondary,
                         wxColour* tertiary) const
{
    if(primary != NULL)
        *primary = m_primary_scheme_colour;
    if(secondary != NULL)
        *secondary = m_secondary_scheme_colour;
    if(tertiary != NULL)
        *tertiary = m_tertiary_scheme_colour;
}
void wxRibbonMetroArtProvider::SetColourScheme(
                         const wxColour& primary,
                         const wxColour& secondary,
                         const wxColour& tertiary)
{
    m_primary_scheme_colour = primary;
    m_secondary_scheme_colour = secondary;
    m_tertiary_scheme_colour = tertiary;

    wxRibbonHSLColour primary_hsl(primary);
    wxRibbonHSLColour secondary_hsl(secondary);
    // tertiary not used for anything

    // Map primary saturation from [0, 1] to [.25, .75]
    bool primary_is_gray = false;
    static const double gray_saturation_threshold = 0.01;
    if(primary_hsl.saturation <= gray_saturation_threshold)
        primary_is_gray = true;
    else
    {
        primary_hsl.saturation = cos(primary_hsl.saturation * M_PI)
            * -0.25 + 0.5;
    }

    // Map primary luminance from [0, 1] to [.23, .83]
    primary_hsl.luminance = cos(primary_hsl.luminance * M_PI) * -0.3 + 0.53;

    // Map secondary saturation from [0, 1] to [0.16, 0.84]
    bool secondary_is_gray = false;
    if(secondary_hsl.saturation <= gray_saturation_threshold)
        secondary_is_gray = true;
    else
    {
        secondary_hsl.saturation = cos(secondary_hsl.saturation * M_PI)
            * -0.34 + 0.5;
    }

    // Map secondary luminance from [0, 1] to [0.1, 0.9]
    secondary_hsl.luminance = cos(secondary_hsl.luminance * M_PI) * -0.4 + 0.5;

#define LikePrimary(h, s, l) \
    primary_hsl.ShiftHue(h ## f).Saturated(primary_is_gray ? 0 : s ## f) \
    .Lighter(l ## f).ToRGB()
#define LikeSecondary(h, s, l) \
    secondary_hsl.ShiftHue(h ## f).Saturated(secondary_is_gray ? 0 : s ## f) \
    .Lighter(l ## f).ToRGB()

    m_page_border_pen = wxColour(218,219,220); // LikePrimary(1.4, 0.00, -0.2); // LikePrimary(1.4, 0.00, -0.08);

	// these are all needed for galleries (which are drawn using base class methods)
    m_page_background_top_colour = wxColour(245,246,247); // wxColour(254,254,254); // 
    m_page_background_top_gradient_colour = wxColour(245,246,247); // wxColour(242,244,246);
    m_page_background_colour = wxColour(245,246,247);
    m_page_background_gradient_colour = wxColour(245,246,247); // wxColour(229,233,238);

    m_tab_active_background_colour = wxColour(245,246,247); // LikePrimary(1.0, 1.0, 0.94);
    m_tab_active_background_gradient_colour = *wxBLUE; // wxColour(250,250,250); // LikePrimary(1.0, 1.0, 0.94);
    m_tab_separator_colour = wxColour (245,246,247); // LikePrimary(0.9, 0.24, 0.05);
    m_tab_ctrl_background_brush = wxColour(254,254,254); // LikePrimary(1.0, 0.39, 0.07);
    m_tab_hover_background_colour = wxColour(253,253,253); // LikePrimary(1.3, 0.15, 0.10);
    m_tab_hover_background_top_colour = *wxBLUE; //  wxColour(254,254,254); // LikePrimary(1.4, 0.36, 0.08);
    m_tab_border_pen = wxColour(218,219,220); // LikePrimary(1.4, 0.03, -0.05);
    m_tab_separator_gradient_colour = LikePrimary(1.7, -0.15, -0.18);
    m_tab_hover_background_top_gradient_colour =  *wxBLUE; // wxColour(254,254,254); // LikePrimary(1.8, 0.34, 0.13);
    m_tab_label_colour = LikePrimary(4.3, 0.13, -0.49);
    m_tab_hover_background_gradient_colour =  *wxBLUE; // LikeSecondary(-1.5, -0.34, 0.01);

    m_panel_minimised_border_gradient_pen = wxColour(232,239,247); // LikePrimary(-6.9, -0.17, -0.09);
    m_panel_minimised_border_pen = wxColour(164, 206, 249); // LikePrimary(-5.3, -0.24, -0.06);
    m_panel_border_gradient_pen = *wxBLUE; // LikePrimary(-5.2, -0.15, -0.06);
    m_panel_border_pen = wxColour(226,227,228); // LikePrimary(-2.8, -0.32, 0.02);
    m_panel_hover_label_background_brush = *wxBLUE; // LikePrimary(1.0, 0.30, 0.09);
    m_panel_active_background_colour = LikePrimary(1.6, -0.18, 0.18);
    m_panel_active_background_gradient_colour = LikePrimary(0.5, 0.34, 0.05);
    m_panel_label_colour = LikePrimary(2.8, -0.14, -0.45); // LikePrimary(2.8, -0.14, -0.35);
    m_panel_minimised_label_colour = m_tab_label_colour;
    m_panel_hover_button_background_brush = wxColour(232,239,247); //LikeSecondary(-0.9, 0.16, -0.07);
    m_panel_hover_button_border_pen = wxColour(164, 206, 249); // LikeSecondary(-3.9, -0.16, -0.14);
    SetColour(wxRIBBON_ART_PANEL_BUTTON_FACE_COLOUR, LikePrimary(1.4, -0.21, -0.23));
    SetColour(wxRIBBON_ART_PANEL_BUTTON_HOVER_FACE_COLOUR, LikePrimary(1.5, -0.24, -0.29));

    m_button_bar_label_colour =  LikePrimary(2.8, -0.14, -0.6); // m_tab_label_colour;
    m_button_bar_hover_border_pen = wxColour(164, 206, 249); // LikeSecondary(-6.2, 0.47, -0.1); // LikeSecondary(-6.2, -0.47, -0.14);
    m_button_bar_hover_background_gradient_colour = wxColour(232,239,247); // LikeSecondary(-0.6, 0.16, 0.2); // LikeSecondary(-0.6, 0.16, 0.04);
    m_button_bar_hover_background_colour = wxColour(232,239,247); // LikeSecondary(-0.2, 0.16, -0.01); // LikeSecondary(-0.2, 0.16, -0.10);
    m_button_bar_hover_background_top_gradient_colour = wxColour(232,239,247); // LikeSecondary(0.2, 0.16, 0.03); // LikeSecondary(0.2, 0.16, 0.03);
    // m_button_bar_hover_background_top_colour = *wxRED; // LikeSecondary(8.8, 0.16, 0.17); // LikeSecondary(8.8, 0.16, 0.17);
    m_button_bar_active_border_pen = wxColour(102,167,232); // LikeSecondary(-6.2, -0.47, -0.25);
    m_button_bar_active_background_top_colour = *wxYELLOW; // LikeSecondary(-8.4, 0.08, 0.06);
    m_button_bar_active_background_top_gradient_colour = *wxYELLOW; // LikeSecondary(-9.7, 0.13, -0.07);
    m_button_bar_active_background_colour = wxColour(209,232,255); // LikeSecondary(-9.9, 0.14, -0.14);
    m_button_bar_active_background_gradient_colour = *wxYELLOW; // LikeSecondary(-8.7, 0.17, -0.03);

    m_gallery_button_disabled_background_colour = LikePrimary(-2.8, -0.46, 0.09);
    m_gallery_button_disabled_background_top_brush = LikePrimary(-2.8, -0.36, 0.15);
    m_gallery_hover_background_brush = LikePrimary(-0.8, 0.05, 0.15);
    m_gallery_border_pen = LikePrimary(0.7, -0.02, 0.03);
    m_gallery_button_background_top_brush = LikePrimary(0.8, 0.34, 0.13);
    m_gallery_button_background_colour = LikePrimary(1.3, 0.10, 0.08);
    // SetColour used so that the relevant bitmaps are generated
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR, LikePrimary(1.4, -0.21, -0.23));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR, LikePrimary(1.5, -0.24, -0.29));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR, LikePrimary(1.5, -0.24, -0.29));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR, LikePrimary(0.0, -1.0, 0.0));
    m_gallery_button_disabled_background_gradient_colour = LikePrimary(1.5, -0.43, 0.12);
    m_gallery_button_background_gradient_colour = LikePrimary(1.7, 0.11, 0.09);
    m_gallery_item_border_pen = LikeSecondary(-3.9, -0.16, -0.14);
    m_gallery_button_hover_background_colour = m_button_bar_hover_background_colour; // LikeSecondary(-0.9, 0.16, -0.07);
    m_gallery_button_hover_background_gradient_colour = m_button_bar_hover_background_gradient_colour; // LikeSecondary(0.1, 0.12, 0.03);
    m_gallery_button_hover_background_top_brush = m_button_bar_hover_background_top_gradient_colour; // LikeSecondary(4.3, 0.16, 0.17);
    m_gallery_button_active_background_colour = LikeSecondary(-9.9, 0.03, -0.22);
    m_gallery_button_active_background_gradient_colour = LikeSecondary(-9.5, 0.14, -0.11);
    m_gallery_button_active_background_top_brush = LikeSecondary(-9.0, 0.15, -0.08);

    m_toolbar_border_pen = m_button_bar_hover_border_pen; // LikePrimary(1.4, -0.21, -0.16);
    SetColour(wxRIBBON_ART_TOOLBAR_FACE_COLOUR, LikePrimary(1.4, -0.17, -0.22));
    m_tool_background_top_colour = LikePrimary(-1.9, -0.07, 0.06);
    m_tool_background_top_gradient_colour = LikePrimary(1.4, 0.12, 0.08);
    m_tool_background_colour = wxColour(232,239,247); // LikePrimary(1.4, -0.09, 0.03);
    m_tool_background_gradient_colour = *wxRED; // LikePrimary(1.9, 0.11, 0.09);
    m_tool_hover_background_top_colour = m_button_bar_hover_background_top_colour; // LikeSecondary(3.4, 0.11, 0.16);
    m_tool_hover_background_top_gradient_colour = m_button_bar_hover_background_top_gradient_colour; // LikeSecondary(-1.4, 0.04, 0.08);
    m_tool_hover_background_colour = m_button_bar_hover_background_colour; // LikeSecondary(-1.8, 0.16, -0.12);
    m_tool_hover_background_gradient_colour = m_button_bar_hover_background_gradient_colour; // LikeSecondary(-2.6, 0.16, 0.05);
    m_tool_active_background_top_colour = LikeSecondary(-9.9, -0.12, -0.09);
    m_tool_active_background_top_gradient_colour = LikeSecondary(-8.5, 0.16, -0.12);
    m_tool_active_background_colour = LikeSecondary(-7.9, 0.16, -0.20);
    m_tool_active_background_gradient_colour = LikeSecondary(-6.6, 0.16, -0.10);

#undef LikePrimary
#undef LikeSecondary

    // Invalidate cached tab separator
    m_cached_tab_separator_visibility = -1.0;
}

void wxRibbonMetroArtProvider::SetFlags(long flags)
{
    if((flags ^ m_flags) & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        if(flags & wxRIBBON_BAR_FLOW_VERTICAL)
        {
            m_page_border_left++;
            m_page_border_right++;
            m_page_border_top--;
            m_page_border_bottom--;
        }
        else
        {
            m_page_border_left--;
            m_page_border_right--;
            m_page_border_top++;
            m_page_border_bottom++;
        }
    }
    m_flags = flags;

    // Need to reload some bitmaps when flags change
#define Reload(setting) SetColour(setting, GetColour(setting))
    Reload(wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR);
    Reload(wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR);
    Reload(wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR);
    Reload(wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR);
    Reload(wxRIBBON_ART_PANEL_BUTTON_FACE_COLOUR);
    Reload(wxRIBBON_ART_PANEL_BUTTON_HOVER_FACE_COLOUR);
#undef Reload
}
void wxRibbonMetroArtProvider::DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_tab_ctrl_background_brush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    dc.SetPen(m_page_border_pen); 
    if(rect.width > 6)
    {
        dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1);
    }
    else
    {
        dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1);
    }
}

void wxRibbonMetroArtProvider::DrawTab(
                 wxDC& dc,
                 wxWindow* WXUNUSED(wnd),
                 const wxRibbonPageTabInfo& tab)
{
    if(tab.rect.height <= 2)
        return;

    if(tab.active || tab.hovered)
    {
        if(tab.active)
        {
            wxRect background(tab.rect);

            background.x += 2;
            background.y += 1;
            background.width -= 3;
            background.height -= 1;
			
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.SetBrush(m_tab_active_background_colour);
			dc.DrawRectangle(background);

            // TODO: active and hovered
        }
        else if(tab.hovered)
        {
            wxRect background(tab.rect);

            background.x += 2;
            background.y += 1;
            background.width -= 3;
            background.height -= 2;
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.SetBrush(m_tab_hover_background_colour);
			dc.DrawRectangle(background);
        }

        wxPoint border_points[4];
        border_points[0] = wxPoint(1, tab.rect.height - 2);
        border_points[1] = wxPoint(1, 0);
        border_points[2] = wxPoint(tab.rect.width - 1, 0);
        border_points[3] = wxPoint(tab.rect.width - 1, tab.rect.height - 1);

        if (tab.active)
			dc.SetPen(m_tab_border_pen);
		else
			dc.SetPen(m_tab_border_pen); // TODO: introduce hover border pen colour
        dc.DrawLines(sizeof(border_points)/sizeof(wxPoint), border_points, tab.rect.x, tab.rect.y);
    }

    if(m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS)
    {
        wxBitmap icon = tab.page->GetIcon();
        if(icon.IsOk())
        {
        int x = tab.rect.x + 4;
        if((m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) == 0)
            x = tab.rect.x + (tab.rect.width - icon.GetWidth()) / 2;
        dc.DrawBitmap(icon, x, tab.rect.y + 1 + (tab.rect.height - 1 -
            icon.GetHeight()) / 2, true);
        }
    }
    if(m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS)
    {
        wxString label = tab.page->GetLabel();
        if(!label.IsEmpty())
        {
            dc.SetFont(m_tab_label_font);
            dc.SetTextForeground(m_tab_label_colour);
            dc.SetBackgroundMode(wxTRANSPARENT);

            int text_height;
            int text_width;
            dc.GetTextExtent(label, &text_width, &text_height);
            int width = tab.rect.width - 5;
            int x = tab.rect.x + 3;
            if(m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS)
            {
                x += 3 + tab.page->GetIcon().GetWidth();
                width -= 3 + tab.page->GetIcon().GetWidth();
            }
            int y = tab.rect.y + (tab.rect.height - text_height) / 2;

            if(width <= text_width)
            {
                dc.SetClippingRegion(x, tab.rect.y, width, tab.rect.height);
                dc.DrawText(label, x, y);
            }
            else
            {
                dc.DrawText(label, x + (width - text_width) / 2 + 1, y);
            }
        }
    }
}

void wxRibbonMetroArtProvider::DrawPartialPageBackground(wxDC& dc,
        wxWindow* wnd, const wxRect& rect, wxRibbonPage* page,
        wxPoint offset, bool hovered)
{
    wxRect background;
    // Expanded panels need a background - the expanded panel at
    // best size may have a greater Y dimension higher than when
    // on the bar if it has a sizer. AUI art provider does not need this
    // because it paints the panel without reference to its parent's size.
    // Expanded panels use a wxFrame as parent (not a wxRibbonPage).

    if(wnd->GetSizer() && wnd->GetParent() != page)
    {
        background = wnd->GetParent()->GetSize();
        offset = wxPoint(0,0);
    }
    else
    {
        background = page->GetSize();
        page->AdjustRectToIncludeScrollButtons(&background);
        background.height -= 2;
    }
    // Page background isn't dependant upon the width of the page
    // (at least not the part of it intended to be painted by this
    // function). Set to wider than the page itself for when externally
    // expanded panels need a background - the expanded panel can be wider
    // than the bar.
    background.x = 0;
    background.width = INT_MAX;

    wxRect paint_rect(rect);

	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(m_page_background_colour);
	dc.DrawRectangle(rect);
}

void wxRibbonMetroArtProvider::DrawPageBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
	wxRect background(rect);

    dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(m_page_background_colour);
	dc.DrawRectangle(background);
	background.width += 1;
	dc.SetPen(m_page_border_pen);
	dc.DrawLine(background.GetBottomLeft(), background.GetBottomRight());
}

void wxRibbonMetroArtProvider::DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect true_rect(rect);
    RemovePanelPadding(&true_rect);
    bool has_ext_button = wnd->HasExtButton();

	// draw panel label
    {
		// int label_height;
        dc.SetFont(m_panel_label_font);
        dc.SetPen(*wxTRANSPARENT_PEN);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetTextForeground(m_panel_label_colour);

        wxRect label_rect(true_rect);
        wxString label = wnd->GetLabel();
        bool clip_label = false;
        wxSize label_size(dc.GetTextExtent(label));

        label_rect.SetX(label_rect.GetX() + 1);
        label_rect.SetWidth(label_rect.GetWidth() - 2);
        label_rect.SetHeight(label_size.GetHeight() + 2);
        label_rect.SetY(true_rect.GetBottom() - label_rect.GetHeight());
        // label_height = label_rect.GetHeight();

        wxRect label_bg_rect = label_rect;

        if(has_ext_button)
            label_rect.SetWidth(label_rect.GetWidth() - 13);

        if(label_size.GetWidth() > label_rect.GetWidth())
        {
            // Test if there is enough length for 3 letters and ...
            wxString new_label = label.Mid(0, 3) + wxT("...");
            label_size = dc.GetTextExtent(new_label);
            if(label_size.GetWidth() > label_rect.GetWidth())
            {
                // Not enough room for three characters and ...
                // Display the entire label and just crop it
                clip_label = true;
            }
            else
            {
                // Room for some characters and ...
                // Display as many characters as possible and append ...
                for(size_t len = label.Len() - 1; len >= 3; --len)
                {
                    new_label = label.Mid(0, len) + wxT("...");
                    label_size = dc.GetTextExtent(new_label);
                    if(label_size.GetWidth() <= label_rect.GetWidth())
                    {
                        label = new_label;
                        break;
                    }
                }
            }
        }

        dc.DrawRectangle(label_bg_rect);
        if(clip_label)
        {
            wxDCClipper clip(dc, label_rect);
            dc.DrawText(label, label_rect.x, label_rect.y +
                (label_rect.GetHeight() - label_size.GetHeight()) / 2);
        }
        else
        {
            dc.DrawText(label, label_rect.x +
                (label_rect.GetWidth() - label_size.GetWidth()) / 2,
                label_rect.y +
                (label_rect.GetHeight() - label_size.GetHeight()) / 2);
        }

        if(has_ext_button)
        {
            if(wnd->IsExtButtonHovered())
            {
                dc.SetPen(m_panel_hover_button_border_pen);
                dc.SetBrush(m_panel_hover_button_background_brush);
                dc.DrawRectangle(label_rect.GetRight(), label_rect.GetBottom() - 14, 14, 14);
                dc.DrawBitmap(m_panel_extension_bitmap[1], label_rect.GetRight() + 3, label_rect.GetBottom() - 10, true);
            }
            else
                dc.DrawBitmap(m_panel_extension_bitmap[0], label_rect.GetRight() + 3, label_rect.GetBottom() - 10, true);
        }
    }
	
	if (wnd->GetParent()->IsKindOf(CLASSINFO(wxFrame))) // expanded panels are in their own wxFrame otherwise normal panel
	{	
		wxRect shadow(rect);
		shadow.x +=4;
		shadow.y +=4;
		dc.SetPen(*wxRED);
		dc.DrawLine(shadow.GetBottomLeft(), shadow.GetBottomRight());
		dc.DrawLine(shadow.GetTopRight(), shadow.GetBottomRight());
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(m_panel_border_pen);
		dc.DrawRoundedRectangle(rect, 1.0);
	}
	else
		DrawPanelBorder(dc, true_rect, m_panel_border_pen, m_panel_border_gradient_pen);
}

void wxRibbonMetroArtProvider::DrawPanelBorder(wxDC& dc, const wxRect& rect,
                                             wxPen& primary_colour,
                                             wxPen& secondary_colour)
{
		dc.SetPen(m_panel_border_pen);
		dc.DrawLine(rect.GetTopRight(), rect.GetBottomRight());
}

void wxRibbonMetroArtProvider::DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap)
{
    DrawPartialPageBackground(dc, wnd, rect, false);

    wxRect true_rect(rect);
    // RemovePanelPadding(&true_rect);
	
	wxRect client_rect(true_rect);
	client_rect.width -= 2;
	client_rect.y++;
	client_rect.height -= 2;
	
	if(wnd->GetExpandedPanel() != NULL)
    {
		dc.SetPen(m_button_bar_active_border_pen);
		dc.SetBrush (m_button_bar_active_background_colour);
		dc.DrawRectangle(client_rect);
    }
    else if(wnd->IsHovered())
    {
		dc.SetPen(m_button_bar_hover_border_pen);
		dc.SetBrush (m_panel_active_background_colour);
		dc.DrawRectangle(client_rect);
    }
	else {
		dc.SetPen(m_button_bar_hover_border_pen);
		dc.SetBrush (m_button_bar_hover_background_colour);
		dc.DrawRectangle(client_rect);		
	}

    wxRect preview;
    DrawMinimisedPanelCommon(dc, wnd, true_rect, &preview);
    dc.SetBrush(m_panel_active_background_colour);
    dc.SetPen(m_button_bar_hover_border_pen);
	dc.DrawRectangle(preview);

    if(bitmap.IsOk())
    {
        dc.DrawBitmap(bitmap, preview.x + (preview.width - bitmap.GetWidth()) / 2,
            preview.y + (preview.height - 7 - bitmap.GetHeight()) / 2, true);
    }

    DrawPanelBorder(dc, true_rect, m_panel_border_pen, m_panel_border_gradient_pen);
}

void wxRibbonMetroArtProvider::DrawMinimisedPanelCommon(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& true_rect,
                        wxRect* preview_rect)
{
    wxRect preview(0, 0, 32, 32);
    if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        preview.x = true_rect.x + 4;
        preview.y = true_rect.y + (true_rect.height - preview.height) / 2;
    }
    else
    {
        preview.x = true_rect.x + (true_rect.width - preview.width) / 2;
        preview.y = true_rect.y + 4;
    }
    if(preview_rect)
        *preview_rect = preview;

    wxCoord label_width, label_height;
    dc.SetFont(m_panel_label_font);
    dc.GetTextExtent(wnd->GetLabel(), &label_width, &label_height);

    int xpos = true_rect.x + (true_rect.width - label_width + 1) / 2;
    int ypos = preview.y + preview.height + 5;

    if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        xpos = preview.x + preview.width + 5;
        ypos = true_rect.y + (true_rect.height - label_height) / 2;
    }

    dc.SetTextForeground(m_panel_minimised_label_colour);
    dc.DrawText(wnd->GetLabel(), xpos, ypos);


    wxPoint arrow_points[3];
    if(m_flags & wxRIBBON_BAR_FLOW_VERTICAL)
    {
        xpos += label_width;
        arrow_points[0] = wxPoint(xpos + 5, ypos + label_height / 2);
        arrow_points[1] = arrow_points[0] + wxPoint(-3,  3);
        arrow_points[2] = arrow_points[0] + wxPoint(-3, -3);
    }
    else
    {
        ypos += label_height;
        arrow_points[0] = wxPoint(true_rect.width / 2, ypos + 5);
        arrow_points[1] = arrow_points[0] + wxPoint(-3, -3);
        arrow_points[2] = arrow_points[0] + wxPoint( 3, -3);
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    wxBrush B(m_panel_minimised_label_colour);
    dc.SetBrush(B);
    dc.DrawPolygon(sizeof(arrow_points)/sizeof(wxPoint), arrow_points,
        true_rect.x, true_rect.y);
}

void wxRibbonMetroArtProvider::DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect, true);
}

void wxRibbonMetroArtProvider::DrawPartialPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        bool allow_hovered)
{
    // Assume the window is a child of a ribbon page, and also check for a
    // hovered panel somewhere between the window and the page, as it causes
    // the background to change.
    wxPoint offset(wnd->GetPosition());
    wxRibbonPage* page = NULL;
    wxWindow* parent = wnd->GetParent();
    wxRibbonPanel* panel = wxDynamicCast(wnd, wxRibbonPanel);
    bool hovered = false;

    if(panel != NULL)
    {
        hovered = allow_hovered && panel->IsHovered();
        if(panel->GetExpandedDummy() != NULL)
        {
            offset = panel->GetExpandedDummy()->GetPosition();
            parent = panel->GetExpandedDummy()->GetParent();
        }
    }
    for(; parent; parent = parent->GetParent())
    {
        if(panel == NULL)
        {
            panel = wxDynamicCast(parent, wxRibbonPanel);
            if(panel != NULL)
            {
                hovered = allow_hovered && panel->IsHovered();
                if(panel->GetExpandedDummy() != NULL)
                {
                    parent = panel->GetExpandedDummy();
                }
            }
        }
        page = wxDynamicCast(parent, wxRibbonPage);
        if(page != NULL)
        {
            break;
        }
        offset += parent->GetPosition();
    }
    if(page != NULL)
    {
        DrawPartialPageBackground(dc, wnd, rect, page, offset, hovered);
        return;
    }

    // No page found - fallback to painting with a stock brush
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxRibbonMetroArtProvider::DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmap_large,
                        const wxBitmap& bitmap_small)
{
    if(kind == wxRIBBON_BUTTON_TOGGLE)
    {
        kind = wxRIBBON_BUTTON_NORMAL;
        if(state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED)
            state ^= wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
    }

    if(state & (wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK |
        wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK))
    {
        if(state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK)
            dc.SetPen(m_button_bar_active_border_pen);
        else
            dc.SetPen(m_button_bar_hover_border_pen);

        wxRect bg_rect(rect);
        bg_rect.x++;
        bg_rect.y++;
        bg_rect.width -= 2;
        bg_rect.height -= 2;
		
        wxRect bg_rect_top(bg_rect);
        bg_rect_top.height /= 3;
        bg_rect.y += bg_rect_top.height;
        bg_rect.height -= bg_rect_top.height;
		
        if(kind == wxRIBBON_BUTTON_HYBRID)
        {
            switch(state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK)
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                {
                    int iYBorder = rect.y + bitmap_large.GetHeight() + 4;
                    wxRect partial_bg(rect);
                    if(state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED)
                    {
                        partial_bg.SetBottom(iYBorder - 1);
                    }
                    else
                    {
                        partial_bg.height -= (iYBorder - partial_bg.y + 1);
                        partial_bg.y = iYBorder + 1;
                    }
                    dc.DrawLine(rect.x, iYBorder, rect.x + rect.width, iYBorder);
                    bg_rect.Intersect(partial_bg);
                    bg_rect_top.Intersect(partial_bg);
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
                {
                    int iArrowWidth = 9;
                    if(state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED)
                    {
                        bg_rect.width -= iArrowWidth;
                        bg_rect_top.width -= iArrowWidth;
                        dc.DrawLine(bg_rect_top.x + bg_rect_top.width,
                            rect.y, bg_rect_top.x + bg_rect_top.width,
                            rect.y + rect.height);
                    }
                    else
                    {
                        --iArrowWidth;
                        bg_rect.x += bg_rect.width - iArrowWidth;
                        bg_rect_top.x += bg_rect_top.width - iArrowWidth;
                        bg_rect.width = iArrowWidth;
                        bg_rect_top.width = iArrowWidth;
                        dc.DrawLine(bg_rect_top.x - 1, rect.y,
                            bg_rect_top.x - 1, rect.y + rect.height);
                    }
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
                break;
            }
        }

        if(state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK)
        {
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.SetBrush(m_button_bar_active_background_colour);
			dc.DrawRectangle(bg_rect_top);
			dc.DrawRectangle(bg_rect);

        }
        else
        {
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.SetBrush(m_button_bar_hover_background_colour);
			dc.DrawRectangle(bg_rect_top);
			dc.DrawRectangle(bg_rect);
        }

		if(state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK)
            dc.SetPen(m_button_bar_active_border_pen);
        else
            dc.SetPen(m_button_bar_hover_border_pen);
			
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle(rect);
    }

    dc.SetFont(m_button_bar_label_font);
    dc.SetTextForeground(m_button_bar_label_colour);
    DrawButtonBarButtonForeground(dc, rect, kind, state, label, bitmap_large,
        bitmap_small);
}

void wxRibbonMetroArtProvider::DrawButtonBarButtonForeground(
                        wxDC& dc,
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmap_large,
                        const wxBitmap& bitmap_small)
{
    switch(state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK)
    {
    case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
        {
            const int padding = 2;
            dc.DrawBitmap(bitmap_large,
                rect.x + (rect.width - bitmap_large.GetWidth()) / 2,
                rect.y + padding, true);
            int ypos = rect.y + padding + bitmap_large.GetHeight() + padding;
            int arrow_width = kind == wxRIBBON_BUTTON_NORMAL ? 0 : 8;
            wxCoord label_w, label_h;
            dc.GetTextExtent(label, &label_w, &label_h);
            if(label_w + 2 * padding <= rect.width)
            {
                dc.DrawText(label, rect.x + (rect.width - label_w) / 2, ypos);
                if(arrow_width != 0)
                {
                    DrawDropdownArrow(dc, rect.x + rect.width / 2,
                        ypos + (label_h * 3) / 2,
                        m_button_bar_label_colour);
                }
            }
            else
            {
                size_t breaki = label.Len();
                do
                {
                    --breaki;
                    if(wxRibbonCanLabelBreakAtPosition(label, breaki))
                    {
                        wxString label_top = label.Mid(0, breaki);
                        dc.GetTextExtent(label_top, &label_w, &label_h);
                        if(label_w + 2 * padding <= rect.width)
                        {
                            dc.DrawText(label_top,
                                rect.x + (rect.width - label_w) / 2, ypos);
                            ypos += label_h;
                            wxString label_bottom = label.Mid(breaki + 1);
                            dc.GetTextExtent(label_bottom, &label_w, &label_h);
                            label_w += arrow_width;
                            int iX = rect.x + (rect.width - label_w) / 2;
                            dc.DrawText(label_bottom, iX, ypos);
                            if(arrow_width != 0)
                            {
                                DrawDropdownArrow(dc,
                                    iX + 2 +label_w - arrow_width,
                                    ypos + label_h / 2 + 1,
                                    m_button_bar_label_colour);
                            }
                            break;
                        }
                    }
                } while(breaki > 0);
            }
        }
        break;
    case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
        {
            int x_cursor = rect.x + 2;
            dc.DrawBitmap(bitmap_small, x_cursor,
                    rect.y + (rect.height - bitmap_small.GetHeight())/2, true);
            x_cursor += bitmap_small.GetWidth() + 2;
            wxCoord label_w, label_h;
            dc.GetTextExtent(label, &label_w, &label_h);
            dc.DrawText(label, x_cursor,
                rect.y + (rect.height - label_h) / 2);
            x_cursor += label_w + 3;
            if(kind != wxRIBBON_BUTTON_NORMAL)
            {
                DrawDropdownArrow(dc, x_cursor, rect.y + rect.height / 2,
                    m_button_bar_label_colour);
            }
            break;
        }
    default:
        // TODO
        break;
    }
}

void wxRibbonMetroArtProvider::DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPageBackground(dc, wnd, rect);
}

void wxRibbonMetroArtProvider::DrawToolGroupBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
		// TODO: there is no DrawToolGroupSeparator method currently
		wxRect line;
		line.x = rect.GetRight() + 4;
		line.y = rect.GetY() + 5;
		line.width = 1;
		line.height = rect.GetHeight() - 10;
		dc.GradientFillLinear(line, wxColour(232,234,237), wxColour(178,183,189), wxSOUTH);
}

void wxRibbonMetroArtProvider::DrawTool(
                wxDC& dc,
                wxWindow* WXUNUSED(wnd),
                const wxRect& rect,
                const wxBitmap& bitmap,
                wxRibbonButtonKind kind,
                long state)
{
    if(kind == wxRIBBON_BUTTON_TOGGLE)
    {
        if(state & wxRIBBON_TOOLBAR_TOOL_TOGGLED)
            state ^= wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
    }

    wxRect bg_rect(rect);
    bg_rect.Deflate(1);
    if((state & wxRIBBON_TOOLBAR_TOOL_LAST) == 0)
        bg_rect.width++;
    
	bool is_split_hybrid = (kind == wxRIBBON_BUTTON_HYBRID && (state &
        (wxRIBBON_TOOLBAR_TOOL_HOVER_MASK | wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK)));

	dc.SetPen(m_toolbar_border_pen);

    if(state & wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK)
    {
		dc.SetBrush(m_tool_background_colour);
		dc.DrawRectangle(rect);
    }
    else if(state & wxRIBBON_TOOLBAR_TOOL_HOVER_MASK)
    {
		dc.SetBrush(m_tool_hover_background_colour);
		dc.DrawRectangle(rect);
    }

	// remove the highlight from the non-active split part of the hybrid
    if(is_split_hybrid)
    {
        wxRect nonrect(bg_rect);
        if(state & (wxRIBBON_TOOLBAR_TOOL_DROPDOWN_HOVERED |
            wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE))
        {
            nonrect.width -= 8;
        }
        else
        {
            nonrect.x += nonrect.width - 8;
            nonrect.width = 7;
        }
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_page_background_top_gradient_colour);
        dc.DrawRectangle(nonrect.x, nonrect.y, nonrect.width, nonrect.height);
    }

    // Foreground
    int avail_width = bg_rect.GetWidth();
    if(kind & wxRIBBON_BUTTON_DROPDOWN)
    {
        avail_width -= 8;
        if(is_split_hybrid)
        {
			dc.SetPen(m_toolbar_border_pen);
            dc.DrawLine(rect.x + avail_width + 1, rect.y,
                rect.x + avail_width + 1, rect.y + rect.height);
        }
        dc.DrawBitmap(m_toolbar_drop_bitmap, bg_rect.x + avail_width + 2,
            bg_rect.y + (bg_rect.height / 2) - 2, true);
    }
    dc.DrawBitmap(bitmap, bg_rect.x + (avail_width - bitmap.GetWidth()) / 2,
        bg_rect.y + (bg_rect.height - bitmap.GetHeight()) / 2, true);
}

