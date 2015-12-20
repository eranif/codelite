//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : NArrow.cpp
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

#include "NArrow.h"

// arrow shape
static const wxRealPoint arrow[6]={wxRealPoint(0,4), wxRealPoint(10,0),wxRealPoint(10,4),wxRealPoint(10,-4),wxRealPoint(10,0), wxRealPoint(0,-4)};

XS_IMPLEMENT_CLONABLE_CLASS(NArrow, wxSFSolidArrow);


NArrow::NArrow(void):wxSFSolidArrow()
{
}

NArrow::NArrow(const NArrow& obj): wxSFSolidArrow(obj) 
{
}

NArrow::NArrow(wxSFShapeBase* parent):wxSFSolidArrow(parent)
{
}

NArrow::~NArrow()
{
}

void NArrow::Draw(const wxRealPoint& from, const wxRealPoint& to, wxDC& dc)
{
	wxPoint rarrow[6];
	
	TranslateArrow( rarrow, arrow, 6, from, to );

	dc.SetPen(m_Pen);
    dc.SetBrush(wxNullBrush);
    dc.DrawLines(6, rarrow);
    dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
