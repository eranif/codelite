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

#include "codelite_exports.h"

#include <vector>
#include <wx/panel.h>
#include <wx/popupwin.h> // Base class: wxPopupTransientWindow

class IEditor;
class CCBox;

WXDLLIMPEXP_SDK extern const wxEventType wxEVT_TIP_BTN_CLICKED_UP;
WXDLLIMPEXP_SDK extern const wxEventType wxEVT_TIP_BTN_CLICKED_DOWN;

class WXDLLIMPEXP_SDK CCBoxTipWindow : public wxPopupWindow
{
protected:
    wxString m_tip;
    size_t m_numOfTips = 1;
    bool m_stripHtmlTags = false;

protected:
    void OnPaint(wxPaintEvent& e);
    void OnEraseBG(wxEraseEvent& e);
    void DoInitialize(size_t numOfTips);
    void DoDrawTip(wxDC& dc);

    void ShrinkToScreen(wxSize& size) const;

public:
    CCBoxTipWindow(wxWindow* parent, const wxString& tip, bool strip_html_tags);
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
