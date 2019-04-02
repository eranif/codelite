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
#include "clScrolledPanel.h"
#include "drawingutils.h"
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
#include "clSystemSettings.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

#ifdef __WXMSW__
#define DEFAULT_FACE_NAME "Consolas"
#define DEFAULT_FONT_SIZE 12
#elif defined(__WXMAC__)
#define DEFAULT_FACE_NAME "monaco"
#define DEFAULT_FONT_SIZE 12
#else // GTK, FreeBSD etc
#define DEFAULT_FACE_NAME "monospace"
#define DEFAULT_FONT_SIZE 12
#endif

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash^M
// See http://trac.wxwidgets.org/ticket/10883^M
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

static void RGBtoHSB(int r, int g, int b, float* h, float* s, float* br)
{
    float hue, saturation, brightness;
    int cmax = (r > g) ? r : g;
    if(b > cmax) cmax = b;
    int cmin = (r < g) ? r : g;
    if(b < cmin) cmin = b;

    brightness = ((float)cmax) / 255.0f;
    if(cmax != 0)
        saturation = ((float)(cmax - cmin)) / ((float)cmax);
    else
        saturation = 0;
    if(saturation == 0)
        hue = 0;
    else {
        float redc = ((float)(cmax - r)) / ((float)(cmax - cmin));
        float greenc = ((float)(cmax - g)) / ((float)(cmax - cmin));
        float bluec = ((float)(cmax - b)) / ((float)(cmax - cmin));
        if(r == cmax)
            hue = bluec - greenc;
        else if(g == cmax)
            hue = 2.0f + redc - bluec;
        else
            hue = 4.0f + greenc - redc;
        hue = hue / 6.0f;
        if(hue < 0) hue = hue + 1.0f;
    }
    (*h) = hue;
    (*s) = saturation;
    (*br) = brightness;
}

//////////////////////////////////////////////////
// Colour methods to convert HSL <-> RGB
//////////////////////////////////////////////////
static float cl_min(float x, float y, float z)
{
    float m = x < y ? x : y;
    m = m < z ? m : z;
    return m;
}

static float cl_max(float x, float y, float z)
{
    float m = x > y ? x : y;
    m = m > z ? m : z;
    return m;
}

static void RGB_2_HSL(float r, float g, float b, float* h, float* s, float* l)
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

        if(*h < 0) *h += 1;
        if(*h > 1) *h -= 1;
    }
}

static float Hue_2_RGB(float v1, float v2, float vH) // Function Hue_2_RGB
{
    if(vH < 0) vH += 1;
    if(vH > 1) vH -= 1;
    if((6.0 * vH) < 1) return (v1 + (v2 - v1) * 6.0 * vH);
    if((2.0 * vH) < 1) return (v2);
    if((3.0 * vH) < 2) return (v1 + (v2 - v1) * ((2.0 / 3.0) - vH) * 6.0);
    return (v1);
}

static void HSL_2_RGB(float h, float s, float l, float* r, float* g, float* b)
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

//-------------------------------------------------------------------------------------------------
// helper functions
//-------------------------------------------------------------------------------------------------

wxColour DrawingUtils::LightColour(const wxColour& color, float percent)
{
    if(percent == 0) { return color; }

    float h, s, l, r, g, b;
    RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);

    // reduce the Lum value
    l += (float)((percent * 5.0) / 100.0);
    if(l > 1.0) l = 1.0;

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
    dc.GetTextExtent(text, &textW, &textH);
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
        if(rectSize >= textW) { return; }
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

    if(high < 1) return;

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
    float h, s, b;
    RGBtoHSB(color.Red(), color.Green(), color.Blue(), &h, &s, &b);
    return (b < 0.5);
}

wxColour DrawingUtils::DarkColour(const wxColour& color, float percent)
{
    if(percent == 0) { return color; }

    float h, s, l, r, g, b;
    RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);

    // reduce the Lum value
    l -= (float)((percent * 5.0) / 100.0);
    if(l < 0) l = 0.0;

    HSL_2_RGB(h, s, l, &r, &g, &b);
    return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

wxColour DrawingUtils::GetPanelBgColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE); }

wxColour DrawingUtils::GetPanelTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT); }

wxColour DrawingUtils::GetTextCtrlTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT); }

wxColour DrawingUtils::GetMenuTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT); }

wxColour DrawingUtils::GetMenuBarBgColour(bool miniToolbar)
{
#ifdef __WXMSW__
    return clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#elif defined(__WXOSX__)
    wxUnusedVar(miniToolbar);
    return clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#else
    wxUnusedVar(miniToolbar);
    return clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
//    return miniToolbar ? clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)
//                       : clSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
#endif
}

void DrawingUtils::FillMenuBarBgColour(wxDC& dc, const wxRect& rect, bool miniToolbar)
{
#ifdef __WXMSW__

    wxColour topColour(*wxWHITE);
    wxColour brushColour(GetMenuBarBgColour(false));

    wxColour bottomColour = brushColour;
    bottomColour = bottomColour.ChangeLightness(90);

    dc.SetPen(brushColour);
    dc.SetBrush(brushColour);
    dc.DrawRectangle(rect);

    dc.SetPen(topColour);
    dc.DrawLine(rect.GetTopLeft(), rect.GetTopRight());

    dc.SetPen(bottomColour);
    dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());

#elif defined(__WXOSX__)
    wxColour bgColour = GetMenuBarBgColour(false);
    dc.SetPen(bgColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(rect);

    wxColour lineColour = bgColour;
    lineColour = lineColour.ChangeLightness(80);
    dc.SetPen(lineColour);
    dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
#else
    wxColour bgColour = GetMenuBarBgColour(miniToolbar);
    dc.SetPen(bgColour);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(rect);

    wxColour lineColour = bgColour;
    lineColour = lineColour.ChangeLightness(90);
    dc.SetPen(lineColour);
    dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
#endif
}

wxColour DrawingUtils::GetMenuBarTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT); }

wxColour DrawingUtils::GetTextCtrlBgColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_WINDOW); }

wxColour DrawingUtils::GetOutputPaneFgColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT); }

wxColour DrawingUtils::GetOutputPaneBgColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_WINDOW); }

wxColour DrawingUtils::GetThemeBgColour() { return GetOutputPaneBgColour(); }

wxColour DrawingUtils::GetThemeBorderColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW); }

wxColour DrawingUtils::GetThemeTextColour() { return clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT); }

wxColour DrawingUtils::GetThemeTipBgColour()
{
    if(IsThemeDark()) {
        return GetThemeBgColour();
    } else {
        return clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    }
}

bool DrawingUtils::IsThemeDark() { return IsDark(GetThemeBgColour()); }

bool DrawingUtils::GetGCDC(wxDC& dc, wxGCDC& gdc)
{
    wxGraphicsRenderer* const renderer = wxGraphicsRenderer::GetDefaultRenderer();
    wxGraphicsContext* context;

    if(wxPaintDC* paintdc = wxDynamicCast(&dc, wxPaintDC)) {
        context = renderer->CreateContext(*paintdc);

    } else if(wxMemoryDC* memdc = wxDynamicCast(&dc, wxMemoryDC)) {
        context = renderer->CreateContext(*memdc);

    } else {
        wxFAIL_MSG("Unknown wxDC kind");
        return false;
    }

    gdc.SetGraphicsContext(context);
    return true;
}

wxColour DrawingUtils::GetAUIPaneBGColour() { return GetPanelBgColour(); }

wxBrush DrawingUtils::GetStippleBrush()
{
    wxMemoryDC memDC;
    wxColour bgColour = GetAUIPaneBGColour();
    // if(clGetManager() && clGetManager()->GetStatusBar() && clGetManager()->GetStatusBar()->GetArt()) {
    //     bgColour = clGetManager()->GetStatusBar()->GetArt()->GetBgColour();
    // }
    wxBitmap bmpStipple(3, 3);
    wxColour lightPen = DrawingUtils::DarkColour(bgColour, 5.0);
    wxColour darkPen = DrawingUtils::LightColour(bgColour, 3.0);
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

wxFont DrawingUtils::GetDefaultFixedFont()
{
    wxFont f(GetDefaultGuiFont());
    f.SetFamily(wxFONTFAMILY_TELETYPE);
    f.SetFaceName(DEFAULT_FACE_NAME);
    // f.SetPointSize(DEFAULT_FONT_SIZE);
    return f;
}

#ifdef __WXOSX__
double wxOSXGetMainScreenContentScaleFactor();
#endif

wxBitmap DrawingUtils::CreateDisabledBitmap(const wxBitmap& bmp)
{
    bool bDarkBG = IsDark(GetPanelBgColour());
    return bmp.ConvertToDisabled(bDarkBG ? 69 : 255);
}

#define DROPDOWN_ARROW_SIZE 20

void DrawingUtils::DrawButton(wxDC& dc, wxWindow* win, const wxRect& rect, const wxString& label, const wxBitmap& bmp,
                              eButtonKind kind, eButtonState state)
{
    // Draw the background
    wxRect clientRect = rect;
    dc.SetPen(GetPanelBgColour());
    dc.SetBrush(GetPanelBgColour());
    dc.DrawRectangle(clientRect);

    // Now draw the border around this control
    // clientRect.Deflate(2);

    wxColour baseColour = GetButtonBgColour();
    wxColour textColour = GetButtonTextColour();
    wxColour penColour = baseColour.ChangeLightness(80);

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

    wxRect downRect = rect;
    downRect.SetHeight(rect.GetHeight() / 2);
    downRect.SetY(rect.GetY() + (rect.GetHeight() / 2));

    wxColour bgColour = baseColour.ChangeLightness(bgLightness);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(bgColour);
    dc.DrawRectangle(clientRect);

    dc.SetBrush(bgColour.ChangeLightness(96));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(downRect);

    dc.SetPen(penColour);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(clientRect);

    clientRect.Deflate(1);
    wxRect textRect, arrowRect;
    textRect = clientRect;
    if(kind == eButtonKind::kDropDown) {
        // we need to save space for the drop down arrow
        int xx = textRect.x + (textRect.GetWidth() - DROPDOWN_ARROW_SIZE);
        int yy = textRect.y + ((textRect.GetHeight() - DROPDOWN_ARROW_SIZE) / 2);
        textRect.SetWidth(textRect.GetWidth() - DROPDOWN_ARROW_SIZE);
        arrowRect = wxRect(xx, yy, DROPDOWN_ARROW_SIZE, DROPDOWN_ARROW_SIZE);
    }

    if(bmp.IsOk()) {
        if(label.IsEmpty()) {
            // There is no label, draw the bitmap centred
            // Bitmaps are drawn to the _left_ of the text
            int bmpX = textRect.GetX() + ((textRect.GetWidth() - bmp.GetScaledWidth()) / 2);
            int bmpY = textRect.GetY() + ((textRect.GetHeight() - bmp.GetScaledHeight()) / 2);
            dc.DrawBitmap(bmp, bmpX, bmpY);
        } else {
            // Bitmaps are drawn to the _left_ of the text
            int xx = textRect.GetX();
            xx += 5;
            int bmpY = textRect.GetY() + ((textRect.GetHeight() - bmp.GetScaledHeight()) / 2);
            dc.DrawBitmap(bmp, xx, bmpY);
            xx += bmp.GetScaledWidth();
            textRect.SetX(xx);
        }
    }

    // Draw the label
    if(!label.IsEmpty()) {
        wxString truncatedText;
        TruncateText(label, textRect.GetWidth() - 5, dc, truncatedText);
        wxSize textSize = dc.GetTextExtent(label);
        int textY = textRect.GetY() + ((textRect.GetHeight() - textSize.GetHeight()) / 2);
        dc.SetClippingRegion(textRect);
        dc.SetFont(GetDefaultGuiFont());
        dc.SetTextForeground(textColour);
        dc.DrawText(truncatedText, textRect.GetX() + 5, textY);
        dc.DestroyClippingRegion();
    }

    // Draw the drop down button
    if(kind == eButtonKind::kDropDown) {
        dc.SetPen(penColour);
        dc.SetBrush(baseColour);
        DrawDropDownArrow(win, dc, arrowRect);
        dc.SetPen(penColour);
        dc.DrawLine(arrowRect.GetX(), clientRect.GetTopLeft().y, arrowRect.GetX(), clientRect.GetBottomLeft().y);
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
    wxRendererNative::Get().DrawTitleBarBitmap(win, dc, rect, wxTITLEBAR_BUTTON_CLOSE, flags);
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
    dc.SetPen(wxPen(xColour, 2));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawLine(innerRect.GetTopLeft(), innerRect.GetBottomRight());
    dc.DrawLine(innerRect.GetTopRight(), innerRect.GetBottomLeft());
    dc.DrawPoint(innerRect.GetBottomRight());
    dc.DrawPoint(innerRect.GetBottomLeft());
#endif
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
    // Draw an arrow
    wxRect buttonRect(rect);
    int sz = wxMin(rect.GetHeight(), rect.GetWidth());
    sz = wxMin(10, sz);
    double height = ((double)sz / 3.0) * 2;
    buttonRect.SetHeight(height);
    buttonRect.SetWidth(sz);
    buttonRect = buttonRect.CenterIn(rect);
    
    wxColour buttonColour = colour;
    if(!buttonColour.IsOk()) {
        // No colour provided, provide one
        wxColour buttonFace = clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
        if(IsDark(buttonFace)) {
            buttonColour = buttonFace.ChangeLightness(150);
        } else {
            buttonColour = clSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
        }
    }
    wxPoint downCenterPoint = wxPoint(buttonRect.GetBottomLeft().x + buttonRect.GetWidth() / 2, buttonRect.GetBottom());
    dc.SetPen(wxPen(buttonColour, 2));
    dc.DrawLine(buttonRect.GetTopLeft(), downCenterPoint);
    dc.DrawLine(buttonRect.GetTopRight(), downCenterPoint);
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
    wxColour bgColour = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
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

wxFont DrawingUtils::GetBestFixedFont(IEditor* editor)
{
    wxFont defaultFont = DrawingUtils::GetDefaultFixedFont();
    wxFont bestFont = defaultFont;
    wxUnusedVar(editor);
//    if(editor) {
//        bestFont = editor->GetCtrl()->StyleGetFont(0);
//#if defined(__WXGTK__) && defined(__WXGTK3__)
//        bestFont.SetPointSize(defaultFont.GetPointSize());
//#endif
//    }
    return bestFont;
}
