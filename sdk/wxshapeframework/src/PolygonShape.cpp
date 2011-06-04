/***************************************************************
 * Name:      PolygonShape.cpp
 * Purpose:   Implements polygonial shape class
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

#include "wx/wxsf/PolygonShape.h"
#include "wx/wxsf/CommonFcn.h"
#include "wx/wxsf/ShapeCanvas.h"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFPolygonShape, wxSFRectShape);

wxSFPolygonShape::wxSFPolygonShape(void)
: wxSFRectShape()
{
	m_fConnectToVertex = sfdvPOLYGONSHAPE_VERTEXCONNECTIONS;

	MarkSerializableDataMembers();
}

wxSFPolygonShape::wxSFPolygonShape(int n, const wxRealPoint pts[], const wxRealPoint& pos, wxSFDiagramManager* manager)
: wxSFRectShape(pos, wxRealPoint(1, 1), manager)
{
	m_fConnectToVertex = sfdvPOLYGONSHAPE_VERTEXCONNECTIONS;

	MarkSerializableDataMembers();

	SetVertices(n, pts);
}

wxSFPolygonShape::wxSFPolygonShape(const wxSFPolygonShape& obj)
: wxSFRectShape(obj)
{
	m_fConnectToVertex = obj.m_fConnectToVertex;

	MarkSerializableDataMembers();

	m_arrVertices.Clear();
	for(size_t i = 0; i < obj.m_arrVertices.Count(); i++)m_arrVertices.Add(obj.m_arrVertices[i]);
}

wxSFPolygonShape::~wxSFPolygonShape(void)
{
}

void wxSFPolygonShape::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(m_fConnectToVertex, wxT("connect_to_vertex"), sfdvPOLYGONSHAPE_VERTEXCONNECTIONS);
    XS_SERIALIZE(m_arrVertices, wxT("vertices"));
}

//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

void wxSFPolygonShape::SetVertices(size_t n, const wxRealPoint pts[])
{
	m_arrVertices.Clear();

	for(size_t i = 0; i < n; i++)m_arrVertices.Add(pts[i]);

	NormalizeVertices();
	FitBoundingBoxToVertices();
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

wxRealPoint wxSFPolygonShape::GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end)
{
    // HINT: override it for custom actions ...

	bool fSuccess = false;
	double tmpMinDist = 0, minDist = 0;
	wxRealPoint tmpIntersection, intersection;
	size_t ptsCnt = m_arrVertices.Count();

	wxRealPoint *pts = new wxRealPoint[ptsCnt];
	GetTranslatedVerices(pts);

	intersection = start; //GetCenter();

	if(ptsCnt == 0)return GetCenter();

	if(m_fConnectToVertex)
	{
		minDist = Distance(pts[0], end);
		intersection = pts[0];

		for(size_t i = 1; i < ptsCnt; i++)
		{
			tmpMinDist = Distance(pts[i], end);
			if(tmpMinDist < minDist)
			{
				minDist = tmpMinDist;
				intersection = pts[i];
			}
		}

		delete [] pts;
		return intersection;
	}
	else
	{
		for(size_t i = 0; i < ptsCnt; i++)
		{
			if(LinesIntersection(pts[i], pts[(i+1) % ptsCnt], start, end, tmpIntersection))
			{
				if(!fSuccess)
				{
					minDist = Distance(intersection, end);
					intersection = tmpIntersection;
				}
				else
				{
					tmpMinDist = Distance(intersection, end);
					if(tmpMinDist < minDist)
					{
						minDist = tmpMinDist;
						intersection = tmpIntersection;
					}
				}
				fSuccess = true;
			}
		}

		delete [] pts;

		if(fSuccess)
		{
			return intersection;
		}
		else
		{
			return GetCenter();
		}
	}
}

void wxSFPolygonShape::FitToChildren()
{
    wxSFRectShape::FitToChildren();

    FitVerticesToBoundingBox();
}

void wxSFPolygonShape::Scale(double x, double y, bool children)
{
	m_nRectSize.x *= x;
	m_nRectSize.y *= y;

	FitVerticesToBoundingBox();

    // call default function implementation (needed for scaling of shape's children)
    wxSFShapeBase::Scale(x, y, children);
}

void wxSFPolygonShape::OnHandle(wxSFShapeHandle& handle)
{
	wxSFRectShape::OnHandle(handle);

	FitVerticesToBoundingBox();
}

//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//

void wxSFPolygonShape::GetExtents(double *minx, double *miny, double *maxx, double *maxy)
{
	if(m_arrVertices.Count() == 0)return;

	*minx = *maxx = m_arrVertices[0].x;
	*miny = *maxy = m_arrVertices[0].y;

	for(size_t i = 1; i < m_arrVertices.Count(); i++)
	{
		if(m_arrVertices[i].x < *minx)*minx = m_arrVertices[i].x;
		if(m_arrVertices[i].x > *maxx)*maxx = m_arrVertices[i].x;
		if(m_arrVertices[i].y < *miny)*miny = m_arrVertices[i].y;
		if(m_arrVertices[i].y > *maxy)*maxy = m_arrVertices[i].y;
	}
}

void wxSFPolygonShape::GetTranslatedVerices(wxRealPoint pts[])
{
	wxRealPoint absPos = GetAbsolutePosition();

	for(size_t i = 0; i < m_arrVertices.Count(); i++)pts[i] = absPos + m_arrVertices[i];
}

void wxSFPolygonShape::GetTranslatedVerices(wxPoint pts[])
{
	wxPoint absPos = Conv2Point(GetAbsolutePosition());

	for(size_t i = 0; i < m_arrVertices.Count(); i++)pts[i] = absPos + Conv2Point(m_arrVertices[i]);
}

void wxSFPolygonShape::NormalizeVertices()
{
	// move all vertices so the polygon's relative bounding box will be located in the origin

	double minx = 0, miny = 0, maxx = 0, maxy = 0, dx = 0, dy = 0;

	GetExtents(&minx, &miny, &maxx, &maxy);

	dx = minx*(-1);
	dy = miny*(-1);

	for(size_t i = 0; i < m_arrVertices.Count(); i++)
	{
		m_arrVertices[i].x += dx;
		m_arrVertices[i].y += dy;
	}
}

void wxSFPolygonShape::FitVerticesToBoundingBox()
{
	double minx = 0, miny = 0, maxx = 0, maxy = 0, sx = 1, sy = 1;

	GetExtents(&minx, &miny, &maxx, &maxy);

	sx = m_nRectSize.x/(maxx - minx);
	sy = m_nRectSize.y/(maxy - miny);

	for(size_t i = 0; i < m_arrVertices.Count(); i++)
	{
		m_arrVertices[i].x *= sx;
		m_arrVertices[i].y *= sy;
	}
}

void wxSFPolygonShape::FitBoundingBoxToVertices()
{
	double minx = 0, miny = 0, maxx = 0, maxy = 0;

	GetExtents(&minx, &miny, &maxx, &maxy);

	m_nRectSize.x = maxx - minx;
	m_nRectSize.y = maxy - miny;
}

void wxSFPolygonShape::DrawPolygonShape(wxDC& dc)
{
	size_t vcount = m_arrVertices.Count();
	wxPoint *pts = new wxPoint[vcount];

	GetTranslatedVerices(pts);
	dc.DrawPolygon(vcount, pts);

	delete [] pts;
}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFPolygonShape::DrawNormal(wxDC& dc)
{
	// HINT: overload it for custom actions...

	dc.SetPen(m_Border);
	dc.SetBrush(m_Fill);
	DrawPolygonShape(dc);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFPolygonShape::DrawHover(wxDC& dc)
{
	// HINT: overload it for custom actions...

	dc.SetPen(wxPen(m_nHoverColor, 1));
	dc.SetBrush(m_Fill);
	DrawPolygonShape(dc);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFPolygonShape::DrawHighlighted(wxDC& dc)
{
	// HINT: overload it for custom actions...

	dc.SetPen(wxPen(m_nHoverColor, 2));
	dc.SetBrush(m_Fill);
	DrawPolygonShape(dc);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFPolygonShape::DrawShadow(wxDC& dc)
{
	// HINT: overload it for custom actions...

    if( m_Fill.GetStyle() != wxTRANSPARENT )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(GetParentCanvas()->GetShadowFill());

        wxRealPoint nOffset = GetParentCanvas()->GetShadowOffset();

        MoveBy(nOffset);
        DrawPolygonShape(dc);
        MoveBy(-nOffset.x, -nOffset.y);

        dc.SetBrush(wxNullBrush);
        dc.SetPen(wxNullPen);
    }
}
