/***************************************************************
 * Name:      ArrowBase.cpp
 * Purpose:   Implements line arrow base class
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

#include "wx/wxsf/ArrowBase.h"
#include "wx/wxsf/LineShape.h"
#include "wx/wxsf/CommonFcn.h"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFArrowBase, xsSerializable);

wxSFArrowBase::wxSFArrowBase(void)
{
	m_pParentShape = NULL;
}

wxSFArrowBase::wxSFArrowBase(wxSFShapeBase* parent)
{
	m_pParentShape = parent;
}

wxSFArrowBase::wxSFArrowBase(const wxSFArrowBase& obj)
: xsSerializable(obj)
{
	m_pParentShape = obj.m_pParentShape;
}

wxSFArrowBase::~wxSFArrowBase(void)
{
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFArrowBase::Draw(const wxRealPoint& from, const wxRealPoint& to, wxDC& dc)
{
	// HINT: override it for custom drawing...
	
	wxUnusedVar( from );
	wxUnusedVar( to );
	wxUnusedVar( dc );
}

//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//

void wxSFArrowBase::TranslateArrow(wxPoint *trg, const wxRealPoint *src, int n, const wxRealPoint &from, const wxRealPoint& to)
{
	double cosa, sina, dist;

	// calculate distance between line points
	dist = Distance(from, to);

	// calculate sin and cos of given line segment
	sina = (from.y - to.y)/dist;
	cosa = (from.x - to.x)/dist;

    // rotate arrow
	for(int i = 0; i<n; i++)
	{
		trg->x = (int)((src->x*cosa-src->y*sina)+to.x);
		trg->y = (int)((src->x*sina+src->y*cosa)+to.y);
		trg++;
		src++;
	}
}
