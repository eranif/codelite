//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cc_box_tip_window.h
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

#ifndef CCBOXTIPWINDOW_H
#define CCBOXTIPWINDOW_H

#include <wx/popupwin.h> // Base class: wxPopupTransientWindow
#include <wx/panel.h>
#include <vector>
#include "codelite_exports.h"

class IEditor;
class CCBox;

WXDLLIMPEXP_SDK extern const wxEventType wxEVT_TIP_BTN_CLICKED_UP;
WXDLLIMPEXP_SDK extern const wxEventType wxEVT_TIP_BTN_CLICKED_DOWN;

class WXDLLIMPEXP_SDK CCBoxTipWindow : public wxPopupWindow
{
protected:
    class Links
    {
    public:
        wxString m_url;
        wxRect m_rect;
    };

    wxString m_tip;
    int m_lineHeight;
    wxFont m_codeFont;
    wxFont m_commentFont;
    size_t m_numOfTips;
    wxRect m_leftTipRect;
    wxRect m_rightTipRect;
    std::vector<Links> m_links;
    bool m_useLightColours;

protected:
    void OnPaint(wxPaintEvent& e);
    void OnEraseBG(wxEraseEvent& e);
    void OnMouseLeft(wxMouseEvent& e);
    wxRect DoPrintText(wxDC& dc, wxString& text, wxPoint& pt, size_t& maxWidth);
    wxString DoStripMarkups();
    void DoInitialize(const wxString& tip, size_t numOfTips, bool simpleTip);
    void DoDrawTip(wxDC& dc, size_t& max_width);

public:
    CCBoxTipWindow(wxWindow* parent, bool manipulateText, const wxString& tip, size_t numOfTips,
                   bool simpleTip = false);
    CCBoxTipWindow(wxWindow* parent, bool manipulateText, const wxString& tip);
    virtual ~CCBoxTipWindow();

    /**
     * @brief position the tip next to 'win' and show it
     * if focusEditor is NOT null, the editor will gain the focus once
     * the tip is shown
     */
    void PositionRelativeTo(wxWindow* win, wxPoint caretPos, IEditor* focusEdior = NULL);

    /**
     * @brief position this window to the left of 'win'
     * if focusEditor is NOT null, the editor will gain the focus once
     * the tip is shown
     */
    void PositionLeftTo(wxWindow* win, IEditor* focusEditor = NULL);
    /**
     * @brief position and show the tip at a given location
     */
    void PositionAt(const wxPoint& pt, IEditor* focusEdior = NULL);
};

#endif // CCBOXTIPWINDOW_H
