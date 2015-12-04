//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : OneArrow.cpp
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

#include "OneArrow.h"

// arrow shape
static const wxRealPoint arrow1[2] = {wxRealPoint(5,4), wxRealPoint(5,-4)};
static const wxRealPoint arrow2[2] = {wxRealPoint(10,4), wxRealPoint(10,-4)};

XS_IMPLEMENT_CLONABLE_CLASS(OneArrow, wxSFSolidArrow);

OneArrow::OneArrow(void)
{
}
OneArrow::OneArrow(const OneArrow& obj): wxSFSolidArrow(obj) 
{
}
OneArrow::OneArrow(wxSFShapeBase* parent):wxSFSolidArrow(parent)
{
}
OneArrow::~OneArrow()
{
}

void OneArrow::Draw(const wxRealPoint& from, const wxRealPoint& to, wxDC& dc)
{
	wxPoint rarrow1[2];
	wxPoint rarrow2[2];
	
	
	TranslateArrow( rarrow1, arrow1, 2, from, to );
	TranslateArrow( rarrow2, arrow2, 2, from, to );
	
	dc.SetPen(m_Pen);
    dc.SetBrush(wxNullBrush);
    dc.DrawLines(2, rarrow1);
	dc.DrawLines(2, rarrow2);
    dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}