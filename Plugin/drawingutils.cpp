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
#include <wx/app.h>
#include <wx/panel.h>
#include "wx/settings.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "wx/dc.h"
#include <wx/graphics.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include "globals.h"
#include "ieditor.h"
#include <wx/stc/stc.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

#ifdef __WXMSW__
#define DEFAULT_FACE_NAME "Consolas"
#define DEFAULT_FONT_SIZE 11
#elif defined(__WXMAC__)
#define DEFAULT_FACE_NAME "monaco"
#define DEFAULT_FONT_SIZE 12
#else // GTK, FreeBSD etc
#define DEFAULT_FACE_NAME "monospace"
#define DEFAULT_FONT_SIZE 11
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

wxColor DrawingUtils::LightColour(const wxColour& color, float percent)
{
    if(percent == 0) {
        return color;
    }

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
    int textLen = (int)text.Length();
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

    for(int i = textLen; i >= 0; i--) {
        dc.GetTextExtent(tempText, &textW, &textH);
        if(rectSize > textW) {
            fixedText = tempText;
            fixedText.RemoveLast(2); // remove last 2 chars, make room for the ".."
            fixedText += wxT("..");
            return;
        }
        tempText = tempText.RemoveLast();
    }
}

void DrawingUtils::PaintStraightGradientBox(
    wxDC& dc, const wxRect& rect, const wxColour& startColor, const wxColour& endColor, bool vertical)
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

        wxPen p(wxColor(r, g, b));
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

void DrawingUtils::DrawVerticalButton(
    wxDC& dc, const wxRect& rect, const bool& focus, const bool& leftTabs, bool vertical, bool hover)
{
    wxColour lightGray = GetGradient();

    // Define the rounded rectangle base on the given rect
    // we need an array of 9 points for it
    wxColour topStartColor(wxT("WHITE"));
    wxColour topEndColor(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    // Define the middle points
    if(focus) {
        PaintStraightGradientBox(dc, rect, topStartColor, topEndColor, vertical);
    } else {
        wxRect r1;
        wxRect r2;

        topStartColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        topEndColor = lightGray;

        if(leftTabs) {
            r1 = wxRect(rect.x, rect.y, rect.width, rect.height / 4);
            r2 = wxRect(rect.x, rect.y + rect.height / 4, rect.width, (rect.height * 3) / 4);
            PaintStraightGradientBox(dc, r1, topEndColor, topStartColor, vertical);
            PaintStraightGradientBox(dc, r2, topStartColor, topStartColor, vertical);

        } else {
            r1 = wxRect(rect.x, rect.y, rect.width, (rect.height * 3) / 4);
            r2 = wxRect(rect.x, rect.y + (rect.height * 3) / 4, rect.width, rect.height / 4);
            PaintStraightGradientBox(dc, r1, topStartColor, topStartColor, vertical);
            PaintStraightGradientBox(dc, r2, topStartColor, topEndColor, vertical);
        }
    }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
}

void DrawingUtils::DrawHorizontalButton(
    wxDC& dc, const wxRect& rect, const bool& focus, const bool& upperTabs, bool vertical, bool hover)
{
    wxColour lightGray = GetGradient();
    wxColour topStartColor(wxT("WHITE"));
    wxColour topEndColor(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    // Define the middle points
    if(focus) {
        if(upperTabs) {
            PaintStraightGradientBox(dc, rect, topStartColor, topEndColor, vertical);
        } else {
            PaintStraightGradientBox(dc, rect, topEndColor, topStartColor, vertical);
        }
    } else {

        topStartColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        topEndColor = lightGray;

        wxRect r1;
        wxRect r2;

        if(upperTabs) {
            r1 = wxRect(rect.x, rect.y, rect.width, rect.height / 4);
            r2 = wxRect(rect.x, rect.y + rect.height / 4, rect.width, (rect.height * 3) / 4);
            PaintStraightGradientBox(dc, r1, topEndColor, topStartColor, vertical);
            PaintStraightGradientBox(dc, r2, topStartColor, topStartColor, vertical);

        } else {
            r1 = wxRect(rect.x, rect.y, rect.width, (rect.height * 3) / 4);
            r2 = wxRect(rect.x, rect.y + (rect.height * 3) / 4, rect.width, rect.height / 4);
            PaintStraightGradientBox(dc, r1, topStartColor, topStartColor, vertical);
            PaintStraightGradientBox(dc, r2, topStartColor, topEndColor, vertical);
        }
    }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
}

bool DrawingUtils::IsDark(const wxColour& color)
{
    float h, s, b;
    RGBtoHSB(color.Red(), color.Green(), color.Blue(), &h, &s, &b);
    return (b < 0.5);
}

float DrawingUtils::GetDdkShadowLightFactor()
{
#if defined(__WXGTK__)
    return 12.0;
#else
    return 8.0;
#endif
}

float DrawingUtils::GetDdkShadowLightFactor2()
{
#if defined(__WXGTK__)
    return 9.0;
#else
    return 3.0;
#endif
}

wxColour DrawingUtils::GetGradient()
{
    //#if defined (__WXGTK__)
    return LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 4.0);
    //	return  wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    //#else
    //	return LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW), GetDdkShadowLightFactor());
    //#endif
}

wxColor DrawingUtils::DarkColour(const wxColour& color, float percent)
{
    if(percent == 0) {
        return color;
    }

    float h, s, l, r, g, b;
    RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);

    // reduce the Lum value
    l -= (float)((percent * 5.0) / 100.0);
    if(l < 0) l = 0.0;

    HSL_2_RGB(h, s, l, &r, &g, &b);
    return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

wxColor DrawingUtils::GetPanelBgColour()
{
#ifdef __WXGTK__
    static bool intitialized(false);
    static wxColour bgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    if(!intitialized) {
        // try to get the background colour from a menu
        GtkWidget* menu = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#ifdef __WXGTK3__
        GdkRGBA col;
        GtkStyleContext* context = gtk_widget_get_style_context(menu);
        gtk_style_context_get_background_color(context, GTK_STATE_FLAG_NORMAL, &col);
        bgColour = wxColour(col);
#else
        GtkStyle* def = gtk_rc_get_style(menu);
        if(!def) def = gtk_widget_get_default_style();

        if(def) {
            GdkColor col = def->bg[GTK_STATE_NORMAL];
            bgColour = wxColour(col);
        }
#endif
        gtk_widget_destroy(menu);
        intitialized = true;
    }
    return bgColour;
#else
    return wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#endif
}

wxColor DrawingUtils::GetTextCtrlTextColour()
{
#ifdef __WXGTK__
    static bool intitialized(false);
    static wxColour textColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

    if(!intitialized) {
        // try to get the text colour from a textctrl
        GtkWidget* textCtrl = gtk_text_view_new();
#ifdef __WXGTK3__
        GdkRGBA col;
        GtkStyleContext* context = gtk_widget_get_style_context(textCtrl);
        gtk_style_context_get_color(context, GTK_STATE_FLAG_NORMAL, &col);
        textColour = wxColour(col);
#else
        GtkStyle* def = gtk_rc_get_style(textCtrl);
        if(!def) def = gtk_widget_get_default_style();

        if(def) {
            GdkColor col = def->text[GTK_STATE_NORMAL];
            textColour = wxColour(col);
        }
#endif
        gtk_widget_destroy(textCtrl);
        intitialized = true;
    }
    return textColour;
#else
    return wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
#endif
}

wxColor DrawingUtils::GetMenuTextColour()
{
#ifdef __WXGTK__
    static bool intitialized(false);
    static wxColour textColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));

    if(!intitialized) {
        // try to get the text colour from a menu
        GtkWidget* menuBar = gtk_menu_new();
#ifdef __WXGTK3__
        GdkRGBA col;
        GtkStyleContext* context = gtk_widget_get_style_context(menuBar);
        gtk_style_context_get_color(context, GTK_STATE_FLAG_NORMAL, &col);
        textColour = wxColour(col);
#else
        GtkStyle* def = gtk_rc_get_style(menuBar);
        if(!def) def = gtk_widget_get_default_style();

        if(def) {
            GdkColor col = def->text[GTK_STATE_NORMAL];
            textColour = wxColour(col);
        }
#endif
        gtk_widget_destroy(menuBar);
        intitialized = true;
    }
    return textColour;
#else
    return wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT);
#endif
}

wxColor DrawingUtils::GetMenuBarBgColour()
{
#ifdef __WXGTK__
    static bool intitialized(false);
    static wxColour textColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));

    if(!intitialized) {
        // try to get the background colour from a menu
        GtkWidget* menuBar = gtk_menu_bar_new();
#ifdef __WXGTK3__
        GdkRGBA col;
        GtkStyleContext* context = gtk_widget_get_style_context(menuBar);
        gtk_style_context_get_background_color(context, GTK_STATE_FLAG_NORMAL, &col);
        textColour = wxColour(col);
#else
        GtkStyle* def = gtk_rc_get_style(menuBar);
        if(!def) def = gtk_widget_get_default_style();

        if(def) {
            GdkColor col = def->bg[GTK_STATE_NORMAL];
            textColour = wxColour(col);
        }
#endif
        gtk_widget_destroy(menuBar);
        intitialized = true;
    }
    return textColour;
#else
    return wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
#endif
}

wxColor DrawingUtils::GetTextCtrlBgColour()
{
#ifdef __WXGTK__
    // static bool     intitialized(false);
    static wxColour bgCol(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    //	if( !intitialized ) {
    //		// try to get the background colour from a textctrl
    //		GtkWidget *textCtrl = gtk_text_view_new();
    //		GtkStyle   *def = gtk_rc_get_style( textCtrl );
    //		if(!def)
    //			def = gtk_widget_get_default_style();
    //
    //		if(def) {
    //			GdkColor col = def->bg[GTK_STATE_NORMAL];
    //			bgCol = wxColour(col);
    //		}
    //		gtk_widget_destroy( textCtrl );
    //		intitialized = true;
    //	}
    return bgCol;

#else
    return wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#endif
}

wxColor DrawingUtils::GetOutputPaneFgColour()
{
    wxString col = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    if(col.IsEmpty()) {
        return GetTextCtrlTextColour();
    }

    return wxColour(col);
}

wxColor DrawingUtils::GetOutputPaneBgColour()
{
    wxString col = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    if(col.IsEmpty()) {
        return GetTextCtrlBgColour();
    }

    return wxColour(col);
}

wxColour DrawingUtils::GetThemeBgColour() { return GetOutputPaneBgColour(); }

wxColour DrawingUtils::GetThemeBorderColour()
{
    wxColour bgColour = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    if(!IsDark(bgColour)) {
        return wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);

    } else {
        return DrawingUtils::LightColour(bgColour, 4.0);
    }
}

wxColour DrawingUtils::GetThemeTextColour() { return EditorConfigST::Get()->GetCurrentOutputviewFgColour(); }

wxColour DrawingUtils::GetThemeTipBgColour()
{
    if(IsThemeDark()) {
        return GetThemeBgColour();
    } else {
#if 0
        return wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK);
#else
        return wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#endif
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

wxColour DrawingUtils::GetAUIPaneBGColour()
{
    // Now set the bg colour. It must be done after setting
    // the pen colour
    wxColour bgColour = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    if(!DrawingUtils::IsDark(bgColour)) {
        bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    } else {
        bgColour = DrawingUtils::LightColour(bgColour, 3.0);
    }
    return bgColour;
}

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
#ifdef __WXMSW__
    wxRegKey re(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\DWM");

    unsigned long colVal = -1;
    if(re.Exists() && re.QueryValue("ColorizationColor", (long*)&colVal)) {
        // Colour format is: 0xAARRGGBB
        int r = (colVal >> 16) & 0xff;
        int g = (colVal >> 8) & 0xff;
        int b = colVal & 0xff;
        return wxColour(r, g, b);
    } else {
        return wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);
    }
#else
    return wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);
#endif
}

wxFont DrawingUtils::GetDefaultFixedFont()
{
    return wxFont(wxFontInfo(DEFAULT_FONT_SIZE).Family(wxFONTFAMILY_TELETYPE).FaceName(DEFAULT_FACE_NAME));
}

clColourPalette DrawingUtils::GetColourPalette()
{
    // basic dark colours
    clColourPalette palette;
    palette.bgColour = wxColour("rgb(64, 64, 64)");
    palette.penColour = wxColour("rgb(100, 100, 100)");
    palette.selecteTextColour = *wxWHITE;
    palette.selectionBgColour = wxColour("rgb(87, 87, 87)");
    palette.textColour = wxColour("rgb(200, 200, 200)");

    if(::clGetManager()) {
        IEditor* editor = ::clGetManager()->GetActiveEditor();
        if(editor && !IsDark(editor->GetCtrl()->StyleGetBackground(0))) {
            palette.bgColour = wxColour("rgb(230, 230, 230)");
            palette.penColour = wxColour("rgb(207, 207, 207)");
            palette.selecteTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
            palette.selectionBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
            palette.textColour = wxColour("rgb(0, 0, 0)");
        }
    }
    return palette;
}
