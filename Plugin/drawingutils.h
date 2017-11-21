//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : drawingutils.h
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
#ifndef DRAWINGUTILS_H
#define DRAWINGUTILS_H

#include "codelite_exports.h"
#include "wx/colour.h"
#include "wx/dc.h"
#include <wx/dcgraph.h>

struct WXDLLIMPEXP_SDK clColourPalette {
    // basic colours
    wxColour textColour;
    wxColour bgColour;
    wxColour penColour;

    // Selected item colours
    wxColour selecteTextColour;
    wxColour selectionBgColour;
};

enum class eButtonState {
    kNormal,
    kPressed,
    kHover,
};

enum class eButtonKind {
    kNormal,
    kDropDown,
};

class WXDLLIMPEXP_SDK DrawingUtils
{
public:
    static wxColour LightColour(const wxColour& color, float percent);
    static wxColour DarkColour(const wxColour& color, float percent);
    static wxColour GetPanelBgColour();
    static wxColour GetPanelTextColour();
    static wxColour GetButtonBgColour();
    static wxColour GetButtonTextColour();
    static wxColour GetTextCtrlTextColour();
    static wxColour GetTextCtrlBgColour();
    static wxColour GetOutputPaneFgColour();
    static wxColour GetOutputPaneBgColour();
    static wxColour GetMenuTextColour();
    static wxColour GetMenuBarBgColour();
    static wxColour GetMenuBarTextColour();

    static void TruncateText(const wxString& text, int maxWidth, wxDC& dc, wxString& fixedText);
    static void PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor,
                                         const wxColour& endColor, bool vertical);
    static void DrawVerticalButton(wxDC& dc, const wxRect& rect, const bool& focus, const bool& upperTabs,
                                   bool vertical, bool hover = false);
    static void DrawHorizontalButton(wxDC& dc, const wxRect& rect, const bool& focus, const bool& upperTabs,
                                     bool vertical, bool hover = false);
    static bool IsDark(const wxColour& col);
    static float GetDdkShadowLightFactor();
    static float GetDdkShadowLightFactor2();
    static wxColour GetGradient();

    static wxFont GetDefaultFixedFont();
    static wxFont GetDefaultGuiFont();
    static wxBitmap CreateDisabledBitmap(const wxBitmap& bmp);
    static wxBitmap CreateGrayBitmap(const wxBitmap& bmp);
    static wxSize GetBestSize(const wxString& label, int xspacer = 5, int yspacer = 5);

    /**
     * @brief return basic colour palette based on the current editor
     */
    static clColourPalette GetColourPalette();

    /**
     * @brief draw a button
     */
    static void DrawButton(wxDC& dc, wxWindow* win, const wxRect& rect, const wxString& label, const wxBitmap& bmp,
                           eButtonKind kind, eButtonState state);

    /**
     * @brief draw a close button
     */
    static void DrawButtonX(wxDC& dc, wxWindow* win, const wxRect& rect, const wxColour& penColour,
                            eButtonState state);
    
    /**
     * @brief draw a drop down arrow
     */
    static void DrawDropDownArrow(wxWindow* win, wxDC& dc, const wxRect& rect, const wxColour& colour);
    
    /// -------------------------------------------------------------
    /// New theme related API
    /// -------------------------------------------------------------

    /**
     * @brief draw a standard codelite background colour
     * @param rect
     * @param dc
     */
    static bool DrawStippleBackground(const wxRect& rect, wxDC& dc);

    /**
     * @brief convert wxDC into wxGCDC
     * @param dc [in] dc
     * @param gdc [in/out] graphics DC
     * @return true on success, false otherwise
     */
    static bool GetGCDC(wxDC& dc, wxGCDC& gdc);

    /**
     * @brief Return true if the current theme dominant colour is dark
     */
    static bool IsThemeDark();

    /// Return the proper colour for painting panel's bg colour
    static wxColour GetThemeBgColour();

    /// Return the proper colour for painting panel's border colour
    static wxColour GetThemeBorderColour();

    /// Return the proper colour for painting panel's text
    static wxColour GetThemeTextColour();

    /// Return the proper colour for painting tooltip bg colour
    static wxColour GetThemeTipBgColour();

    /// Return the proper colour for painting URL link
    static wxColour GetThemeLinkColour();

    /**
     * @brief return the AUI pane bg colour
     */
    static wxColour GetAUIPaneBGColour();

    /**
     * @brief get the caption colour
     */
    static wxColour GetCaptionColour();

    /**
     * @brief stipple brush used for painting on various wxPanels
     */
    static wxBrush GetStippleBrush();
};

#endif // DRAWINGUTILS_H
