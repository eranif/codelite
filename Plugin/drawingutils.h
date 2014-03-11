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

#include "wx/dc.h"
#include "wx/colour.h"
#include "codelite_exports.h"
#include <wx/dcgraph.h>

class WXDLLIMPEXP_SDK DrawingUtils
{
public:
    static wxColor LightColour(const wxColour& color, float percent);
    static wxColor DarkColour (const wxColour& color, float percent);
    static wxColor GetPanelBgColour();
    static wxColor GetTextCtrlTextColour();
    static wxColor GetTextCtrlBgColour();
    static wxColor GetOutputPaneFgColour();
    static wxColor GetOutputPaneBgColour();
    static wxColor GetMenuTextColour();
    static wxColor GetMenuBarBgColour();

    static void TruncateText(const wxString& text, const int &maxWidth, wxString& fixedText);
    static void PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor, const wxColour& endColor, bool vertical);
    static void DrawVerticalButton(wxDC& dc, const wxRect& rect, const bool &focus, const bool &upperTabs, bool vertical, bool hover = false);
    static void DrawHorizontalButton(wxDC& dc, const wxRect& rect, const bool &focus, const bool &upperTabs, bool vertical, bool hover = false);
    static bool IsDark(const wxColour &col);
    static float GetDdkShadowLightFactor();
    static float GetDdkShadowLightFactor2();
    static wxColour GetGradient();
    
    /// -------------------------------------------------------------
    /// New theme related API
    /// -------------------------------------------------------------
    
    
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
     * @brief stipple brush used for painting on various wxPanels
     */
    static wxBrush GetStippleBrush();
};

#endif //DRAWINGUTILS_H
