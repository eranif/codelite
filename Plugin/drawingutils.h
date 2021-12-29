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

#include "clColours.h"
#include "codelite_exports.h"
#include "ieditor.h"
#include "wx/colour.h"
#include "wx/dc.h"

#include <wx/dcgraph.h>

enum class eButtonState {
    kNormal,
    kPressed,
    kHover,
    kDisabled,
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
    static wxColour GetMenuBarBgColour(bool miniToolbar = true);
    static wxColour GetMenuBarTextColour();
    static void FillMenuBarBgColour(wxDC& dc, const wxRect& rect, bool miniToolbar = true);
    static void TruncateText(const wxString& text, int maxWidth, wxDC& dc, wxString& fixedText);
    static void PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor,
                                         const wxColour& endColor, bool vertical);
    static bool IsDark(const wxColour& col);
    /**
     * @brief return the default fixed font. We use the ColoursAndFontsManager to pick
     * the best font
     */
    static wxFont GetDefaultFixedFont();

    /**
     * @brief return a fallback fixed font incase we could not locate one in the
     * settings
     * @param win a window object. requires for DPI calculations
     */
    static wxFont GetFallbackFixedFont(const wxWindow* win, bool bold = false, bool italic = false);

    static int GetFallbackFixedFontSize(const wxWindow* win);
    static const wxString& GetFallbackFixedFontFace();
    static int FixFontSize(int size, const wxWindow* win);

    static wxFont GetBestFixedFont(IEditor* editor = nullptr);
    static wxFont GetDefaultGuiFont();
    static wxBitmap CreateDisabledBitmap(const wxBitmap& bmp);
    static wxSize GetBestSize(const wxString& label, int xspacer = 5, int yspacer = 5);

    /**
     * @brief draw a button
     */
    static void DrawButton(wxDC& dc, wxWindow* win, const wxRect& rect, const wxString& label, const wxBitmap& bmp,
                           eButtonKind kind, eButtonState state);

    /**
     * @brief draw a close button
     */
    static void DrawButtonX(wxDC& dc, wxWindow* win, const wxRect& rect, const wxColour& penColour,
                            const wxColour& bgColouur, eButtonState state);

    /**
     * @brief draw a close button
     */
    static void DrawButtonMaximizeRestore(wxDC& dc, wxWindow* win, const wxRect& rect, const wxColour& penColour,
                                          const wxColour& bgColouur, eButtonState state);

    /**
     * @brief draw a drop down arrow
     * pass an invalid colour to let this function determine the best colour to use
     */
    static void DrawDropDownArrow(wxWindow* win, wxDC& dc, const wxRect& rect, const wxColour& colour = wxColour());

    static void DrawNativeChoice(wxWindow* win, wxDC& dc, const wxRect& rect, const wxString& label,
                                 const wxBitmap& bmp = wxNullBitmap,
                                 int align = wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);

    static void DrawCustomChoice(wxWindow* win, wxDC& dc, const wxRect& rect, const wxString& label,
                                 const wxColour& baseColour = wxNullColour, const wxBitmap& bmp = wxNullBitmap,
                                 int align = wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
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
     * @brief convert wxDC into wxGCDC. Return the a DC to work with.
     * This function never fails
     */
    static wxDC& GetGCDC(wxDC& dc, wxGCDC& gdc);

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
     * @brief return the colour suitable for drawing on the caption
     */
    static wxColour GetCaptionTextColour();

    /**
     * @brief stipple brush used for painting on various wxPanels
     */
    static wxBrush GetStippleBrush();

    /**
     * @brief get colours object
     */
    static clColours& GetColours(bool darkColours = false);
};

#endif // DRAWINGUTILS_H
