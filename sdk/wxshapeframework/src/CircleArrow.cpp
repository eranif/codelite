/***************************************************************
 * Name:      SolidArrow.cpp
 * Purpose:   Implements solid arrow for line shapes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/CircleArrow.h"
#include "wx/wxsf/CommonFcn.h"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFCircleArrow, wxSFSolidArrow);

wxSFCircleArrow::wxSFCircleArrow(void)
: wxSFSolidArrow()
{
	m_nRadius = sfdvARROW_RADIUS;
	
	XS_SERIALIZE_INT_EX( m_nRadius, wxT("radius"), sfdvARROW_RADIUS );
}

wxSFCircleArrow::wxSFCircleArrow(wxSFShapeBase* parent)
: wxSFSolidArrow(parent)
{
	m_nRadius = sfdvARROW_RADIUS;
	
	XS_SERIALIZE_INT_EX( m_nRadius, wxT("radius"), sfdvARROW_RADIUS );
}

wxSFCircleArrow::wxSFCircleArrow(const wxSFCircleArrow& obj)
: wxSFSolidArrow(obj)
{
	m_nRadius = obj.m_nRadius;
	
	XS_SERIALIZE_INT_EX( m_nRadius, wxT("radius"), sfdvARROW_RADIUS );
}

wxSFCircleArrow::~wxSFCircleArrow(void)
{
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFCircleArrow::Draw(const wxRealPoint &from, const wxRealPoint &to, wxDC& dc)
{
	wxUnusedVar(from);

	dc.SetPen(m_Pen);
    dc.SetBrush(m_Fill);
    dc.DrawCircle( Conv2Point( to ), m_nRadius );
    dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

