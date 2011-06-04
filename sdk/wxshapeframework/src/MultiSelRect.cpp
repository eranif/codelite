/***************************************************************
 * Name:      MultiSelRect.cpp
 * Purpose:   Implements aux. multiselection shape class
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

#include "wx/wxsf/MultiSelRect.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/TextShape.h"
#include <math.h>

wxSFMultiSelRect::wxSFMultiSelRect(void)
{
	SetBorder(wxPen(wxColour(100, 100, 100), 1, wxDOT));
	SetFill(*wxTRANSPARENT_BRUSH);
}

wxSFMultiSelRect::~wxSFMultiSelRect(void)
{
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFMultiSelRect::OnBeginHandle(wxSFShapeHandle& handle)
{
	// inform all selected shapes about begin of the handle dragging

	if(GetParentCanvas())
	{
		ShapeList lstShapes;
		GetParentCanvas()->GetSelectedShapes(lstShapes);

		ShapeList::compatibility_iterator node = lstShapes.GetFirst();
		while(node)
		{
			node->GetData()->OnBeginHandle(handle);
			node = node->GetNext();
		}
	}
}

void wxSFMultiSelRect::OnEndHandle(wxSFShapeHandle& handle)
{
	// inform all selected shapes about end of the handle dragging

	if(GetParentCanvas())
	{
		ShapeList lstShapes;
		GetParentCanvas()->GetSelectedShapes(lstShapes);

		ShapeList::compatibility_iterator node = lstShapes.GetFirst();
		while(node)
		{
			node->GetData()->OnEndHandle(handle);
			node = node->GetNext();
		}
	}
}

//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//

bool wxSFMultiSelRect::AnyWidthExceeded(const wxPoint& delta)
{
	if(GetParentCanvas())
	{
	    wxSFShapeBase* pShape;
		ShapeList m_lstSelection;
		GetParentCanvas()->GetSelectedShapes(m_lstSelection);

		// first determine whether any shape in the selection exceeds its bounds
		ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
		while(node)
		{
		    pShape = node->GetData();

		    if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
                if((pShape->GetBoundingBox().GetWidth() + delta.x) <= 1)return true;

			node = node->GetNext();
		}
		return false;
	}
	return true;
}

bool wxSFMultiSelRect::AnyHeightExceeded(const wxPoint& delta)
{
	if(GetParentCanvas())
	{
	    wxSFShapeBase* pShape;
		ShapeList m_lstSelection;
		GetParentCanvas()->GetSelectedShapes(m_lstSelection);

		// first determine whether any shape in the selection exceeds its bounds
		ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
		while(node)
		{
		    pShape = node->GetData();

		    if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
                if((pShape->GetBoundingBox().GetHeight() + delta.y) <= 1)return true;

			node = node->GetNext();
		}
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void wxSFMultiSelRect::OnRightHandle(wxSFShapeHandle& handle)
{
	if(GetParentCanvas() && !AnyWidthExceeded(handle.GetDelta()))
	{
	    wxXS::RealPointList::compatibility_iterator ptnode;
	    wxSFLineShape* pLine;
	    wxRealPoint* pt;

		double dx, sx = (GetRectSize().x - 2*sfDEFAULT_ME_OFFSET + handle.GetDelta().x)/(GetRectSize().x - 2*sfDEFAULT_ME_OFFSET);

		ShapeList m_lstSelection;
		GetParentCanvas()->GetSelectedShapes(m_lstSelection);

		ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
		while(node)
		{
			wxSFShapeBase* pShape = node->GetData();

			// scale main parent shape
			if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
			{
			    dx = (pShape->GetAbsolutePosition().x - (GetAbsolutePosition().x + sfDEFAULT_ME_OFFSET))/(GetRectSize().x - 2*sfDEFAULT_ME_OFFSET)*handle.GetDelta().x;

				if(pShape->ContainsStyle(sfsSIZE_CHANGE))pShape->Scale(sx, 1, sfWITHCHILDREN);
                if(pShape->ContainsStyle(sfsPOSITION_CHANGE))pShape->MoveBy(dx, 0);
				
				pShape->FitToChildren();
			}
			else
			{
			    if(pShape->ContainsStyle(sfsPOSITION_CHANGE))
			    {
                    pLine = (wxSFLineShape*)pShape;
                    ptnode = pLine->GetControlPoints().GetFirst();
                    while(ptnode)
                    {
                        pt = ptnode->GetData();
                        dx = ( pt->x - (GetAbsolutePosition().x + sfDEFAULT_ME_OFFSET))/(GetRectSize().x - 2*sfDEFAULT_ME_OFFSET)*handle.GetDelta().x;
                        pt->x += dx;
                        pt->x = floor(pt->x);
                        ptnode = ptnode->GetNext();
                    }
			    }
			}

			node = node->GetNext();
		}
	}
}

void wxSFMultiSelRect::OnLeftHandle(wxSFShapeHandle& handle)
{
	if(GetParentCanvas() && !AnyWidthExceeded(wxPoint(-handle.GetDelta().x, 0)))
	{
	    wxXS::RealPointList::compatibility_iterator ptnode;
	    wxSFLineShape* pLine;
	    wxRealPoint* pt;

		double dx, sx = (GetRectSize().x - 2*sfDEFAULT_ME_OFFSET - handle.GetDelta().x)/(GetRectSize().x - 2*sfDEFAULT_ME_OFFSET);

		ShapeList m_lstSelection;
		GetParentCanvas()->GetSelectedShapes(m_lstSelection);

		ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
		while(node)
		{
			wxSFShapeBase* pShape = node->GetData();

            if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
            {
                if(pShape->ContainsStyle(sfsPOSITION_CHANGE))
                {
                    if(pShape->GetParentShape())
                    {
                        pShape->SetRelativePosition(pShape->GetRelativePosition().x*sx, pShape->GetRelativePosition().y);
                    }
                    else
                    {
                        double dx = handle.GetDelta().x - (pShape->GetAbsolutePosition().x - (GetAbsolutePosition().x + sfDEFAULT_ME_OFFSET))/(GetRectSize().x - 2*sfDEFAULT_ME_OFFSET)*handle.GetDelta().x;
                        pShape->MoveBy(dx, 0);
                    }
                }

                if(pShape->ContainsStyle(sfsSIZE_CHANGE))pShape->Scale(sx, 1, sfWITHCHILDREN);
				
				pShape->FitToChildren();
            }
            else
			{
			    if(pShape->ContainsStyle(sfsPOSITION_CHANGE))
			    {
                    pLine = (wxSFLineShape*)pShape;
                    ptnode = pLine->GetControlPoints().GetFirst();
                    while(ptnode)
                    {
                        pt = ptnode->GetData();
                        dx = handle.GetDelta().x - (pt->x - (GetAbsolutePosition().x + sfDEFAULT_ME_OFFSET))/(GetRectSize().x - 2*sfDEFAULT_ME_OFFSET)*handle.GetDelta().x;
                        pt->x += dx;
                        pt->x = floor(pt->x);
                        ptnode = ptnode->GetNext();
                    }
			    }
			}

			node = node->GetNext();
		}
	}
}

void wxSFMultiSelRect::OnBottomHandle(wxSFShapeHandle& handle)
{
	if(GetParentCanvas()  && !AnyHeightExceeded(handle.GetDelta()))
	{
        wxXS::RealPointList::compatibility_iterator ptnode;
	    wxSFLineShape* pLine;
	    wxRealPoint* pt;

		double dy, sy = (GetRectSize().y - 2*sfDEFAULT_ME_OFFSET + handle.GetDelta().y)/(GetRectSize().y - 2*sfDEFAULT_ME_OFFSET);

		ShapeList m_lstSelection;
		GetParentCanvas()->GetSelectedShapes(m_lstSelection);

		ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
		while(node)
		{
			wxSFShapeBase* pShape = node->GetData();

            if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
            {
                dy = (pShape->GetAbsolutePosition().y - (GetAbsolutePosition().y + sfDEFAULT_ME_OFFSET))/(GetRectSize().y - 2*sfDEFAULT_ME_OFFSET)*handle.GetDelta().y;

                if(pShape->ContainsStyle(sfsSIZE_CHANGE))pShape->Scale(1, sy, sfWITHCHILDREN);
                if(pShape->ContainsStyle(sfsPOSITION_CHANGE))pShape->MoveBy(0, dy);
				
				pShape->FitToChildren();
            }
            else
            {
                if(pShape->ContainsStyle(sfsPOSITION_CHANGE))
                {
                    pLine = (wxSFLineShape*)pShape;
                    ptnode = pLine->GetControlPoints().GetFirst();
                    while(ptnode)
                    {
                        pt = ptnode->GetData();
                        dy = ( pt->y - (GetAbsolutePosition().y + sfDEFAULT_ME_OFFSET))/(GetRectSize().y - 2*sfDEFAULT_ME_OFFSET)*handle.GetDelta().y;
                        pt->y += dy;
                        pt->y = floor(pt->y);
                        ptnode = ptnode->GetNext();
                    }
                }
            }

            node = node->GetNext();
		}
	}
}

void wxSFMultiSelRect::OnTopHandle(wxSFShapeHandle& handle)
{
	if(GetParentCanvas()  && !AnyHeightExceeded(wxPoint(0, -handle.GetDelta().y)))
	{
	    wxXS::RealPointList::compatibility_iterator ptnode;
	    wxSFLineShape* pLine;
	    wxRealPoint* pt;

		double dy, sy = (GetRectSize().y - 2*sfDEFAULT_ME_OFFSET - handle.GetDelta().y)/(GetRectSize().y - 2*sfDEFAULT_ME_OFFSET);

		ShapeList m_lstSelection;
		GetParentCanvas()->GetSelectedShapes(m_lstSelection);

		ShapeList::compatibility_iterator node = m_lstSelection.GetFirst();
		while(node)
		{
			wxSFShapeBase* pShape = node->GetData();

            if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
            {
                if(pShape->ContainsStyle(sfsPOSITION_CHANGE))
                {
                    if(pShape->GetParentShape())
                    {
                        pShape->SetRelativePosition(pShape->GetRelativePosition().x, pShape->GetRelativePosition().y*sy);
                    }
                    else
                    {
                        double dy = handle.GetDelta().y - (pShape->GetAbsolutePosition().y - (GetAbsolutePosition().y + sfDEFAULT_ME_OFFSET))/(GetRectSize().y - 2*sfDEFAULT_ME_OFFSET)*handle.GetDelta().y;
                        pShape->MoveBy(0, dy);
                    }
                }

                if(pShape->ContainsStyle(sfsSIZE_CHANGE))pShape->Scale(1, sy, sfWITHCHILDREN);
				
				pShape->FitToChildren();
            }
            else
            {
                if(pShape->ContainsStyle(sfsPOSITION_CHANGE))
                {
                    pLine = (wxSFLineShape*)pShape;
                    ptnode = pLine->GetControlPoints().GetFirst();
                    while(ptnode)
                    {
                        pt = ptnode->GetData();
                        dy = handle.GetDelta().y - (pt->y - (GetAbsolutePosition().y + sfDEFAULT_ME_OFFSET))/(GetRectSize().y - 2*sfDEFAULT_ME_OFFSET)*handle.GetDelta().y;
                        pt->y += dy;
                        pt->y = floor(pt->y);
                        ptnode = ptnode->GetNext();
                    }
                }
            }

            node = node->GetNext();
		}
	}
}
