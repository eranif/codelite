#include "NArrow.h"

// arrow shape
static const wxRealPoint arrow[6]={wxRealPoint(0,4), wxRealPoint(10,0),wxRealPoint(10,4),wxRealPoint(10,-4),wxRealPoint(10,0), wxRealPoint(0,-4)};

XS_IMPLEMENT_CLONABLE_CLASS(NArrow, wxSFSolidArrow);


NArrow::NArrow(void):wxSFSolidArrow()
{
}

NArrow::NArrow(const NArrow& obj): wxSFSolidArrow(obj) 
{
}

NArrow::NArrow(wxSFShapeBase* parent):wxSFSolidArrow(parent)
{
}

NArrow::~NArrow()
{
}

void NArrow::Draw(const wxRealPoint& from, const wxRealPoint& to, wxDC& dc)
{
	wxPoint rarrow[6];
	
	TranslateArrow( rarrow, arrow, 6, from, to );

	dc.SetPen(m_Pen);
    dc.SetBrush(wxNullBrush);
    dc.DrawLines(6, rarrow);
    dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}
