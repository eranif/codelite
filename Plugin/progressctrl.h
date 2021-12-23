//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : progressctrl.h
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
#ifndef __progressctrl__
#define __progressctrl__

#include "codelite_exports.h"

#include <wx/panel.h>

class WXDLLIMPEXP_SDK ProgressCtrl : public wxPanel
{

    wxString m_msg;
    size_t m_maxRange;
    size_t m_currValue;
    wxColour m_fillCol;

public:
    ProgressCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize, long style = wxBORDER_NONE);
    virtual ~ProgressCtrl();

    void Update(size_t value, const wxString& msg);
    void Clear();

    // Setters
    void SetMaxRange(const size_t& maxRange) { this->m_maxRange = maxRange; }
    void SetMsg(const wxString& msg) { this->m_msg = msg; }
    // Getters
    const size_t& GetMaxRange() const { return m_maxRange; }
    const wxString& GetMsg() const { return m_msg; }
    void SetFillCol(const wxColour& col) { m_fillCol = col; }

    void OnPaint(wxPaintEvent& e);
    void OnEraseBg(wxEraseEvent& e);
    void OnSize(wxSizeEvent& event);
};
#endif // __progressctrl__
