//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : drawingutils.cpp
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
#include "drawingutils.h"

#include "ColoursAndFontsManager.h"
#include "FontUtils.hpp"
#include "clScrolledPanel.h"
#include "clSystemSettings.h"
#include "globals.h"
#include "wx/dc.h"
#include "wx/settings.h"

#include <wx/app.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/graphics.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/renderer.h>
#include <wx/stc/stc.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash^M
// See http://trac.wxwidgets.org/ticket/10883^M
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

namespace
{
//////////////////////////////////////////////////
// Colour methods to convert HSL <-> RGB
//////////////////////////////////////////////////
float cl_min(float x, float y, float z)
{
    float m = x < y ? x : y;
    m = m < z ? m : z;
    return m;
}

float cl_max(float x, float y, float z)
{
    float m = x > y ? x : y;
    m = m > z ? m : z;
    return m;
}

void RGB_2_HSL(float r, float g, float b, float* h, float* s, float* l)
{
    float var_R = (r / 255.0); // RGB from 0 to 255
    float var_G = (g / 255.0);
    float var_B = (b / 255.0);

    float var_Min = cl_min(var_R, var_G, var_B); // Min. value of RGB
    float var_Max = cl_max(var_R, var_G, var_B); // Max. value of RGB
    float del_Max = var_Max - var_Min;           // Delta RGB value

    *l = (var_Max + var_Min) / 2.0;

    if(del_Max == 0) { // This is a gray, no chroma...
        *h = 0;        // HSL results from 0 to 1
        *s = 0;
    } else { // Chromatic data...
        if(*l < 0.5)
            *s = del_Max / (var_Max + var_Min);
        else
            *s = del_Max / (2.0 - var_Max - var_Min);

        float del_R = (((var_Max - var_R) / 6.0) + (del_Max / 2.0)) / del_Max;
        float del_G = (((var_Max - var_G) / 6.0) + (del_Max / 2.0)) / del_Max;
        float del_B = (((var_Max - var_B) / 6.0) + (del_Max / 2.0)) / del_Max;

        if(var_R == var_Max)
            *h = del_B - del_G;
        else if(var_G == var_Max)
            *h = (1.0 / 3.0) + del_R - del_B;
        else if(var_B == var_Max)
            *h = (2.0 / 3.0) + del_G - del_R;

        if(*h < 0)
            *h += 1;
        if(*h > 1)
            *h -= 1;
    }
}

float Hue_2_RGB(float v1, float v2, float vH) // Function Hue_2_RGB
{
    if(vH < 0)
        vH += 1;
    if(vH > 1)
        vH -= 1;
    if((6.0 * vH) < 1)
        return (v1 + (v2 - v1) * 6.0 * vH);
    if((2.0 * vH) < 1)
        return (v2);
    if((3.0 * vH) < 2)
        return (v1 + (v2 - v1) * ((2.0 / 3.0) - vH) * 6.0);
    return (v1);
}

void HSL_2_RGB(float h, float s, float l, float* r, float* g, float* b)
{
    if(s == 0) {        // HSL from 0 to 1
        *r = l * 255.0; // RGB results from 0 to 255
        *g = l * 255.0;
        *b = l * 255.0;
    } else {
        float var_2;
        if(l < 0.5)
            var_2 = l * (1.0 + s);
        else
            var_2 = (l + s) - (s * l);

        float var_1 = 2.0 * l - var_2;

        *r = 255.0 * Hue_2_RGB(var_1, var_2, h + (1.0 / 3.0));
        *g = 255.0 * Hue_2_RGB(var_1, var_2, h);
        *b = 255.0 * Hue_2_RGB(var_1, var_2, h - (1.0 / 3.0));
    }
}
} // namespace

//-------------------------------------------------------------------------------------------------
// helper functions
//-------------------------------------------------------------------------------------------------

wxColour DrawingUtils::LightColour(const wxColour& color, float percent)
{
    if(percent == 0) {
        return color;
    }

    float h, s, l, r, g, b;
    RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);

    // reduce the Lum value
    l += (float)((percent * 5.0) / 100.0);
    if(l > 1.0)
        l = 1.0;

    HSL_2_RGB(h, s, l, &r, &g, &b);
    return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

void DrawingUtils::TruncateText(const wxString& text, int maxWidth, wxDC& dc, wxString& fixedText)
{
    int textH, textW;
    int rectSize = maxWidth + 4; // error size
    // int textLen = (int)text.Length();
    wxString tempText = text;

    fixedText = wxT("");
    dc.GetTextExtent(tempText, &textW, &textH);
    if(rectSize >= textW) {
        fixedText = text;
        return;
    }

    // The text does not fit in the designated area,
    // so we need to truncate it a bit
    wxString suffix = wxT("..");
    int w, h;
    dc.GetTextExtent(suffix, &w, &h);
    rectSize -= w;

    int mid = (text.size() / 2);
    wxString text1 = text.Mid(0, mid);
    wxString text2 = text.Mid(mid);
    int min = std::min(text1.size(), text2.size());
    for(int i = 0; i < min; ++i) {
        text1.RemoveLast();
        text2.Remove(0, 1);

        fixedText = text1 + suffix + text2;
        dc.GetTextExtent(fixedText, &textW, &textH);
        if(rectSize >= textW) {
            return;
        }
    }
}

void DrawingUtils::PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor,
                                            const wxColour& endColor, bool vertical)
{
    int rd, gd, bd, high = 0;
    rd = endColor.Red() - startColor.Red();
    gd = endColor.Green() - startColor.Green();
    bd = endColor.Blue() - startColor.Blue();

    /// Save the current pen and brush
    wxPen savedPen = dc.GetPen();
    wxBrush savedBrush = dc.GetBrush();

    if(vertical)
        high = rect.GetHeight() - 1;
    else
        high = rect.GetWidth() - 1;

    if(high < 1)
        return;

    for(int i = 0; i <= high; ++i) {
        int r = startColor.Red() + ((i * rd * 100) / high) / 100;
        int g = startColor.Green() + ((i * gd * 100) / high) / 100;
        int b = startColor.Blue() + ((i * bd * 100) / high) / 100;

        wxPen p(wxColour(r, g, b));
        dc.SetPen(p);

        if(vertical)
            dc.DrawLine(rect.x, rect.y + i, rect.x + rect.width, rect.y + i);
        else
            dc.DrawLine(rect.x + i, rect.y, rect.x + i, rect.y + rect.height);
    }

    /// Restore the pen and brush
    dc.SetPen(savedPen);
    dc.SetBrush(savedBrush);
}

bool DrawingUtils::IsDark(const wxColour& color)
{
    double r = color.Red();
    double g = color.Green();
    double b = color.Blue();

    double luma = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    return (luma < 140);
}

wxColour DrawingUtils::DarkColour(const wxColour& color, float percent)
{
    if(percent == 0) {
        return color;
    }

    float h, s, l, r, g, b;
    RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);

    // reduce the Lum value
    l -= (float)((percent * 5.0) / 100.0);
    if(l < 0)
        l = 0.0;

    HSL_2_RGB(h, s, l, &r, &g, &b);
    return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

wxColour DrawingUtils::GetPanelBgColour() { return clSystemSettings::GetDefaultPanelColour(); }

wxColour DrawingUtils::GetPanelTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT); }

wxColour DrawingUtils::GetTextCtrlTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT); }

wxColour DrawingUtils::GetMenuTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT); }
#include "cl_defs.h"

wxColour DrawingUtils::GetMenuBarBgColour(bool miniToolbar)
{
    wxUnusedVar(miniToolbar);
#if defined(CL_USE_NATIVEBOOK) || defined(__WXOSX__)
    return clSystemSettings::GetDefaultPanelColour();
#else
    clTabColours c;
    c.UpdateColours(0);
    return c.activeTabBgColour;
#endif
}

void DrawingUtils::FillMenuBarBgColour(wxDC& dc, const wxRect& rect, bool miniToolbar)
{
#if defined(__WXGTK__) || defined(__WXMSW__)
    wxColour c = GetMenuBarBgColour(miniToolbar);
    dc.SetPen(c);
    dc.SetBrush(c);
    dc.DrawRectangle(rect);

#elif defined(__WXOSX__)
    wxColour c = GetMenuBarBgColour(miniToolbar);
    dc.SetPen(c);
    dc.SetBrush(c);
    dc.DrawRectangle(rect);
#else

#define RATIO 8
#define BIG_PART (RATIO - 1)

    wxColour c = GetMenuBarBgColour(miniToolbar);

    // bottom rect, 7/8 of the rect height
    wxRect bottomRect = rect;
    bottomRect.SetHeight((rect.GetHeight() / RATIO) * BIG_PART);
    bottomRect.SetBottom(rect.GetBottom());

    dc.SetPen(c);
    dc.SetBrush(c);
    dc.DrawRectangle(bottomRect);

    wxRect topRect = rect;
    topRect.height = rect.GetHeight() / RATIO;
    topRect.y = bottomRect.GetTopLeft().y;

    wxColour c1, c2;
    c1 = c;
    c2 = c1.ChangeLightness(IsDark(c) ? 70 : 100);
    PaintStraightGradientBox(dc, topRect, c2, c1, true);

#undef RATIO
#undef SMALL_PART
#undef BIG_PART

#endif
}

wxColour DrawingUtils::GetMenuBarTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT); }

wxColour DrawingUtils::GetTextCtrlBgColour() { return clSystemSettings::GetDefaultPanelColour(); }

wxColour DrawingUtils::GetOutputPaneFgColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT); }

wxColour DrawingUtils::GetOutputPaneBgColour() { return clSystemSettings::GetDefaultPanelColour(); }

wxColour DrawingUtils::GetThemeBgColour() { return GetOutputPaneBgColour(); }

wxColour DrawingUtils::GetThemeBorderColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW); }

wxColour DrawingUtils::GetThemeTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT); }

wxColour DrawingUtils::GetThemeTipBgColour()
{
    if(IsThemeDark()) {
        return GetThemeBgColour();
    } else {
        return clSystemSettings::GetDefaultPanelColour();
    }
}

bool DrawingUtils::IsThemeDark() { return IsDark(GetThemeBgColour()); }

wxDC& DrawingUtils::GetGCDC(wxDC& dc, wxGCDC& gdc)
{
    wxGraphicsRenderer* renderer = nullptr;
#if defined(__WXGTK__)
    renderer = wxGraphicsRenderer::GetCairoRenderer();
#elif defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D
    renderer = wxGraphicsRenderer::GetDirect2DRenderer();
#else
    renderer = wxGraphicsRenderer::GetDefaultRenderer();
#endif

    wxGraphicsContext* context;
    if(wxPaintDC* paintdc = wxDynamicCast(&dc, wxPaintDC)) {
        context = renderer->CreateContext(*paintdc);

    } else if(wxMemoryDC* memdc = wxDynamicCast(&dc, wxMemoryDC)) {
        context = renderer->CreateContext(*memdc);

    } else {
        return dc;
    }
    context->SetAntialiasMode(wxANTIALIAS_DEFAULT);
    gdc.SetGraphicsContext(context);
    return gdc;
}

wxColour DrawingUtils::GetAUIPaneBGColour() { return GetPanelBgColour(); }

wxBrush DrawingUtils::GetStippleBrush()
{
    wxMemoryDC memDC;
#ifdef __WXMSW__
    wxColour bgColour = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    wxBitmap bmpStipple(3, 3);
    wxColour lightPen = bgColour.ChangeLightness(105);
    wxColour darkPen = clSystemSettings::Get().SelectLightDark(bgColour.ChangeLightness(95), *wxBLACK);
#else
    wxColour bgColour = clSystemSettings::GetDefaultPanelColour();
    wxBitmap bmpStipple(3, 3);
    wxColour lightPen = bgColour.ChangeLightness(105);
    wxColour darkPen = bgColour.ChangeLightness(95);
#endif

    memDC.SelectObject(bmpStipple);
    memDC.SetBrush(bgColour);
    memDC.SetPen(bgColour);
    memDC.DrawRectangle(wxPoint(0, 0), bmpStipple.GetSize());

    /// Draw all the light points, we have 3 of them
    memDC.SetPen(lightPen);
    memDC.DrawPoint(0, 2);
    memDC.DrawPoint(2, 0);

    /// and 2 dark points
    memDC.SetPen(darkPen);
    memDC.DrawPoint(0, 1);

    memDC.SelectObject(wxNullBitmap);
    return wxBrush(bmpStipple);
}

wxColour DrawingUtils::GetThemeLinkColour()
{
    wxColour bgColour = GetThemeTipBgColour();
    if(!IsDark(bgColour)) {
        return "BLUE";

    } else {
        return "YELLOW";
    }
}

bool DrawingUtils::DrawStippleBackground(const wxRect& rect, wxDC& dc)
{
    // dark theme
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(GetStippleBrush());
    dc.DrawRectangle(rect);
    return true;
}

wxColour DrawingUtils::GetCaptionColour()
{
    wxColour defaultCaptionColour = clSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);
    return defaultCaptionColour;
}

wxFont DrawingUtils::GetFallbackFixedFont() { return FontUtils::GetDefaultMonospacedFont(); }

#ifdef __WXOSX__
double wxOSXGetMainScreenContentScaleFactor();
#endif

wxBitmap DrawingUtils::CreateDisabledBitmap(const wxBitmap& bmp)
{
    bool bDarkBG = IsDark(GetPanelBgColour());
    if(!bmp.IsOk()) {
        return wxNullBitmap;
    }
    return bmp.ConvertToDisabled(bDarkBG ? 69 : 255);
}

#define DROPDOWN_ARROW_SIZE 20

namespace
{
wxColour update_button_bg_colour(const wxColour& baseColour, eButtonState state)
{
    int bgLightness = 0;
    switch(state) {
    case eButtonState::kHover:
#ifdef __WXMSW__
        bgLightness = 140;
#else
        bgLightness = 115;
#endif
        break;
    case eButtonState::kPressed:
        bgLightness = 80;
        break;
    default:
    case eButtonState::kNormal:
        bgLightness = 100;
        break;
    }
    return baseColour.ChangeLightness(bgLightness);
}
} // namespace

void DrawingUtils::DrawButton(wxDC& dc, wxWindow* win, const wxRect& rect, const wxString& label, const wxBitmap& bmp,
                              eButtonKind kind, eButtonState state)
{
    wxDCFontChanger font_changer(dc);
    wxDCTextColourChanger text_changer(dc);

    // there are 3 rectangles involved when drawing buttons:
    // - bitmap
    // - text
    // - drop down arrow
    wxRect allocated_text_rect = rect;
    wxRect allocated_bmp_rect;
    wxRect allocated_dropdown_arrow_rect;

    // Draw the background
    wxRect clientRect = rect;

    wxColour button_bg_colour = update_button_bg_colour(GetButtonBgColour(), state);
    wxDCBrushChanger brush_changer(dc, button_bg_colour);
    wxDCPenChanger pen_changer(dc, button_bg_colour.ChangeLightness(60));

#ifdef __WXGTK__
    // translate states
    int flags = 0;
    switch(state) {
    case eButtonState::kNormal:
        flags = 0;
        break;
    case eButtonState::kDisabled:
        flags = wxCONTROL_DISABLED;
        break;
    case eButtonState::kHover:
        flags = wxCONTROL_CURRENT;
        break;
    case eButtonState::kPressed:
        flags = wxCONTROL_PRESSED;
        break;
    }
    wxRendererNative::Get().DrawPushButton(win, dc, rect, flags);
#else
    dc.DrawRectangle(clientRect);
#endif

    wxColour textColour = GetButtonTextColour();
    dc.SetTextForeground(textColour);

    if(kind == eButtonKind::kDropDown) {
        // we want a drop down to the right
        int height = rect.GetHeight();
        allocated_dropdown_arrow_rect =
            wxRect(allocated_text_rect.GetWidth() - height, allocated_text_rect.GetY(), height, height);

        // update the text rectangle
        allocated_text_rect.SetWidth(allocated_text_rect.GetWidth() - height);
    }

    // check if we have a bitmap
    if(bmp.IsOk()) {
        // bitmap is drawn on the left side of the button
        allocated_bmp_rect = allocated_text_rect;
        allocated_bmp_rect.SetWidth(allocated_text_rect.GetHeight());

        // update the text rect
        allocated_text_rect.SetX(allocated_text_rect.GetX() + allocated_text_rect.GetHeight());
        allocated_text_rect.SetWidth(allocated_text_rect.GetWidth() - allocated_text_rect.GetHeight());
    }

    // draw the bimap
    if(bmp.IsOk()) {
        // draw the bitmap
        wxRect bmp_rect(0, 0, bmp.GetScaledWidth(), bmp.GetScaledHeight());
        bmp_rect = bmp_rect.CenterIn(allocated_bmp_rect);
        dc.SetClippingRegion(allocated_bmp_rect);
        dc.DrawBitmap(bmp, bmp_rect.GetTopLeft());
        dc.DestroyClippingRegion();
    }

    // the the text
    if(!label.empty()) {
        wxRect text_rect = dc.GetTextExtent(label);
        text_rect = text_rect.CenterIn(allocated_text_rect);

        dc.SetClippingRegion(allocated_text_rect);
        dc.DrawText(label, text_rect.GetTopLeft());
        dc.DestroyClippingRegion();
    }

    // draw the dropdown
    if(kind == eButtonKind::kDropDown) {
        const wxString DROPDOWN_RECT = wxT("\u25BC");
        wxRect text_rect = dc.GetTextExtent(label);
        text_rect = text_rect.CenterIn(allocated_text_rect);

        dc.SetClippingRegion(allocated_text_rect);
        dc.DrawText(DROPDOWN_RECT, text_rect.GetTopLeft());
        dc.DestroyClippingRegion();
    }
}

wxFont DrawingUtils::GetDefaultGuiFont() { return clScrolledPanel::GetDefaultFont(); }

wxSize DrawingUtils::GetBestSize(const wxString& label, int xspacer, int yspacer)
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    memDC.SetFont(GetDefaultGuiFont());
    wxSize size = memDC.GetTextExtent(label);
    size.SetWidth(size.GetWidth() + (2 * xspacer));
    size.SetHeight(size.GetHeight() + (2 * yspacer));
    return size;
}

wxColour DrawingUtils::GetButtonBgColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE); }

wxColour DrawingUtils::GetButtonTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT); }

void DrawingUtils::DrawButtonX(wxDC& dc, wxWindow* win, const wxRect& rect, const wxColour& penColour,
                               const wxColour& bgColouur, eButtonState state, const wxString& unicode_symbol)
{
    // Calculate the circle radius:
    wxColour bg_colour = bgColouur;
    bool is_dark = IsDark(bg_colour);
    wxColour xColour = penColour;
    bool drawBackground = false;
    switch(state) {
    case eButtonState::kNormal:
        break;
    case eButtonState::kDisabled:
        drawBackground = false;
        break;
    case eButtonState::kHover:
        drawBackground = true;
        bg_colour = is_dark ? bg_colour.ChangeLightness(110) : bg_colour.ChangeLightness(90);
        break;
    case eButtonState::kPressed:
        drawBackground = true;
        bg_colour = is_dark ? bg_colour.ChangeLightness(110) : bg_colour.ChangeLightness(90);
        break;
    default:
        break;
    }

    wxRect xrect(rect);
    wxRect bgRect = rect;
    if(drawBackground) {
        bgRect.Inflate(3);
        bgRect = bgRect.CenterIn(rect);
        dc.SetBrush(bg_colour);
        dc.SetPen(bg_colour);
        dc.DrawRectangle(bgRect);
    }

    wxDCFontChanger font_changer(dc);
    wxDCTextColourChanger font_colour_changer(dc, xColour);
    wxFont font = GetDefaultGuiFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    dc.SetFont(font);

    xrect = dc.GetTextExtent(unicode_symbol);
    xrect = xrect.CenterIn(bgRect);
    dc.DrawText(unicode_symbol, xrect.GetTopLeft());
}

void DrawingUtils::DrawButtonMaximizeRestore(wxDC& dc, wxWindow* win, const wxRect& rect, const wxColour& penColour,
                                             const wxColour& bgColouur, eButtonState state)
{
#if 0
    size_t flags = 0;
    switch(state) {
    case eButtonState::kHover:
        flags = wxCONTROL_CURRENT;
        break;
    case eButtonState::kPressed:
        flags = wxCONTROL_PRESSED;
        break;
    default:
        break;
    }
    wxRendererNative::Get().DrawTitleBarBitmap(win, dc, rect, wxTITLEBAR_BUTTON_MAXIMIZE, flags);
    if(IsDark(bgColouur)) {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(bgColouur);
        dc.DrawRectangle(rect);
    }
#else
    // Calculate the circle radius:
    wxRect innerRect(rect);
    wxColour b = bgColouur;
    wxColour xColour = penColour;
    switch(state) {
    case eButtonState::kHover:
        b = b.ChangeLightness(120);
        break;
    case eButtonState::kPressed:
        b = b.ChangeLightness(70);
        xColour = b.ChangeLightness(150);
        break;
    default:
        break;
    }

    // Draw the background
    if(state != eButtonState::kNormal) {
        dc.SetPen(b);
        dc.SetBrush(b);
        dc.DrawRoundedRectangle(rect, 2.0);
    }

    // draw the x sign
    innerRect.Deflate(2);
    innerRect = innerRect.CenterIn(rect);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(wxPen(xColour, 2));
    dc.DrawRectangle(innerRect);
    innerRect.Deflate(0, 3);
    dc.DrawLine(innerRect.GetTopLeft(), innerRect.GetTopRight());
#endif
}

void DrawingUtils::DrawDropDownArrow(wxWindow* win, wxDC& dc, const wxRect& rect, const wxColour& colour)
{
    // make sure we exit this function with the font that we entered
    wxDCFontChanger font_changer(dc);

    // Draw an arrow
    const wxString arrowSymbol = wxT("\u25BC");
    dc.SetFont(GetDefaultGuiFont());

    wxRect arrowRect{ { 0, 0 }, dc.GetTextExtent(arrowSymbol) };
    arrowRect = arrowRect.CenterIn(rect);

    wxColour buttonColour = colour;
    if(!buttonColour.IsOk()) {
        // No colour provided, provide one
        buttonColour = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        buttonColour = IsDark(buttonColour) ? buttonColour.ChangeLightness(120) : buttonColour.ChangeLightness(80);
    }

    dc.SetTextForeground(buttonColour);
    dc.DrawText(arrowSymbol, arrowRect.GetTopLeft());
}

wxColour DrawingUtils::GetCaptionTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT); }

#ifdef __WXMSW__
#define X_MARGIN 6
#else
#define X_MARGIN 4
#endif

void DrawingUtils::DrawCustomChoice(wxWindow* win, wxDC& dc, const wxRect& rect, const wxString& label,
                                    const wxColour& baseColour, const wxBitmap& bmp, int align)
{
    wxRect choiceRect = rect;
    // Fill the drop down button with the custom base colour
    dc.SetPen(baseColour);
    dc.SetBrush(baseColour);
    dc.DrawRectangle(rect);

    // Create colour pallete
    clColours c;
    c.InitFromColour(baseColour);
    wxColour borderColour = c.GetBorderColour();
    wxColour arrowColour = c.GetDarkBorderColour();
    wxColour textColour = c.GetItemTextColour();

    int width = choiceRect.GetHeight();
    wxRect dropDownRect = wxRect(0, 0, width, width);
    int x = choiceRect.GetX() + choiceRect.GetWidth() - dropDownRect.GetWidth();
    dropDownRect.SetX(x);
    dropDownRect = dropDownRect.CenterIn(choiceRect, wxVERTICAL);
    dc.SetBrush(baseColour);
    dc.SetPen(borderColour);
    dc.DrawRoundedRectangle(choiceRect, 3.0);
    DrawDropDownArrow(win, dc, dropDownRect, arrowColour);

    // Common to all platforms: draw the text + bitmap
    wxRect textRect = choiceRect;
    textRect.SetWidth(textRect.GetWidth() - textRect.GetHeight());
    dc.SetClippingRegion(textRect);

    int xx = textRect.GetX() + X_MARGIN;
    if(bmp.IsOk()) {
        // Draw bitmap first
        wxRect bmpRect(xx, textRect.GetY(), bmp.GetScaledWidth(), bmp.GetScaledHeight());
        bmpRect = bmpRect.CenterIn(choiceRect, wxVERTICAL);
        dc.DrawBitmap(bmp, bmpRect.GetTopLeft());
        xx += bmpRect.GetWidth() + X_MARGIN;
    }
    dc.SetFont(GetDefaultGuiFont());
    wxSize textSize = dc.GetTextExtent(label);
    textRect.SetHeight(textSize.GetHeight());
    textRect = textRect.CenterIn(choiceRect, wxVERTICAL);
    wxString truncatedText;
    TruncateText(label, textRect.GetWidth(), dc, truncatedText);
    dc.SetTextForeground(textColour);
    dc.DrawText(truncatedText, xx, textRect.GetY());
    dc.DestroyClippingRegion();
}

void DrawingUtils::DrawNativeChoice(wxWindow* win, wxDC& dc, const wxRect& rect, const wxString& label,
                                    const wxBitmap& bmp, int align)
{
    wxRect choiceRect = rect;
#if defined(__WXMSW__) || defined(__WXGTK__)
#ifdef __WXMSW__
    int width = clSystemSettings::GetMetric(wxSYS_SMALLICON_X);
#else
    int width = choiceRect.GetHeight();
#endif
    wxRect dropDownRect = wxRect(0, 0, width, width);
    int x = choiceRect.GetX() + choiceRect.GetWidth() - dropDownRect.GetWidth();
    dropDownRect.SetX(x);
    dropDownRect = dropDownRect.CenterIn(choiceRect, wxVERTICAL);
    wxRendererNative::Get().DrawPushButton(win, dc, choiceRect, 0);
    wxRendererNative::Get().DrawDropArrow(win, dc, dropDownRect, 0);
#else
    // OSX
    wxColour bgColour = clSystemSettings::GetDefaultPanelColour();
    if(IsDark(bgColour)) {
        // On Dark theme (Mojave and later)
        int width = choiceRect.GetHeight();
        wxRect dropDownRect = wxRect(0, 0, width, width);
        int x = choiceRect.GetX() + choiceRect.GetWidth() - dropDownRect.GetWidth();
        dropDownRect.SetX(x);
        dropDownRect = dropDownRect.CenterIn(choiceRect, wxVERTICAL);
        wxColour borderColour = clSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
        dc.SetBrush(clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        dc.SetPen(borderColour);
        dc.DrawRoundedRectangle(choiceRect, 3.0);
        DrawDropDownArrow(win, dc, dropDownRect);
    } else {
        wxRendererNative::Get().DrawChoice(win, dc, choiceRect, 0);
    }
#endif

    // Common to all platforms: draw the text + bitmap
    wxRect textRect = choiceRect;
    textRect.SetWidth(textRect.GetWidth() - textRect.GetHeight());
    dc.SetClippingRegion(textRect);

    int xx = textRect.GetX() + X_MARGIN;
    if(bmp.IsOk()) {
        // Draw bitmap first
        wxRect bmpRect(xx, textRect.GetY(), bmp.GetScaledWidth(), bmp.GetScaledHeight());
        bmpRect = bmpRect.CenterIn(choiceRect, wxVERTICAL);
        dc.DrawBitmap(bmp, bmpRect.GetTopLeft());
        xx += bmpRect.GetWidth() + X_MARGIN;
    }
    dc.SetFont(GetDefaultGuiFont());
    wxSize textSize = dc.GetTextExtent(label);
    textRect.SetHeight(textSize.GetHeight());
    textRect = textRect.CenterIn(choiceRect, wxVERTICAL);
    wxString truncatedText;
    TruncateText(label, textRect.GetWidth(), dc, truncatedText);
    dc.SetTextForeground(clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    dc.DrawText(truncatedText, xx, textRect.GetY());
    dc.DestroyClippingRegion();
}

clColours& DrawingUtils::GetColours(bool darkColours)
{
    static bool once = true;
    static clColours g_darkColours;
    static clColours g_normalColours;
    if(once) {
        g_darkColours.InitDarkDefaults();
        g_normalColours.InitDefaults();
        once = false;
    }
    if(darkColours) {
        return g_darkColours;
    } else {
        return g_normalColours;
    }
}

int DrawingUtils::GetFallbackFixedFontSize() { return GetFallbackFixedFont().GetPointSize(); }
wxString DrawingUtils::GetFallbackFixedFontFace() { return GetFallbackFixedFont().GetFaceName(); }

wxRect DrawingUtils::DrawColourPicker(wxWindow* win, wxDC& dc, const wxRect& rect, const wxColour& pickerColour,
                                      eButtonState state)
{
    wxColour fixed_picker_colour = pickerColour.IsOk() ? pickerColour : *wxBLACK;
    wxString label = fixed_picker_colour.GetAsString(wxC2S_HTML_SYNTAX);

    // set the dont
    wxDCFontChanger font_changer(dc);
    wxFont f = GetDefaultGuiFont();
    dc.SetFont(f);

    wxRect text_rect = dc.GetTextExtent(label);
    text_rect = text_rect.CenterIn(rect);

    // draw button frame
    DrawButton(dc, win, rect, wxEmptyString, wxNullBitmap, eButtonKind::kNormal, state);

    // draw background colour
    wxRect bg_rect = rect;
    bg_rect.Deflate(3);
    bg_rect = bg_rect.CenterIn(rect);
    wxDCPenChanger pen_changer(dc, fixed_picker_colour.ChangeLightness(75));
    wxDCBrushChanger brush_changer(dc, fixed_picker_colour);
    dc.DrawRectangle(bg_rect);

    // draw the label
    wxColour text_colour = IsDark(fixed_picker_colour) ? *wxWHITE : *wxBLACK;

    wxDCTextColourChanger text_colour_changer(dc);

    dc.SetTextForeground(text_colour);
    dc.DrawText(label, text_rect.GetTopLeft());
    return rect;
}
