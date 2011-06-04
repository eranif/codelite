/***************************************************************
 * Name:      DiamondShape.cpp
 * Purpose:   Implements diamond shape class
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

#include "wx/wxsf/DiamondShape.h"
#include "wx/wxsf/CommonFcn.h"

// diamond shape
const wxRealPoint diamond[4]={wxRealPoint(0,25), wxRealPoint(50,0), wxRealPoint(100, 25), wxRealPoint(50, 50)};

XS_IMPLEMENT_CLONABLE_CLASS(wxSFDiamondShape, wxSFPolygonShape);

wxSFDiamondShape::wxSFDiamondShape()
: wxSFPolygonShape()
{
	EnablePropertySerialization(wxT("vertices"), false);

	SetVertices(4, diamond);
}

wxSFDiamondShape::wxSFDiamondShape(const wxRealPoint& pos, wxSFDiagramManager* manager)
: wxSFPolygonShape(4, diamond, pos, manager)
{
	EnablePropertySerialization(wxT("vertices"), false);
}

wxSFDiamondShape::wxSFDiamondShape(const wxSFDiamondShape& obj)
: wxSFPolygonShape(obj)
{

}

wxSFDiamondShape::~wxSFDiamondShape()
{

}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

bool wxSFDiamondShape::Contains(const wxPoint& pos)
{
    wxRect bbRct = this->GetBoundingBox();
    if(!bbRct.Contains(pos))return false;

    wxRealPoint center = GetCenter();
    double k = ((double)bbRct.GetHeight()/2)/((double)bbRct.GetWidth()/2);

    if(pos.x <= center.x)
    {
        // test left-top quadrant
        if((pos.y <= center.y) && (pos.y >= (center.y - (pos.x - bbRct.GetLeft())*k))) return true;
        // test left-bottom quadrant
        if((pos.y >= center.y) && (pos.y <= (center.y + (pos.x - bbRct.GetLeft())*k))) return true;
    }
    else
    {
        // test right-top quadrant
        if((pos.y <= center.y) && (pos.y >= (bbRct.GetTop() + (pos.x - center.x)*k))) return true;
        // test left-bottom quadrant
        if((pos.y >= center.y) && (pos.y <= (bbRct.GetBottom() - (pos.x - center.x)*k))) return true;
    }

    return false;
}
