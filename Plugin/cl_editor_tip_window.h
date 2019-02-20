//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_editor_tip_window.h
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

#ifndef CLFUNCTIONTIPWINDOW_H
#define CLFUNCTIONTIPWINDOW_H

#include <wx/panel.h> // Base class: wxPanel
#include <wx/bitmap.h>
#include <vector>
#include "cl_calltip.h"
#include "codelite_exports.h"
#include "cl_command_event.h"
#include <wx/arrstr.h>

class WXDLLIMPEXP_SDK clEditorTipWindow : public wxPanel
{
public:
    struct TipInfo {
        clCallTipPtr tip;
        int highlightIndex;
    };

protected:
    std::vector<TipInfo> m_tips;
    int m_highlighIndex;
    wxString m_tipText;
    wxPoint m_point;
    int m_lineHeight = 0;
    wxColour m_parentBgColour;
    wxString m_selectedSignature;
    wxFont m_font;
    wxArrayString m_args;
    wxString m_footer; // the line that says  "1 of 2"
    wxString m_header; // The return value line
    
protected:
    wxSize DoGetTipSize();
    int DoGetTextLen(wxDC& dc, const wxString& txt);
    void DoAdjustPosition();
    void DoLayoutTip();
    /**
     * @brief make the tooltip span over multiple lines
     */
    void DoMakeMultipleLineTip();

public:
    clEditorTipWindow(wxWindow* parent);
    virtual ~clEditorTipWindow();

    // API
    ///////////////////////////////////////////////////
    void AddCallTip(clCallTipPtr tip);
    void SelectSignature(const wxString& signature);
    void Remove();
    clCallTipPtr GetTip();
    bool IsEmpty();
    void Clear();
    bool IsActive();
    void Activate(wxPoint pt, int lineHeight, wxColour parentBgColour);
    void Deactivate();
    void SelectNext(int argIdxToHilight);
    void SelectPrev(int argIdxToHilight);
    void Highlight(int argIdxToHilight);
    wxString GetText();

    DECLARE_EVENT_TABLE()
    void OnPaint(wxPaintEvent& e);
    void OnEraseBg(wxEraseEvent& e);
    void OnLeftDown(wxMouseEvent& e);
    void OnEditoConfigChanged(clCommandEvent& e);
};

#endif // CLFUNCTIONTIPWINDOW_H
