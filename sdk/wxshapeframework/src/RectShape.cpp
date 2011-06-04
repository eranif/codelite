/***************************************************************
 * Name:      RectShape.cpp
 * Purpose:   Implements rectangular shape class
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

#include "wx/wxsf/RectShape.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/CommonFcn.h"

// TODO: wxSFShapeBase: Implement LockAspectRation() function

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFRectShape, wxSFShapeBase);

wxSFRectShape::wxSFRectShape(void) : wxSFShapeBase()
{
	m_nRectSize = sfdvRECTSHAPE_SIZE;
	m_Border = sfdvRECTSHAPE_BORDER;
	m_Fill = sfdvRECTSHAPE_FILL;

	MarkSerializableDataMembers();
}

wxSFRectShape::wxSFRectShape(const wxRealPoint& pos, const wxRealPoint& size, wxSFDiagramManager* manager)
: wxSFShapeBase(pos, manager)
{
	m_nRectSize = size;
	m_Border = sfdvRECTSHAPE_BORDER;
	m_Fill = sfdvRECTSHAPE_FILL;

	MarkSerializableDataMembers();
}

wxSFRectShape::wxSFRectShape(const wxSFRectShape& obj) : wxSFShapeBase(obj)
{
	m_nRectSize = obj.m_nRectSize;
	m_Border = obj.m_Border;
	m_Fill = obj.m_Fill;

	MarkSerializableDataMembers();
}

wxSFRectShape::~wxSFRectShape(void)
{
}

void wxSFRectShape::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(m_nRectSize, wxT("size"), sfdvRECTSHAPE_SIZE);
	XS_SERIALIZE_EX(m_Border, wxT("border"), sfdvRECTSHAPE_BORDER);
	XS_SERIALIZE_EX(m_Fill, wxT("fill"), sfdvRECTSHAPE_FILL);
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

wxRect wxSFRectShape::GetBoundingBox()
{
    wxRealPoint apos = this->GetAbsolutePosition();
    return wxRect(wxPoint((int)apos.x, (int)apos.y), wxSize((int)m_nRectSize.x, (int)m_nRectSize.y ));
}

void wxSFRectShape::Scale(double x, double y, bool children)
{
	// HINT: overload it for custom actions...

	if((x > 0) && (y > 0))
	{
		SetRectSize(m_nRectSize.x * x, m_nRectSize.y * y);

        // call default function implementation (needed for scaling of shape's children)
		wxSFShapeBase::Scale(x, y, children);
	}
}

void wxSFRectShape::FitToChildren()
{
    // HINT: overload it for custom actions...

    wxSFShapeBase* pChild;

    // get bounding box of the shape and children set be inside it	
	wxRect chBB = this->GetBoundingBox();
	wxRect shpBB = chBB;

    SerializableList::compatibility_iterator node = GetFirstChildNode();
    while(node)
    {
        pChild = (wxSFShapeBase*)node->GetData();

        if( pChild->ContainsStyle(sfsALWAYS_INSIDE) )
        {
            pChild->GetCompleteBoundingBox(chBB, bbSELF | bbCHILDREN);
        }
        node = node->GetNext();
    }

	if(!chBB.IsEmpty())
	{
		//wxRect shpBB = this->GetBoundingBox();

		if(!shpBB.Contains(chBB))
		{
			double dx = chBB.GetLeft() - shpBB.GetLeft();
			double dy = chBB.GetTop() - shpBB.GetTop();

			// resize parent shape
			shpBB.Union(chBB);
			MoveTo(shpBB.GetPosition().x, shpBB.GetPosition().y);
			m_nRectSize = wxRealPoint(shpBB.GetSize().x, shpBB.GetSize().y);

			// move its "1st level" children if neccessary
			if((dx < 0) || (dy < 0))
			{
				node = GetFirstChildNode();
				while(node)
				{
					pChild = (wxSFShapeBase*)node->GetData();
					if(dx < 0)pChild->MoveBy(abs((int)dx), 0);
					if(dy < 0)pChild->MoveBy(0, abs((int)dy));

					node = node->GetNext();
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFRectShape::DrawNormal(wxDC& dc)
{
	// HINT: overload it for custom actions...

	dc.SetPen(m_Border);
	dc.SetBrush(m_Fill);
	dc.DrawRectangle(Conv2Point(GetAbsolutePosition()), Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFRectShape::DrawHover(wxDC& dc)
{
	// HINT: overload it for custom actions...

	dc.SetPen(wxPen(m_nHoverColor, 1));
	dc.SetBrush(m_Fill);
	dc.DrawRectangle(Conv2Point(GetAbsolutePosition()), Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFRectShape::DrawHighlighted(wxDC& dc)
{
	// HINT: overload it for custom actions...

	dc.SetPen(wxPen(m_nHoverColor, 2));
	dc.SetBrush(m_Fill);
	dc.DrawRectangle(Conv2Point(GetAbsolutePosition()), Conv2Size(m_nRectSize));
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFRectShape::DrawShadow(wxDC& dc)
{
	// HINT: overload it for custom actions...

    if( m_Fill.GetStyle() != wxTRANSPARENT )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(GetParentCanvas()->GetShadowFill());
        dc.DrawRectangle(Conv2Point(GetAbsolutePosition() + GetParentCanvas()->GetShadowOffset()), Conv2Size(m_nRectSize));
        dc.SetBrush(m_Fill);
        dc.SetPen(wxNullPen);
    }
}

void wxSFRectShape::OnRightHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//m_nRectSize.x = handle.GetPosition().x - GetAbsolutePosition().x;
	m_nRectSize.x += handle.GetDelta().x;
}

void wxSFRectShape::OnLeftHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

    wxSFShapeBase *pChild;

	//double dx = (double)handle.GetPosition().x - GetAbsolutePosition().x;
	double dx = (double)handle.GetDelta().x;

	// update position of children
	if( !ContainsStyle(sfsLOCK_CHILDREN) )
	{	
		SerializableList::compatibility_iterator node = GetFirstChildNode();
		while(node)
		{
			pChild = (wxSFShapeBase*)node->GetData();
			if( pChild->GetHAlign() == halignNONE )
			{
				pChild->MoveBy(-dx, 0);
			}
			node = node->GetNext();
		}
	}
	// update position and size of the shape
	m_nRectSize.x -= dx;
	m_nRelativePosition.x += dx;
}

void wxSFRectShape::OnTopHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

    wxSFShapeBase *pChild;

	//double dy = (double)handle.GetPosition().y - GetAbsolutePosition().y;
	double dy = (double)handle.GetDelta().y;

	// update position of children
	if( !ContainsStyle( sfsLOCK_CHILDREN ) )
	{
		SerializableList::compatibility_iterator node = GetFirstChildNode();
		while(node)
		{
			pChild = (wxSFShapeBase*)node->GetData();
			if( pChild->GetVAlign() == valignNONE )
			{
				pChild->MoveBy(0, -dy);
			}
			node = node->GetNext();
		}
	}
	// update position and size of the shape
	m_nRectSize.y -= dy;
	m_nRelativePosition.y += dy;
}

void wxSFRectShape::OnBottomHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...

	//m_nRectSize.y = handle.GetPosition().y - GetAbsolutePosition().y;
	m_nRectSize.y += handle.GetDelta().y;
}

wxRealPoint wxSFRectShape::GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end)
{
    // HINT: override it for custom actions ...

    // the function calculates intesection of line leading from the shape center to
    // given point with the shape's bounding box;

    wxRealPoint intersection;
    wxRect bbRct = this->GetBoundingBox();


    if(LinesIntersection(wxRealPoint(bbRct.GetTopLeft().x, bbRct.GetTopLeft().y),
                             wxRealPoint(bbRct.GetTopRight().x + 1, bbRct.GetTopRight().y), start, end, intersection)) return intersection;

    if(LinesIntersection(wxRealPoint(bbRct.GetTopRight().x + 1, bbRct.GetTopRight().y),
                                  wxRealPoint(bbRct.GetBottomRight().x + 1, bbRct.GetBottomRight().y + 1), start, end, intersection)) return intersection;

	if(LinesIntersection(wxRealPoint(bbRct.GetBottomRight().x + 1, bbRct.GetBottomRight().y + 1),
                                  wxRealPoint(bbRct.GetBottomLeft().x, bbRct.GetBottomLeft().y + 1), start, end, intersection)) return intersection;

    if(LinesIntersection(wxRealPoint(bbRct.GetBottomLeft().x, bbRct.GetBottomLeft().y + 1),
                                  wxRealPoint(bbRct.GetTopLeft().x, bbRct.GetTopLeft().y), start, end, intersection)) return intersection;
	
    return GetCenter();
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFRectShape::CreateHandles()
{
	// HINT: overload it for custom actions...

	AddHandle(wxSFShapeHandle::hndLEFTTOP);
	AddHandle(wxSFShapeHandle::hndTOP);
	AddHandle(wxSFShapeHandle::hndRIGHTTOP);
	AddHandle(wxSFShapeHandle::hndRIGHT);
	AddHandle(wxSFShapeHandle::hndRIGHTBOTTOM);
	AddHandle(wxSFShapeHandle::hndBOTTOM);
	AddHandle(wxSFShapeHandle::hndLEFTBOTTOM);
	AddHandle(wxSFShapeHandle::hndLEFT);
	AddHandle(wxSFShapeHandle::hndLEFTTOP);
}

void wxSFRectShape::OnHandle(wxSFShapeHandle& handle)
{
    // HINT: overload it for custom actions...

	switch(handle.GetType())
	{
	case wxSFShapeHandle::hndLEFT:
		OnLeftHandle(handle);
		break;

	case wxSFShapeHandle::hndLEFTTOP:
		OnLeftHandle(handle);
		OnTopHandle(handle);
		break;

	case wxSFShapeHandle::hndLEFTBOTTOM:
		OnLeftHandle(handle);
		OnBottomHandle(handle);
		break;

	case wxSFShapeHandle::hndRIGHT:
		OnRightHandle(handle);
		break;

	case wxSFShapeHandle::hndRIGHTTOP:
		OnRightHandle(handle);
		OnTopHandle(handle);
		break;

	case wxSFShapeHandle::hndRIGHTBOTTOM:
		OnRightHandle(handle);
		OnBottomHandle(handle);
		break;

	case wxSFShapeHandle::hndTOP:
		OnTopHandle(handle);
		break;

	case wxSFShapeHandle::hndBOTTOM:
		OnBottomHandle(handle);
		break;

    default:
        break;
	}
	
	wxSFShapeBase::OnHandle( handle );
}

void wxSFRectShape::OnBeginHandle(wxSFShapeHandle& handle)
{
    m_nPrevPosition = m_nRelativePosition;
    m_nPrevSize = m_nRectSize;
	
	wxSFShapeBase::OnBeginHandle( handle );
}
