//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : progressctrl.cpp
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
#include "progressctrl.h"

#include "drawingutils.h"

#include <wx/dcbuffer.h>
#include <wx/settings.h>

ProgressCtrl::ProgressCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize&, long style)
    : wxPanel(parent, id, pos, wxDefaultSize, style)
    , m_maxRange(100)
    , m_currValue(0)
    , m_fillCol(wxT("DARK GREEN"))
{
    wxSize sz = wxWindow::GetTextExtent("Tp");
    SetSizeHints(wxSize(-1, sz.GetHeight() + 2));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &ProgressCtrl::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &ProgressCtrl::OnEraseBg, this);
    Bind(wxEVT_SIZE, &ProgressCtrl::OnSize, this);
}

ProgressCtrl::~ProgressCtrl()
{
    Unbind(wxEVT_PAINT, &ProgressCtrl::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &ProgressCtrl::OnEraseBg, this);
    Unbind(wxEVT_SIZE, &ProgressCtrl::OnSize, this);
}

void ProgressCtrl::OnEraseBg(wxEraseEvent& e) { wxUnusedVar(e); }

void ProgressCtrl::OnPaint(wxPaintEvent& e)
{
    wxUnusedVar(e);
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);

    wxColour brushCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    dc.SetPen(brushCol);
    dc.SetBrush(brushCol);

    dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
    wxRect rect = GetClientSize();
    // wxRect client_rect = GetClientSize();

    // draw a bordered rectangle
    dc.DrawRectangle(rect);

    // fill it with progress range
    if(m_currValue > m_maxRange) {
        m_currValue = m_maxRange;
    }

    double factor = (double)m_currValue / (double)m_maxRange;
    double fill_width = factor * rect.width;
    wxRect rr(rect);
    rr.Deflate(1, 1);
    rr.width = static_cast<int>(fill_width);

    dc.SetPen(wxPen(m_fillCol));
    dc.SetBrush(wxBrush(m_fillCol));
    dc.DrawRectangle(rr);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    // calculate the location to place the string
    wxCoord xx, yy;
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    dc.GetTextExtent(m_msg, &xx, &yy, NULL, NULL, &f);

    wxCoord txtYCoord = (rect.GetHeight() - yy) / 2;
    wxCoord txtXCoord = (rect.GetWidth() - xx) / 2; // text in the middle

    // make sure the colour used here is the system default
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    dc.DrawText(m_msg, txtXCoord, txtYCoord);
}

void ProgressCtrl::Update(size_t value, const wxString& msg)
{
    m_currValue = value;
    m_msg = msg;
    m_msg.append(wxT(" ")); // force our own copy of the string
    Refresh();
}

void ProgressCtrl::Clear()
{
    m_msg = wxEmptyString;
    m_currValue = 0;
    Refresh();
}

void ProgressCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}
