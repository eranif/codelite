//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : art_metro.h
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

///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/art.h
// Purpose:     Art providers for ribbon-bar-style interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_METRO_ART_H_
#define _WX_RIBBON_METRO_ART_H_

#include "codelite_exports.h"
#include "wx/bitmap.h"
#include "wx/brush.h"
#include "wx/colour.h"
#include "wx/defs.h"
#include "wx/font.h"
#include "wx/pen.h"
#include "wx/ribbon/art.h"

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_SDK wxRibbonMetroArtProvider : public wxRibbonMSWArtProvider
{
public:
    wxRibbonMetroArtProvider(bool set_colour_scheme = true);
    virtual ~wxRibbonMetroArtProvider();
    void SetFlags(long flags);

    void GetColourScheme(wxColour* primary, wxColour* secondary, wxColour* tertiary) const;

    void SetColourScheme(const wxColour& primary, const wxColour& secondary, const wxColour& tertiary);
    void DrawTabCtrlBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);

    void DrawTab(wxDC& dc, wxWindow* wnd, const wxRibbonPageTabInfo& tab);

    void DrawPageBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);

    void DrawPanelBackground(wxDC& dc, wxRibbonPanel* wnd, const wxRect& rect);

    void DrawMinimisedPanel(wxDC& dc, wxRibbonPanel* wnd, const wxRect& rect, wxBitmap& bitmap);

    void DrawButtonBarBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);

    void DrawButtonBarButton(wxDC& dc, wxWindow* wnd, const wxRect& rect, wxRibbonButtonKind kind, long state,
                             const wxString& label, const wxBitmap& bitmap_large, const wxBitmap& bitmap_small);

    void DrawToolBarBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);

    void DrawToolGroupBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);

    void DrawTool(wxDC& dc, wxWindow* wnd, const wxRect& rect, const wxBitmap& bitmap, wxRibbonButtonKind kind,
                  long state);

protected:
    // void ReallyDrawTabSeparator(wxWindow* wnd, const wxRect& rect, double visibility);
    void DrawPartialPageBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect, bool allow_hovered = true);
    void DrawPartialPageBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect, wxRibbonPage* page, wxPoint offset,
                                   bool hovered = false);
    void DrawPanelBorder(wxDC& dc, const wxRect& rect, wxPen& primary_colour, wxPen& secondary_colour);

    void DrawButtonBarButtonForeground(wxDC& dc, const wxRect& rect, wxRibbonButtonKind kind, long state,
                                       const wxString& label, const wxBitmap& bitmap_large,
                                       const wxBitmap& bitmap_small);
    void DrawMinimisedPanelCommon(wxDC& dc, wxRibbonPanel* wnd, const wxRect& rect, wxRect* preview_rect);
};

#endif // _WX_RIBBON_METRO_ART_H_
