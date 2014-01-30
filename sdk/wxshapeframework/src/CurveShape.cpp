/***************************************************************
 * Name:      CurveShape.cpp
 * Purpose:   Implements curve shape class
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

#include "wx/wxsf/CurveShape.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/CommonFcn.h"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFCurveShape, wxSFLineShape);

wxSFCurveShape::wxSFCurveShape() : wxSFLineShape()
{
}

wxSFCurveShape::wxSFCurveShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager)
: wxSFLineShape(src, trg, path, manager)
{
}

wxSFCurveShape::wxSFCurveShape(const wxSFCurveShape& obj)
: wxSFLineShape(obj)
{
}

wxSFCurveShape::~wxSFCurveShape()
{

}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

wxRect wxSFCurveShape::GetBoundingBox()
{
    return wxSFLineShape::GetBoundingBox().Inflate(20, 20);
}

//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

wxRealPoint wxSFCurveShape::GetPoint(size_t segment, double offset)
{
	if( segment <= m_lstPoints.GetCount() )
	{
		wxRealPoint A, B, C, D;
		GetSegmentQuaternion( segment, A, B, C, D );
		return Coord_Catmul_Rom_Kubika(A, B, C, D, offset);
	}
	else
		return wxRealPoint();
}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFCurveShape::DrawCompleteLine(wxDC& dc)
{
    size_t i = 0;
	wxRealPoint A, B, C, D;
	
    switch(m_nMode)
    {
    case modeREADY:
        {
			// draw line segments
			if( !m_lstPoints.IsEmpty() )
			{
				for( i = 0; i <= m_lstPoints.GetCount(); i++ )
				{
					GetSegmentQuaternion( i, A, B, C, D );
					Catmul_Rom_Kubika(A, B, C, D, dc);
				}
			}
			else
			{
				GetDirectLine( B, C );
				dc.DrawLine( Conv2Point(B), Conv2Point(C) );
			}
				  
			// draw target arrow
            if( m_pTrgArrow ) m_pTrgArrow->Draw( B, C, dc);
			GetLineSegment( 0, B, C );
            // draw source arrow
			if( m_pSrcArrow ) m_pSrcArrow->Draw(C, B, dc);			
        }
        break;

    case modeUNDERCONSTRUCTION:
        {
            // draw basic line parts
			if( !m_lstPoints.IsEmpty() )
			{
				for( i = 0; i < m_lstPoints.GetCount(); i++ )
				{
					GetSegmentQuaternion( i, A, B, C, D );
					Catmul_Rom_Kubika(A, B, C, D, dc);
				}
			}
			
			// draw unfinished line segment if any (for interactive line creation)
			dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
			
			if( i )
			{
				dc.DrawLine(Conv2Point( C ), m_nUnfinishedPoint);
			}
			else if( m_nSrcShapeId != -1 )
			{
				// draw unfinished line segment if any (for interactive line creation)
				dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
				
				wxSFShapeBase* pSrcShape = GetShapeManager()->FindShape(m_nSrcShapeId);
				if( pSrcShape )
				{
					if( pSrcShape->GetConnectionPoints().IsEmpty() )
					{
						dc.DrawLine( Conv2Point(pSrcShape->GetBorderPoint(pSrcShape->GetCenter(), Conv2RealPoint(m_nUnfinishedPoint))), m_nUnfinishedPoint );
					}
					else
						dc.DrawLine( Conv2Point( GetModSrcPoint() ), m_nUnfinishedPoint );
				}				
				
				dc.SetPen(wxNullPen);
			}
			dc.SetPen(wxNullPen);
        }
        break;

    case modeSRCCHANGE:
        {
            // draw basic line parts
			for( i = 1; i <= m_lstPoints.GetCount(); i++ )
			{
				GetSegmentQuaternion( i, A, B, C, D );
				Catmul_Rom_Kubika(A, B, C, D, dc);
			}
			
            // draw linesegment being updated
            dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
			if( !m_lstPoints.IsEmpty() )
			{
				GetSegmentQuaternion( 0, A, B, C, D );
			}
			else
				GetDirectLine( B, C );
            dc.DrawLine(m_nUnfinishedPoint, Conv2Point(C));
            dc.SetPen(wxNullPen);
        }
        break;

    case modeTRGCHANGE:
        {
            // draw basic line parts
			if( !m_lstPoints.IsEmpty() )
			{
				for( i = 0; i < m_lstPoints.GetCount(); i++ )
				{
					GetSegmentQuaternion( i, A, B, C, D );
					Catmul_Rom_Kubika(A, B, C, D, dc);
				}
			}
			else
				C = GetSrcPoint();

            // draw linesegment being updated
            dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
            dc.DrawLine(m_nUnfinishedPoint, Conv2Point(C));
            dc.SetPen(wxNullPen);
        }
        break;
    }
}

//----------------------------------------------------------------------------------//
// private functions
//----------------------------------------------------------------------------------//

void wxSFCurveShape::GetSegmentQuaternion(size_t segment, wxRealPoint& A, wxRealPoint& B, wxRealPoint& C, wxRealPoint& D)
{
	static wxRealPoint quart[4];
	wxXS::RealPointList::compatibility_iterator node;
	
	int nIndex = 2 - segment;
	
	if( ( nIndex - 1 ) >= 0 ) quart[ nIndex - 1 ] = GetSrcPoint();
	if( ( nIndex - 2 ) >= 0 ) quart[ nIndex - 2 ] = GetModSrcPoint();
	
	if( nIndex >= 0 ) node = m_lstPoints.Item( 0 );
	else
	{
		node = m_lstPoints.Item( abs( nIndex ) );
		nIndex = 0;
	}
		 
	for( ; nIndex < 4; nIndex++ )
	{
		if( node )
		{
			quart[ nIndex ] = *node->GetData();
			node = node->GetNext();
		}
		else
		{
			if( nIndex == 2 ) quart[ 2 ] = GetTrgPoint();
			else if( nIndex == 3 )
			{
				if( m_nMode == modeUNDERCONSTRUCTION ) quart[ 3 ] = Conv2RealPoint( m_nUnfinishedPoint );
				else if( m_nTrgShapeId != -1 ) quart[ 3 ] = GetModTrgPoint();
			}
		}
	}
	
	A = quart[0];
	B = quart[1];
	C = quart[2];
	D = quart[3];
}

void wxSFCurveShape::Catmul_Rom_Kubika(const wxRealPoint& A, const wxRealPoint& B, const wxRealPoint& C, const wxRealPoint& D, wxDC& dc)
{
	// the begginig of the curve is in the B point
	wxRealPoint point0=B;
	wxRealPoint point1;

	int nOptimSteps = double(Distance(B, C)) / 10;
	if( nOptimSteps < 10 ) nOptimSteps = 10;

    // draw the curve
	for(double t = 0; t <= (1 + (1.0f / nOptimSteps)); t += 1.0f / (nOptimSteps-1))
	{
		point1 = Coord_Catmul_Rom_Kubika(A,B,C,D,t);
		dc.DrawLine((int)point0.x, (int)point0.y, (int)point1.x, (int)point1.y);
		point0 = point1;
	}
	point1 = Coord_Catmul_Rom_Kubika(A,B,C,D,1);
	dc.DrawLine((int)point0.x, (int)point0.y, (int)point1.x, (int)point1.y);
}

wxRealPoint wxSFCurveShape::Coord_Catmul_Rom_Kubika(const wxRealPoint& p1, const wxRealPoint& p2, const wxRealPoint& p3, const wxRealPoint& p4, double t)
{
	double pom1, pom2;
	double C1,C2,C3,C4;
    wxRealPoint point;

	// auxiliary variables
	pom1 = t - 1;
	pom2 = t * t;

	// used polynoms
    C1 = (-pom2*t + 2*pom2 - t)  / 2;
    C2 = (3*pom2*t - 5*pom2 + 2) / 2;
    C3 = (-3*pom2*t + 4*pom2 +t) / 2;
    C4 = pom1*pom2 / 2;

	// calculation of curve point for t = <0,1>
	point.x = C1*p1.x + C2*p2.x + C3*p3.x + C4*p4.x;
    point.y = C1*p1.y + C2*p2.y + C3*p3.y + C4*p4.y;
	//point.z = C1*p1.z + C2*p2.z + C3*p3.z + C4*p4.z;

    return point;
}



