/***************************************************************
 * Name:      DiamondArrow.cpp
 * Purpose:   Implements diamond arrow for line shapes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2009-04-18
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/DiamondArrow.h"

// arrow shape
static const wxRealPoint arrow[4]={wxRealPoint(0,0), wxRealPoint(10,4), wxRealPoint(20,0), wxRealPoint(10,-4)};

XS_IMPLEMENT_CLONABLE_CLASS(wxSFDiamondArrow, wxSFSolidArrow);

wxSFDiamondArrow::wxSFDiamondArrow(void)
: wxSFSolidArrow()
{
}

wxSFDiamondArrow::wxSFDiamondArrow(wxSFShapeBase* parent)
: wxSFSolidArrow(parent)
{
}

wxSFDiamondArrow::wxSFDiamondArrow(const wxSFDiamondArrow& obj)
: wxSFSolidArrow(obj)
{
}

wxSFDiamondArrow::~wxSFDiamondArrow(void)
{
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFDiamondArrow::Draw(const wxRealPoint &from, const wxRealPoint &to, wxDC& dc)
{
	wxPoint rarrow[4];
	
	TranslateArrow( rarrow, arrow, 4, from, to );

	dc.SetPen(m_Pen);
    dc.SetBrush(m_Fill);
    dc.DrawPolygon(4, rarrow);
    dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
