/***************************************************************
 * Name:      RoundRectShape.cpp
 * Purpose:   Implements rounded rectangular shape class
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

#include <math.h>

#include "wx/wxsf/RoundRectShape.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/CommonFcn.h"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFRoundRectShape, wxSFRectShape);

wxSFRoundRectShape::wxSFRoundRectShape(void) : wxSFRectShape()
{
	m_nRadius = sfdvROUNDRECTSHAPE_RADIUS;

	MarkSerializableDataMembers();
}

wxSFRoundRectShape::wxSFRoundRectShape(const wxRealPoint& pos, const wxRealPoint &size, double radius, wxSFDiagramManager* manager)
: wxSFRectShape(pos, size, manager)
{
	m_nRadius = radius;

	MarkSerializableDataMembers();
}

wxSFRoundRectShape::wxSFRoundRectShape(const wxSFRoundRectShape& obj) : wxSFRectShape(obj)
{
	m_nRadius = obj.m_nRadius;

	MarkSerializableDataMembers();
}

wxSFRoundRectShape::~wxSFRoundRectShape(void)
{
}

void wxSFRoundRectShape::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(m_nRadius, wxT("radius"), sfdvROUNDRECTSHAPE_RADIUS);
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

bool wxSFRoundRectShape::Contains(const wxPoint &pos)
{
	// get original bounding box
	wxRect shpBB = GetBoundingBox();

	// calculate modified boxes
	wxRect hr(shpBB);
	hr.Deflate(0, (int)m_nRadius);
	wxRect vr(shpBB);
	vr.Deflate((int)m_nRadius, 0);

	// test whether given position is inside body rect or rounded corners
	if(hr.Contains(pos))return true;
	else if(vr.Contains(pos))return true;
	else if(IsInCircle(pos, shpBB.GetTopLeft() + wxPoint((int)m_nRadius, (int)m_nRadius)))return true;
	else if(IsInCircle(pos, shpBB.GetBottomLeft() + wxPoint((int)m_nRadius, (int)-m_nRadius)))return true;
	else if(IsInCircle(pos, shpBB.GetTopRight() + wxPoint((int)-m_nRadius, (int)m_nRadius)))return true;
	else if(IsInCircle(pos, shpBB.GetBottomRight() + wxPoint((int)-m_nRadius, (int)-m_nRadius)))return true;

	return false;
}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFRoundRectShape::DrawNormal(wxDC& dc)
{
	dc.SetPen(m_Border);
	dc.SetBrush(m_Fill);
	dc.DrawRoundedRectangle(Conv2Point(GetAbsolutePosition()), Conv2Size(m_nRectSize), m_nRadius);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFRoundRectShape::DrawHover(wxDC& dc)
{
	dc.SetPen(wxPen(m_nHoverColor, 1));
	dc.SetBrush(m_Fill);
	dc.DrawRoundedRectangle(Conv2Point(GetAbsolutePosition()), Conv2Size(m_nRectSize), m_nRadius);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFRoundRectShape::DrawHighlighted(wxDC& dc)
{
	dc.SetPen(wxPen(m_nHoverColor, 2));
	dc.SetBrush(m_Fill);
	dc.DrawRoundedRectangle(Conv2Point(GetAbsolutePosition()), Conv2Size(m_nRectSize), m_nRadius);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFRoundRectShape::DrawShadow(wxDC& dc)
{
	// HINT: overload it for custom actions...

    if( m_Fill.GetStyle() != wxTRANSPARENT )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(GetParentCanvas()->GetShadowFill());
        dc.DrawRoundedRectangle(Conv2Point(GetAbsolutePosition() + GetParentCanvas()->GetShadowOffset()), Conv2Size(m_nRectSize), m_nRadius);
        dc.SetBrush(wxNullBrush);
        dc.SetPen(wxNullPen);
    }
}

//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

bool wxSFRoundRectShape::IsInCircle(const wxPoint& pos, const wxPoint& center)
{
    return (Distance(Conv2RealPoint(center), Conv2RealPoint(pos)) <= m_nRadius);
}
