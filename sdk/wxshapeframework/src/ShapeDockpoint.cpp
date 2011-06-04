/***************************************************************
 * Name:      ShapeDockpoint.cpp
 * Purpose:   Implements shape docpoint class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2010-12-14
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/ShapeDockpoint.h"
#include "wx/wxsf/ShapeBase.h"
#include "wx/wxsf/CommonFcn.h"

#define RADIUS 3

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFConnectionPoint, wxObject);

// constructor and destructor //////////////////////////////////////////////////

wxSFConnectionPoint::wxSFConnectionPoint()
{
	m_pParentShape = NULL;
	m_nType = cpUNDEF;
	m_fMouseOver = false;
	m_nRelPosition = sfdvCONNPOINT_RELPOS;
	
	MarkSerializableDataMembers();
}

wxSFConnectionPoint::wxSFConnectionPoint(wxSFShapeBase* parent, CPTYPE type)
{
	wxASSERT( parent );
	
	m_pParentShape = parent;
	m_nType = type;
	m_fMouseOver = false;
	m_nRelPosition = sfdvCONNPOINT_RELPOS;
	
	MarkSerializableDataMembers();
}

wxSFConnectionPoint::wxSFConnectionPoint(wxSFShapeBase* parent, const wxRealPoint& relpos, long id)
{
	wxASSERT( parent );
	
	m_pParentShape = parent;
	m_nType = cpCUSTOM;
	m_fMouseOver = false;
	m_nRelPosition = relpos;
	
	SetId( id );
	
	MarkSerializableDataMembers();
}

wxSFConnectionPoint::wxSFConnectionPoint(const wxSFConnectionPoint& obj) : xsSerializable(obj)
{
	m_pParentShape = NULL;
	m_nType = obj.m_nType;
	m_fMouseOver = obj.m_fMouseOver;
	m_nRelPosition = obj.m_nRelPosition;
	
	MarkSerializableDataMembers();
}

void wxSFConnectionPoint::MarkSerializableDataMembers()
{
	XS_SERIALIZE_INT( m_nType, wxT("connection_type") );
	XS_SERIALIZE_EX( m_nRelPosition, wxT("relative_position"), sfdvCONNPOINT_RELPOS );
}

// other functions /////////////////////////////////////////////////////////////

bool wxSFConnectionPoint::Contains(const wxPoint& pos) const
{
	// HINT: overload it for custom actions...
	
	return Distance( GetConnectionPoint(), Conv2RealPoint(pos) ) < 3 * RADIUS;
}

void wxSFConnectionPoint::Draw(wxDC& dc)
{
	if( m_fMouseOver )
	{
		this->DrawHover( dc );
	}
	else
		this->DrawNormal( dc );
}

void wxSFConnectionPoint::DrawHover(wxDC& dc)
{
	// HINT: overload it for custom actions...
	
	dc.SetPen( *wxBLACK_PEN );
	dc.SetBrush( *wxRED_BRUSH );
	
	dc.DrawCircle( Conv2Point( GetConnectionPoint() ), RADIUS );
	
	dc.SetBrush( wxNullBrush );
	dc.SetPen( wxNullPen );
}

void wxSFConnectionPoint::DrawNormal(wxDC& dc)
{
	// HINT: overload it for custom actions...
}


wxRealPoint wxSFConnectionPoint::GetConnectionPoint() const
{
	wxASSERT( m_pParentShape );
	
	if( m_pParentShape )
	{
		wxRect rctParent = m_pParentShape->GetBoundingBox();
		
		switch( m_nType )
		{
			case cpTOPLEFT:
				return Conv2RealPoint( rctParent.GetTopLeft() );
				
			case cpTOPMIDDLE:
				return wxRealPoint(  rctParent.GetLeft() + rctParent.GetWidth()/2, rctParent.GetTop() );
				
			case cpTOPRIGHT:
				return Conv2RealPoint( rctParent.GetTopRight() );
				
			case cpCENTERLEFT:
				return wxRealPoint( rctParent.GetLeft(), rctParent.GetTop() + rctParent.GetHeight()/2 );
				
			case cpCENTERMIDDLE:
				return wxRealPoint( rctParent.GetLeft() + rctParent.GetWidth()/2, rctParent.GetTop() + rctParent.GetHeight()/2 );
				
			case cpCENTERRIGHT:
				return wxRealPoint( rctParent.GetRight(), rctParent.GetTop() + rctParent.GetHeight()/2 );
				
			case cpBOTTOMLEFT:
				return Conv2RealPoint( rctParent.GetLeftBottom() );
				
			case cpBOTTOMMIDDLE:
				return wxRealPoint( rctParent.GetLeft() + rctParent.GetWidth()/2, rctParent.GetBottom() );
				
			case cpBOTTOMRIGHT:
				return Conv2RealPoint( rctParent.GetRightBottom() );
				
			case cpCUSTOM:
				return wxRealPoint( rctParent.GetLeft() + rctParent.GetWidth() * m_nRelPosition.x/100, rctParent.GetTop() + rctParent.GetHeight() * m_nRelPosition.y/100 );
				
			default:
				;
		}
	}
	
	return wxRealPoint(0, 0);
}

void wxSFConnectionPoint::Refresh()
{
	if( m_pParentShape ) m_pParentShape->Refresh();
}

void wxSFConnectionPoint::_OnMouseMove(const wxPoint& pos)
{
	if( this->Contains(pos) )
	{
		if( !m_fMouseOver )
		{
			m_fMouseOver = true;
			Refresh();
		}
	}
	else
	{
		if( m_fMouseOver )
		{
			m_fMouseOver = false;
			Refresh();
		}
	}
}
