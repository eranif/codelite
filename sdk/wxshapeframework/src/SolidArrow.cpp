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

#include "wx/wxsf/SolidArrow.h"
#include "wx/wxsf/CommonFcn.h"

// arrow shape
static const wxRealPoint arrow[3]={wxRealPoint(0,0), wxRealPoint(10,4), wxRealPoint(10,-4)};

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFSolidArrow, wxSFArrowBase);

wxSFSolidArrow::wxSFSolidArrow(void)
: wxSFArrowBase()
{
	m_Fill = sfdvARROW_FILL;
	m_Pen = sfdvARROW_BORDER;

	MarkSerializableDataMembers();
}

wxSFSolidArrow::wxSFSolidArrow(wxSFShapeBase* parent)
: wxSFArrowBase(parent)
{
	m_Fill = sfdvARROW_FILL;
	m_Pen = sfdvARROW_BORDER;

	MarkSerializableDataMembers();
}

wxSFSolidArrow::wxSFSolidArrow(const wxSFSolidArrow& obj)
: wxSFArrowBase(obj)
{
	m_Fill = obj.m_Fill;
	m_Pen = obj.m_Pen;

	MarkSerializableDataMembers();
}

wxSFSolidArrow::~wxSFSolidArrow(void)
{
}

void wxSFSolidArrow::MarkSerializableDataMembers()
{
	XS_SERIALIZE_EX(m_Fill, wxT("fill"), sfdvARROW_FILL);
	XS_SERIALIZE_EX(m_Pen, wxT("border"), sfdvARROW_BORDER);
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFSolidArrow::Draw(const wxRealPoint &from, const wxRealPoint &to, wxDC& dc)
{
	wxPoint rarrow[3];
	
	TranslateArrow( rarrow, arrow, 3, from, to );

	dc.SetPen(m_Pen);
    dc.SetBrush(m_Fill);
    dc.DrawPolygon(3, rarrow);
    dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

