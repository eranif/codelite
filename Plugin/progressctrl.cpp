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
#include <wx/settings.h>
#include "progressctrl.h"
#include <wx/dcbuffer.h>
#include "drawingutils.h"

BEGIN_EVENT_TABLE(ProgressCtrl, wxPanel)
EVT_PAINT(ProgressCtrl::OnPaint)
EVT_ERASE_BACKGROUND(ProgressCtrl::OnEraseBg)
END_EVENT_TABLE()

ProgressCtrl::~ProgressCtrl() {}

ProgressCtrl::ProgressCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize&, long style)
    : wxPanel(parent, id, pos, wxSize(-1, 24), style)
    , m_maxRange(100)
    , m_currValue(0)
    , m_fillCol(wxT("DARK GREEN"))
{
}

void ProgressCtrl::OnEraseBg(wxEraseEvent& e) { wxUnusedVar(e); }

void ProgressCtrl::OnPaint(wxPaintEvent& e)
{
    wxUnusedVar(e);
    wxBufferedPaintDC dc(this);

    wxColour brushCol = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    dc.SetPen(brushCol);
    dc.SetBrush(brushCol);

    dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
    wxRect rect = GetClientSize();
    //wxRect client_rect = GetClientSize();

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

    //dc.SetPen(*wxWHITE_PEN);
    //dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
    //dc.DrawLine(rect.GetTopRight(), rect.GetBottomRight());
    //
    //dc.DrawPoint(client_rect.GetBottomRight());
    //rect.Deflate(1, 1);
    //
    //dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    //dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
    //dc.DrawLine(rect.GetTopRight(), rect.GetBottomRight());
    //
    //dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));
    //dc.DrawLine(rect.GetBottomLeft(), rect.GetTopLeft());
    //dc.DrawLine(rect.GetTopLeft(), rect.GetTopRight());

    // calculate the location to place the string
    wxCoord xx, yy;
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    dc.GetTextExtent(m_msg, &xx, &yy, NULL, NULL, &f);

    wxCoord txtYCoord = (rect.GetHeight() - yy) / 2;
    wxCoord txtXCoord = 5;

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
