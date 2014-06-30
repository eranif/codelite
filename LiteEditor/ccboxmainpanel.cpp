//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : ccboxmainpanel.cpp
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

#include "ccboxmainpanel.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(CCBoxMainPanel, wxPanel)
EVT_PAINT(CCBoxMainPanel::OnPaint)
EVT_ERASE_BACKGROUND(CCBoxMainPanel::OnErasebg)
END_EVENT_TABLE()

CCBoxMainPanel::CCBoxMainPanel(wxWindow* win)
: wxPanel(win)
{
}

CCBoxMainPanel::~CCBoxMainPanel()
{
}

void CCBoxMainPanel::OnErasebg(wxEraseEvent& e)
{
}

void CCBoxMainPanel::OnPaint(wxPaintEvent& e)
{
	wxBufferedPaintDC dc(this);
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	dc.DrawRectangle(0, 0, this->GetSize().x, this->GetSize().y);
}

