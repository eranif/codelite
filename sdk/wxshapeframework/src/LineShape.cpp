/***************************************************************
 * Name:      LineShape.cpp
 * Purpose:   Implements line shape class
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

#include "wx/wxsf/LineShape.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/CommonFcn.h"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFLineShape, wxSFShapeBase);

// arrow shape
const wxRealPoint arrow[3]={wxRealPoint(0,0), wxRealPoint(10,4), wxRealPoint(10,-4)};

wxSFLineShape::wxSFLineShape(void)
{
	m_nSrcShapeId = sfdvLINESHAPE_UNKNOWNID;
	m_nTrgShapeId = sfdvLINESHAPE_UNKNOWNID;
	m_pSrcArrow = NULL;
	m_pTrgArrow = NULL;
	m_nSrcPoint = sfdvLINESHAPE_DEFAULTPOINT;
	m_nTrgPoint = sfdvLINESHAPE_DEFAULTPOINT;

	m_nDockPoint = sfdvLINESHAPE_DOCKPOINT;
	m_Pen = sfdvLINESHAPE_PEN;

	m_nSrcOffset = m_nTrgOffset = sfdvLINESHAPE_OFFSET;

	m_nMode = modeREADY;
	m_fStandAlone = sfdvLINESHAPE_STANDALONE;

	MarkSerializableDataMembers();

	m_lstPoints.DeleteContents(true);
}

wxSFLineShape::wxSFLineShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager)
: wxSFShapeBase(wxRealPoint(0, 0), manager)
{
	m_nSrcShapeId = src;
	m_nTrgShapeId = trg;
	m_nDockPoint = sfdvLINESHAPE_DOCKPOINT;

	m_pSrcArrow = NULL;
	m_pTrgArrow = NULL;

	m_nSrcPoint = sfdvLINESHAPE_DEFAULTPOINT;
	m_nTrgPoint = sfdvLINESHAPE_DEFAULTPOINT;

	m_Pen = sfdvLINESHAPE_PEN;

	m_nSrcOffset = m_nTrgOffset = sfdvLINESHAPE_OFFSET;

	m_nMode = modeREADY;
	m_fStandAlone = false;

	wxXS::RealPointList::compatibility_iterator node = path.GetFirst();
	while(node)
	{
		m_lstPoints.Append(new wxRealPoint(*node->GetData()));
		node = node->GetNext();
	}

	MarkSerializableDataMembers();

	m_lstPoints.DeleteContents(true);
}

wxSFLineShape::wxSFLineShape(const wxRealPoint& src, const wxRealPoint& trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager)
: wxSFShapeBase(wxRealPoint(0, 0), manager)
{
	m_nSrcShapeId = sfdvLINESHAPE_UNKNOWNID;
	m_nTrgShapeId = sfdvLINESHAPE_UNKNOWNID;
	
	m_nDockPoint = sfdvLINESHAPE_DOCKPOINT;

	m_pSrcArrow = NULL;
	m_pTrgArrow = NULL;

	m_nSrcPoint = src;
	m_nTrgPoint = trg;

	m_Pen = sfdvLINESHAPE_PEN;

	m_nSrcOffset = m_nTrgOffset = sfdvLINESHAPE_OFFSET;

	m_nMode = modeREADY;
	m_fStandAlone = true;

	wxXS::RealPointList::compatibility_iterator node = path.GetFirst();
	while(node)
	{
		m_lstPoints.Append(new wxRealPoint(*node->GetData()));
		node = node->GetNext();
	}

	MarkSerializableDataMembers();

	m_lstPoints.DeleteContents(true);
}

wxSFLineShape::wxSFLineShape(const wxSFLineShape& obj)
: wxSFShapeBase(obj)
{
	m_nSrcShapeId = obj.m_nSrcShapeId;
	m_nTrgShapeId = obj.m_nTrgShapeId;
	
	m_nDockPoint = obj.m_nDockPoint;

	m_nSrcOffset = obj.m_nSrcOffset;
	m_nTrgOffset = obj.m_nTrgOffset;
	
	m_nSrcPoint = obj.m_nSrcPoint;
	m_nTrgPoint = obj.m_nTrgPoint;

	if(obj.m_pSrcArrow)
	{
		m_pSrcArrow = (wxSFArrowBase*)obj.m_pSrcArrow->Clone();
	}
	else
		m_pSrcArrow = NULL;

	if(obj.m_pTrgArrow)
	{
		m_pTrgArrow = (wxSFArrowBase*)obj.m_pTrgArrow->Clone();
	}
	else
		m_pTrgArrow = NULL;

	m_Pen = obj.m_Pen;

	m_nMode = obj.m_nMode;
	m_fStandAlone = obj.m_fStandAlone;

	wxXS::RealPointList::compatibility_iterator node = obj.m_lstPoints.GetFirst();
	while(node)
	{
		m_lstPoints.Append(new wxRealPoint(*node->GetData()));
		node = node->GetNext();
	}

	MarkSerializableDataMembers();

	m_lstPoints.DeleteContents(true);
}

wxSFLineShape::~wxSFLineShape(void)
{
	m_lstPoints.Clear();

	if(m_pSrcArrow)delete m_pSrcArrow;
	if(m_pTrgArrow)delete m_pTrgArrow;
}

void wxSFLineShape::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(m_nSrcShapeId, wxT("source"), sfdvLINESHAPE_UNKNOWNID);
	XS_SERIALIZE_EX(m_nTrgShapeId, wxT("target"), sfdvLINESHAPE_UNKNOWNID);
	XS_SERIALIZE_EX(m_nSrcPoint, wxT("source_point"), sfdvLINESHAPE_DEFAULTPOINT);
	XS_SERIALIZE_EX(m_nTrgPoint, wxT("target_point"), sfdvLINESHAPE_DEFAULTPOINT);
	XS_SERIALIZE_EX(m_fStandAlone, wxT("standalone"), sfdvLINESHAPE_STANDALONE);
	XS_SERIALIZE_DYNAMIC_OBJECT(m_pSrcArrow, wxT("source_arrow"));
	XS_SERIALIZE_DYNAMIC_OBJECT(m_pTrgArrow, wxT("target_arrow"));
	XS_SERIALIZE_EX(m_nSrcOffset, wxT("source_offset"), sfdvLINESHAPE_OFFSET);
	XS_SERIALIZE_EX(m_nTrgOffset, wxT("target_offset"), sfdvLINESHAPE_OFFSET);
	XS_SERIALIZE_LONG_EX(m_nDockPoint, wxT("dock_point"), sfdvLINESHAPE_DOCKPOINT);
	XS_SERIALIZE_EX(m_Pen, wxT("line_style"), sfdvLINESHAPE_PEN);
	XS_SERIALIZE(m_lstPoints, wxT("control_points"));
}

//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

void wxSFLineShape::SetSrcArrow(wxSFArrowBase* arrow)
{
	if(m_pSrcArrow)delete m_pSrcArrow;

	m_pSrcArrow = arrow;
	if(m_pSrcArrow)
	{
		m_pSrcArrow->SetParentShape(this);
	}
}

wxSFArrowBase* wxSFLineShape::SetSrcArrow(wxClassInfo* arrowInfo)
{
	SetSrcArrow((wxSFArrowBase*)arrowInfo->CreateObject());

	return m_pSrcArrow;
}

void wxSFLineShape::SetTrgArrow(wxSFArrowBase* arrow)
{
	if(m_pTrgArrow)delete m_pTrgArrow;

	m_pTrgArrow = arrow;
	if(m_pTrgArrow)
	{
		m_pTrgArrow->SetParentShape(this);
	}
}

wxSFArrowBase* wxSFLineShape::SetTrgArrow(wxClassInfo* arrowInfo)
{
	SetTrgArrow((wxSFArrowBase*)arrowInfo->CreateObject());

	return m_pTrgArrow;
}

wxRealPoint wxSFLineShape::GetSrcPoint()
{
	if( m_fStandAlone )
	{
		return m_nSrcPoint;
	}
	else
	{
		wxRealPoint pt1, pt2;
		wxSFShapeBase* pSrcShape = GetShapeManager()->FindShape(m_nSrcShapeId);
		
		if( pSrcShape && !m_lstPoints.IsEmpty() )
		{
			if( pSrcShape->GetConnectionPoints().IsEmpty() )
			{
				wxXS::RealPointList::compatibility_iterator node = m_lstPoints.GetFirst();
				if( node )
				{	
					pt1 = *node->GetData();
					return pSrcShape->GetBorderPoint(GetModSrcPoint(), pt1);
				}
			}
			else
				return GetModSrcPoint();
		}
		else
		{
			if( m_nMode != modeUNDERCONSTRUCTION )
				GetDirectLine( pt1, pt2 );
			else
				pt1 = GetModSrcPoint();
			 
			return pt1;
		}

		return wxRealPoint();
	}
}

wxRealPoint wxSFLineShape::GetTrgPoint()
{
	if( m_fStandAlone )
	{
		return m_nTrgPoint;
	}
	else
	{
		wxRealPoint pt1, pt2;
		wxSFShapeBase* pTrgShape = GetShapeManager()->FindShape(m_nTrgShapeId);
		
		if( pTrgShape && !m_lstPoints.IsEmpty() )
		{
			if( pTrgShape->GetConnectionPoints().IsEmpty() )
			{
				wxXS::RealPointList::compatibility_iterator node = m_lstPoints.GetLast();
				if( node )
				{	
					pt2 = *node->GetData();
					return pTrgShape->GetBorderPoint(GetModTrgPoint(), pt2);
				}
			}
			else
				return GetModTrgPoint();
		}
		else
		{
			if( m_nMode != modeUNDERCONSTRUCTION )
				GetDirectLine( pt1, pt2 );
			else
				pt2 = Conv2RealPoint( m_nUnfinishedPoint );
				
			return pt2;
		}

		return wxRealPoint();
	}
}

void wxSFLineShape::GetDirectLine(wxRealPoint& src, wxRealPoint& trg)
{
	if( m_fStandAlone )
	{
		src = m_nSrcPoint;
		trg = m_nTrgPoint;
	}
	else
	{
		wxSFShapeBase* pSrcShape = GetShapeManager()->FindShape(m_nSrcShapeId);
		wxSFShapeBase* pTrgShape = GetShapeManager()->FindShape(m_nTrgShapeId);

		if( pSrcShape && pTrgShape )
		{
			wxRealPoint trgCenter = GetModTrgPoint();
			wxRealPoint srcCenter = GetModSrcPoint();

			if( (pSrcShape->GetParent() == pTrgShape) || (pTrgShape->GetParent() == pSrcShape) )
			{
				wxRect trgBB = pTrgShape->GetBoundingBox();
				wxRect srcBB = pSrcShape->GetBoundingBox();

				if( trgBB.Contains((int)srcCenter.x, (int)srcCenter.y) )
				{
					if( srcCenter.y > trgCenter.y )
					{
						src = wxRealPoint(srcCenter.x, srcBB.GetBottom());
						trg = wxRealPoint(srcCenter.x, trgBB.GetBottom());
					}
					else
					{
						src = wxRealPoint(srcCenter.x, srcBB.GetTop());
						trg = wxRealPoint(srcCenter.x, trgBB.GetTop());
					}
					return;
				}
				else if( srcBB.Contains((int)trgCenter.x, (int)trgCenter.y) )
				{
					if( trgCenter.y > srcCenter.y )
					{
						src = wxRealPoint(trgCenter.x, srcBB.GetBottom());
						trg = wxRealPoint(trgCenter.x, trgBB.GetBottom());
					}
					else
					{
						src = wxRealPoint(trgCenter.x, srcBB.GetTop());
						trg = wxRealPoint(trgCenter.x, trgBB.GetTop());
					}
					return;
				}
			}
			
			if( pSrcShape->GetConnectionPoints().IsEmpty() ) src = pSrcShape->GetBorderPoint(srcCenter, trgCenter);
			else
				src = srcCenter;
				
			if( pTrgShape->GetConnectionPoints().IsEmpty() ) trg = pTrgShape->GetBorderPoint(trgCenter, srcCenter);
			else
				trg = trgCenter;
		}
	}
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

wxRealPoint wxSFLineShape::GetAbsolutePosition()
{
	return GetDockPointPosition( m_nDockPoint );
}

wxRealPoint wxSFLineShape::GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end)
{
	wxUnusedVar( start );
	wxUnusedVar( end );
	
	return GetAbsolutePosition();
}

wxRect wxSFLineShape::GetBoundingBox()
{
    wxASSERT(m_pParentManager);

	wxRect lineRct(0, 0, 0, 0);

    // calculate control points area if they exist
    if( !m_lstPoints.IsEmpty() )
    {
		wxRealPoint prevPt = GetSrcPoint();
		
		wxXS::RealPointList::compatibility_iterator node = m_lstPoints.GetFirst();
		while( node )
		{
			if(lineRct.IsEmpty())
            {
                lineRct = wxRect(Conv2Point(prevPt), Conv2Point(*node->GetData()));
            }
            else
                lineRct.Union(wxRect(Conv2Point(prevPt), Conv2Point(*node->GetData())));
				
			prevPt = *node->GetData();
			node = node->GetNext();
		}
		
		lineRct.Union(wxRect(Conv2Point(prevPt), Conv2Point(GetTrgPoint())));
		
    }
    else
    {
		wxRealPoint pt;
		
        // include starting point
		pt = GetSrcPoint();
		if(!lineRct.IsEmpty())
		{
			lineRct.Union(wxRect((int)pt.x, (int)pt.y, 1, 1));
		}
		else
			lineRct = wxRect((int)pt.x, (int)pt.y, 1, 1);

        // include ending point
		pt = GetTrgPoint();
		if(!lineRct.IsEmpty())
		{
			lineRct.Union(wxRect((int)pt.x, (int)pt.y, 1, 1));
		}
		else
			lineRct = wxRect((int)pt.x, (int)pt.y, 1, 1);
    }

    // include unfinished point if the line is under construction
    if((m_nMode == modeUNDERCONSTRUCTION) || (m_nMode == modeSRCCHANGE) || (m_nMode == modeTRGCHANGE))
    {
	    if(!lineRct.IsEmpty())
	    {
            lineRct.Union(wxRect(m_nUnfinishedPoint.x, m_nUnfinishedPoint.y, 1, 1));
	    }
	    else
            lineRct = wxRect(m_nUnfinishedPoint.x, m_nUnfinishedPoint.y, 1, 1);
    }

	return lineRct;
}

wxRealPoint wxSFLineShape::GetDockPointPosition(int dp)
{
	wxXS::RealPointList::compatibility_iterator ptnode;

	int ptsCnt = (int)m_lstPoints.GetCount();

	if( dp >= 0 )
	{
		if( ptsCnt > dp )
		{
			ptnode = m_lstPoints.Item(dp);
			if( ptnode )return *ptnode->GetData();
		}
		else if( ptsCnt > 0 )
		{
			ptnode = m_lstPoints.Item(ptsCnt/2);
			if( ptnode )return *ptnode->GetData();
		}
	}
	else if( dp == -1 ) // start line point
	{
		return GetSrcPoint();
	}
	else if( dp == -2 ) // end line point
	{
		return GetTrgPoint();
	}

    return GetCenter();
}

bool wxSFLineShape::GetLineSegment(size_t index, wxRealPoint& src, wxRealPoint& trg)
{
	if( !m_lstPoints.IsEmpty() )
	{
		if( index == 0 )
		{
			src = GetSrcPoint();
			trg = *m_lstPoints.GetFirst()->GetData();
			return true;
		}
		else if( index == m_lstPoints.GetCount() )
		{
			src = *m_lstPoints.GetLast()->GetData();
			trg = GetTrgPoint();
			return true;
		}
		else if( (index > 0) && (index < m_lstPoints.GetCount()) )
		{
			wxXS::RealPointList::compatibility_iterator node = m_lstPoints.Item(index);
			
			src = *node->GetPrevious()->GetData();
			trg = *node->GetData();
			return true;
		}
		return false;
	}
	else
	{
		if( index == 0 )
		{
			GetDirectLine( src, trg );
			return true;
		}
		else
			return false;
	}
}

bool wxSFLineShape::Contains(const wxPoint& pos)
{
	if( (m_nMode != modeUNDERCONSTRUCTION) && (this->GetHitLinesegment(pos) >= 0) )return true;
	else
		return false;
}

void wxSFLineShape::Scale(double x, double y, bool children)
{
	wxRealPoint *pt;

	wxXS::RealPointList::compatibility_iterator node = m_lstPoints.GetFirst();
	while(node)
	{
		pt = node->GetData();

        pt->x *= x;
        pt->y *= y;

		node = node->GetNext();
	}

    // call default function implementation (needed for scaling of shape's children)
	wxSFShapeBase::Scale(x, y, children);
}

void wxSFLineShape::MoveTo(double x, double y)
{
	MoveBy(x - m_nPrevPosition.x, y - m_nPrevPosition.y);
	m_nPrevPosition.x = x;
	m_nPrevPosition.y = y;
}

void wxSFLineShape::MoveBy(double x, double y)
{
	wxRealPoint *pt;

	wxXS::RealPointList::compatibility_iterator node = m_lstPoints.GetFirst();
	while(node)
	{
		pt = node->GetData();

		pt->x += x;
		pt->y += y;

		node = node->GetNext();
	}
	
	if( m_fStandAlone )
	{
		m_nSrcPoint = m_nSrcPoint + wxRealPoint(x, y);
		m_nTrgPoint = m_nTrgPoint + wxRealPoint(x, y);
	}
	
	if( !m_lstChildItems.IsEmpty() ) Update();
}

void wxSFLineShape::CreateHandles()
{
    // first clear all previously used handles and then create new ones
    m_lstHandles.Clear();

    // create control points handles
	for(size_t i = 0; i < m_lstPoints.GetCount(); i++) AddHandle(wxSFShapeHandle::hndLINECTRL, (int)i);

	// create border handles
	AddHandle(wxSFShapeHandle::hndLINESTART);
	AddHandle(wxSFShapeHandle::hndLINEEND);
}

void wxSFLineShape::OnHandle(wxSFShapeHandle& handle)
{
    switch(handle.GetType())
    {
    case wxSFShapeHandle::hndLINECTRL:
        {
            wxXS::RealPointList::compatibility_iterator node = m_lstPoints.Item(handle.GetId());
            if(node)
            {
                wxRealPoint* pt = node->GetData();
                pt->x = handle.GetPosition().x;
                pt->y = handle.GetPosition().y;
            }
        }
        break;

    case wxSFShapeHandle::hndLINEEND:
		{
			m_nUnfinishedPoint = handle.GetPosition();
			if( m_fStandAlone )	m_nTrgPoint = Conv2RealPoint( handle.GetPosition() );
		}
		break;
		
    case wxSFShapeHandle::hndLINESTART:
        {
			m_nUnfinishedPoint = handle.GetPosition();
			if( m_fStandAlone )	m_nSrcPoint = Conv2RealPoint( handle.GetPosition() );
        }
        break;

    default:
        break;
    }
	
	wxSFShapeBase::OnHandle( handle );
}

void wxSFLineShape::OnEndHandle(wxSFShapeHandle& handle)
{
	// update percentual offset of the line's ending points

    wxSFShapeBase *pParent = GetParentCanvas()->GetShapeUnderCursor();

    if( pParent )
    {
        wxRect bbRect = pParent->GetBoundingBox();

        switch( handle.GetType() )
        {
            case wxSFShapeHandle::hndLINESTART:
                if( !m_fStandAlone && ( pParent->GetId() == m_nSrcShapeId ) )
                {
                    m_nSrcOffset.x = double(handle.GetPosition().x - bbRect.GetLeft()) / bbRect.GetWidth();
                    m_nSrcOffset.y = double(handle.GetPosition().y - bbRect.GetTop()) / bbRect.GetHeight();
                }
                break;

            case wxSFShapeHandle::hndLINEEND:
                if( !m_fStandAlone && ( pParent->GetId() == m_nTrgShapeId ) )
                {
                    m_nTrgOffset.x = double(handle.GetPosition().x - bbRect.GetLeft()) / bbRect.GetWidth();
                    m_nTrgOffset.y = double(handle.GetPosition().y - bbRect.GetTop()) / bbRect.GetHeight();
                }
                break;

            default:
                break;
        }
    }
	
	wxSFShapeBase::OnEndHandle(handle);
}

void wxSFLineShape::OnBeginDrag(const wxPoint& pos)
{
	m_nPrevPosition = GetAbsolutePosition();
	
	wxSFShapeBase::OnBeginDrag(pos);
}

void wxSFLineShape::OnLeftDoubleClick(const wxPoint& pos)
{
    // HINT: override it for custom actions

    if(GetParentCanvas())
    {
        // remove existing handle if exist otherwise create a new one at the
        // given position
        wxSFShapeHandle *pHandle = GetParentCanvas()->GetTopmostHandleAtPosition(pos);
        if(pHandle && (pHandle->GetParentShape() == this))
        {
            if(pHandle->GetType() == wxSFShapeHandle::hndLINECTRL)
                m_lstPoints.DeleteNode(m_lstPoints.Item(pHandle->GetId()));
        }
        else
        {
            int nIndex = this->GetHitLinesegment(pos);
            if( nIndex > -1 ) m_lstPoints.Insert(nIndex, new wxRealPoint(pos.x, pos.y));
        }
        CreateHandles();
        ShowHandles(true);
    }
}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFLineShape::DrawNormal(wxDC& dc)
{
	dc.SetPen(m_Pen);
	DrawCompleteLine(dc);
	dc.SetPen(wxNullPen);
}

void wxSFLineShape::DrawHover(wxDC& dc)
{
	dc.SetPen(wxPen(m_nHoverColor, 1));
	DrawCompleteLine(dc);
	dc.SetPen(wxNullPen);
}

void wxSFLineShape::DrawHighlighted(wxDC& dc)
{
	dc.SetPen(wxPen(m_nHoverColor, 2));
	DrawCompleteLine(dc);
	dc.SetPen(wxNullPen);
}

void wxSFLineShape::DrawCompleteLine(wxDC& dc)
{
    if(!m_pParentManager)return;

    size_t i;
	wxRealPoint src, trg;

    switch(m_nMode)
    {

    case modeREADY:
        {
            // draw basic line parts
            for(i = 0; i <= m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				dc.DrawLine(Conv2Point(src), Conv2Point(trg));
			}

            // draw target arrow
            if(m_pTrgArrow)m_pTrgArrow->Draw(src, trg, dc);
            // draw source arrow
			
			if(m_pSrcArrow)
			{
				GetLineSegment( 0, src, trg );
				m_pSrcArrow->Draw(trg, src, dc);
			}
        }
        break;

    case modeUNDERCONSTRUCTION:
        {
            // draw basic line parts
            for(i = 0; i < m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				dc.DrawLine(Conv2Point(src), Conv2Point(trg));
			}

            // draw unfinished line segment if any (for interactive line creation)
            dc.SetPen( wxPen(*wxBLACK, 1, wxDOT) );
			
			if( i )
			{
				dc.DrawLine( Conv2Point(trg), m_nUnfinishedPoint );
			}
			else
			{
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
			}
				
            dc.SetPen(wxNullPen);
        }
        break;

    case modeSRCCHANGE:
        {
            // draw basic line parts
            for(i = 1; i <= m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				dc.DrawLine(Conv2Point(src), Conv2Point(trg));
			}

            // draw linesegment being updated
			GetLineSegment( 0, src, trg );
			
            if( !m_fStandAlone ) dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
            dc.DrawLine(m_nUnfinishedPoint, Conv2Point(trg));
            dc.SetPen(wxNullPen);
        }
        break;

    case modeTRGCHANGE:
        {
            // draw basic line parts
			if( !m_lstPoints.IsEmpty() )
			{
				for(i = 0; i < m_lstPoints.GetCount(); i++)
				{
					GetLineSegment( i, src, trg );
					dc.DrawLine(Conv2Point(src), Conv2Point(trg));
				}
			}
			else
				trg = GetSrcPoint();
			
            // draw linesegment being updated
            if( !m_fStandAlone ) dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
            dc.DrawLine( Conv2Point(trg), m_nUnfinishedPoint);
            dc.SetPen(wxNullPen);
        }
        break;

    }
}

int wxSFLineShape::GetHitLinesegment(const wxPoint& pos)
{
    //if(!GetBoundingBox().Inflate(10, 10).Contains(pos))return -1;
    if(!GetBoundingBox().Contains(pos))return -1;

    double a, b, c, d;
    wxRealPoint ptSrc, ptTrg;
    wxRect lsBB;

    // Get all polyline segments
	for(size_t i = 0; i <= m_lstPoints.GetCount(); i++)
	{
		GetLineSegment( i, ptSrc, ptTrg );
		
		// calculate line segment bounding box
        lsBB = wxRect(Conv2Point(ptSrc), Conv2Point(ptTrg));
        lsBB.Inflate(2);

        // convert line segment to its parametric form
        a = ptTrg.y - ptSrc.y;
        b = ptSrc.x - ptTrg.x;
        c = -a*ptSrc.x - b*ptSrc.y;

        // calculate distance of the line and give point
        d = (a*pos.x + b*pos.y + c)/sqrt(a*a + b*b);
        if((abs((int)d) <= 5) && lsBB.Contains(pos)) return (int)i;		
	}

    return -1;
}

//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//

wxRealPoint wxSFLineShape::GetModSrcPoint()
{
    wxSFShapeBase* pSrcShape = GetShapeManager()->FindShape(m_nSrcShapeId);
	if( !pSrcShape )return wxRealPoint();
	
	wxRealPoint nModPoint;

    if( m_nSrcOffset != sfdvLINESHAPE_OFFSET )
    {
        wxRect bbRct = pSrcShape->GetBoundingBox();
        nModPoint = pSrcShape->GetAbsolutePosition();

        nModPoint.x += (double)bbRct.GetWidth() * m_nSrcOffset.x;
        nModPoint.y += (double)bbRct.GetHeight() * m_nSrcOffset.y;
    }
    else
        nModPoint = pSrcShape->GetCenter();
		
	wxSFConnectionPoint *pConnPt = pSrcShape->GetNearestConnectionPoint( nModPoint );
	if( pConnPt ) nModPoint = pConnPt->GetConnectionPoint();
	
	return nModPoint;
}

wxRealPoint wxSFLineShape::GetModTrgPoint()
{
    wxSFShapeBase* pTrgShape = GetShapeManager()->FindShape(m_nTrgShapeId);
	if( !pTrgShape )return wxRealPoint();
	
	wxRealPoint nModPoint;

    if( m_nTrgOffset != sfdvLINESHAPE_OFFSET )
    {
        wxRect bbRct = pTrgShape->GetBoundingBox();
        nModPoint = pTrgShape->GetAbsolutePosition();

        nModPoint.x += (double)bbRct.GetWidth() * m_nTrgOffset.x;
        nModPoint.y += (double)bbRct.GetHeight() * m_nTrgOffset.y;
    }
    else
        nModPoint = pTrgShape->GetCenter();
		
	wxSFConnectionPoint *pConnPt = pTrgShape->GetNearestConnectionPoint( nModPoint );
	if( pConnPt ) nModPoint = pConnPt->GetConnectionPoint();
	
	return nModPoint;
}

void wxSFLineShape::SetEndingConnectionPoint(const wxSFConnectionPoint* cp)
{
	if( cp && cp->GetParentShape() )
	{
		wxRealPoint posCp = cp->GetConnectionPoint();
		wxRect rctBB = cp->GetParentShape()->GetBoundingBox();
		
		m_nTrgOffset.x = double(posCp.x - rctBB.GetLeft()) / rctBB.GetWidth();
		m_nTrgOffset.y = double(posCp.y - rctBB.GetTop()) / rctBB.GetHeight();
	}
}

void wxSFLineShape::SetStartingConnectionPoint(const wxSFConnectionPoint* cp)
{
	if( cp && cp->GetParentShape() )
	{
		wxRealPoint posCp = cp->GetConnectionPoint();
		wxRect rctBB = cp->GetParentShape()->GetBoundingBox();
		
		m_nSrcOffset.x = double(posCp.x - rctBB.GetLeft()) / rctBB.GetWidth();
		m_nSrcOffset.y = double(posCp.y - rctBB.GetTop()) / rctBB.GetHeight();
	}
}
