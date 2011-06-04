/***************************************************************
 * Name:      RoundOrthoShape.cpp
 * Purpose:   Implements rounded orthogonal line shape class
 * Author:    Michal Bližňák (michal.bliznak@gmail.com)
 * Created:   2011-01-27
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include <math.h>

#include "wx/wxsf/RoundOrthoShape.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/CommonFcn.h"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFRoundOrthoLineShape, wxSFOrthoLineShape);

wxSFRoundOrthoLineShape::wxSFRoundOrthoLineShape() : wxSFOrthoLineShape()
{
	m_nMaxRadius = sfdvROUNDORTHOSHAPE_MAXRADIUS;
	
	XS_SERIALIZE_EX(m_nMaxRadius, wxT("max_radius"), sfdvROUNDORTHOSHAPE_MAXRADIUS);
}

wxSFRoundOrthoLineShape::wxSFRoundOrthoLineShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager)
: wxSFOrthoLineShape(src, trg, path, manager)
{
	m_nMaxRadius = sfdvROUNDORTHOSHAPE_MAXRADIUS;
	
	XS_SERIALIZE_EX(m_nMaxRadius, wxT("max_radius"), sfdvROUNDORTHOSHAPE_MAXRADIUS);
}

wxSFRoundOrthoLineShape::wxSFRoundOrthoLineShape(const wxSFRoundOrthoLineShape& obj)
: wxSFOrthoLineShape(obj)
{
	m_nMaxRadius = obj.m_nMaxRadius;
	
	XS_SERIALIZE_EX(m_nMaxRadius, wxT("max_radius"), sfdvROUNDORTHOSHAPE_MAXRADIUS);
}

wxSFRoundOrthoLineShape::~wxSFRoundOrthoLineShape()
{

}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFRoundOrthoLineShape::DrawLineSegment(wxDC& dc, const wxRealPoint& src, const wxRealPoint& trg)
{
	if( (trg.x == src.x) || ( trg.y == src.y ) )
	{
		dc.DrawLine( src.x, src.y, trg.x, trg.y );
		return;
	}
	
	double nDx, nDy;
	int kx, ky, nR;
	
	nDx = trg.x - src.x;
	nDy = trg.y - src.y;
	nDx < 0 ? kx = -1 : kx = 1;
	nDy < 0 ? ky = 1 : ky = -1;

	wxRealPoint ptCenter( (src.x + trg.x)/2, (src.y + trg.y)/2 );
	
	dc.SetBrush( *wxTRANSPARENT_BRUSH );
	
	if( fabs(nDy) / fabs(nDx) < 1 )
	{
		nR = fabs(nDy * m_nMaxRadius/100) < m_nMaxRadius ? fabs(nDy * m_nMaxRadius/100) : m_nMaxRadius;
		
		dc.DrawLine( src.x, src.y, ptCenter.x - nR * kx, src.y );
		dc.DrawLine( ptCenter.x, src.y - nR * ky, ptCenter.x, trg.y + nR * ky);
		dc.DrawLine( ptCenter.x + nR * kx, trg.y, trg.x, trg.y );
		
		if( nR > 0 )
		{
			if( ( ky > 0 && kx > 0) || ( ky < 0 && kx < 0) )
			{
				dc.DrawArc( ptCenter.x - nR * kx, src.y, ptCenter.x, src.y - nR * ky, ptCenter.x - nR * kx, src.y - nR * ky );
				dc.DrawArc( ptCenter.x + nR * kx, trg.y, ptCenter.x, trg.y + nR * ky, ptCenter.x + nR * kx, trg.y + nR * ky );
			}
			else
			{
				dc.DrawArc( ptCenter.x, src.y - nR * ky, ptCenter.x - nR * kx, src.y, ptCenter.x - nR * kx, src.y - nR * ky );
				dc.DrawArc( ptCenter.x, trg.y + nR * ky, ptCenter.x + nR * kx, trg.y, ptCenter.x + nR * kx, trg.y + nR * ky );
			}
		}
	}
	else
	{
		nR = fabs(nDx * m_nMaxRadius/100) < m_nMaxRadius ? fabs(nDx * m_nMaxRadius/100) : m_nMaxRadius;

		dc.DrawLine( src.x, src.y, src.x, ptCenter.y + nR * ky );
		dc.DrawLine( src.x + nR * kx, ptCenter.y, trg.x - nR * kx, ptCenter.y );
		dc.DrawLine( trg.x, ptCenter.y - nR * ky, trg.x, trg.y );
		
		if( nR > 0 )
		{
			if( ( ky > 0 && kx > 0) || ( ky < 0 && kx < 0) )
			{
				dc.DrawArc( src.x + nR * kx, ptCenter.y, src.x, ptCenter.y + nR * ky, src.x + nR * kx, ptCenter.y + nR * ky );
				dc.DrawArc( trg.x - nR * kx, ptCenter.y, trg.x, ptCenter.y - nR * ky, trg.x - nR * kx, ptCenter.y - nR * ky );
			}
			else
			{
				dc.DrawArc( src.x, ptCenter.y + nR * ky, src.x + nR * kx, ptCenter.y, src.x + nR * kx, ptCenter.y + nR * ky );
				dc.DrawArc( trg.x, ptCenter.y - nR * ky, trg.x - nR * kx, ptCenter.y, trg.x - nR * kx, ptCenter.y - nR * ky );
			}
			}
	}
	
	dc.SetBrush( wxNullBrush );
}
