/***************************************************************
 * Name:      ShapeBase.h
 * Purpose:   Implements shape base class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"
#include "wx/wxsf/SFEvents.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/ShapeBase.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/TextShape.h"
#include "wx/wxsf/GridShape.h"
#include "wx/wxsf/ControlShape.h"
#include "wx/wxsf/CommonFcn.h"

#include <wx/listimpl.cpp>

using namespace wxSFCommonFcn;

WX_DEFINE_EXPORTED_LIST(ShapeList);

ShapeList m_lstProcessed;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFShapeBase, xsSerializable);

wxSFShapeBase::wxSFShapeBase(void)
{
	// initialize data members
	m_fSelected = false;
	m_fMouseOver = false;
	m_fFirstMove = false;
	m_fHighlighParent = false;
	m_pUserData = NULL;

	// archived properties
	m_fVisible = sfdvBASESHAPE_VISIBILITY;
	m_fActive = sfdvBASESHAPE_ACTIVITY;
	m_nStyle = sfdvBASESHAPE_DEFAULT_STYLE;
	m_nRelativePosition = sfdvBASESHAPE_POSITION;
	m_nHoverColor = sfdvBASESHAPE_HOVERCOLOUR;
	m_nVAlign = sfdvBASESHAPE_VALIGN;
	m_nHAlign = sfdvBASESHAPE_HALIGN;
	m_nVBorder = sfdvBASESHAPE_VBORDER;
	m_nHBorder = sfdvBASESHAPE_HBORDER;
	m_nCustomDockPoint = sfdvBASESHAPE_DOCK_POINT;

    // mark serialized properties
	MarkSerializableDataMembers();

	m_lstHandles.DeleteContents(true);
	m_lstConnectionPts.DeleteContents(true);
}

wxSFShapeBase::wxSFShapeBase(const wxRealPoint& pos, wxSFDiagramManager* manager)
{
	wxASSERT_MSG(manager, wxT("Shape manager has not been properly set in shape constructor."));

	// initialize data members
	SetParentManager(manager);
    m_pUserData = NULL;

	if(m_pParentManager)
	{
		if( GetShapeManager()->GetShapeCanvas() )m_nHoverColor = GetShapeManager()->GetShapeCanvas()->GetHoverColour();
		else
            m_nHoverColor = sfdvBASESHAPE_HOVERCOLOUR;
	}
	else
	{
		m_nHoverColor = sfdvBASESHAPE_HOVERCOLOUR;
	}

	m_fSelected = false;
	m_fMouseOver = false;
	m_fFirstMove = false;
	m_fHighlighParent = false;

	// archived properties
	m_fVisible = sfdvBASESHAPE_VISIBILITY;
	m_fActive = sfdvBASESHAPE_ACTIVITY;
	m_nStyle = sfdvBASESHAPE_DEFAULT_STYLE;
	m_nVAlign = sfdvBASESHAPE_VALIGN;
	m_nHAlign = sfdvBASESHAPE_HALIGN;
	m_nVBorder = sfdvBASESHAPE_VBORDER;
	m_nHBorder = sfdvBASESHAPE_HBORDER;
	m_nCustomDockPoint = sfdvBASESHAPE_DOCK_POINT;

	wxSFShapeBase* m_pParentShape = GetParentShape();
	if(m_pParentShape)m_nRelativePosition = pos - GetParentAbsolutePosition();
	else
		m_nRelativePosition = sfdvBASESHAPE_POSITION;

	// mark serialized properties
	MarkSerializableDataMembers();

	m_lstHandles.DeleteContents(true);
	m_lstConnectionPts.DeleteContents(true);
}

wxSFShapeBase::wxSFShapeBase(const wxSFShapeBase& obj) : xsSerializable(obj)
{
	// initialize data members
	m_fSelected = false;
	m_fMouseOver = false;
	m_fFirstMove = false;
	m_fHighlighParent = false;

	m_fVisible = obj.m_fVisible;
	m_fActive = obj.m_fActive;
	m_nStyle = obj.m_nStyle;
	m_nVAlign = obj.m_nVAlign;
	m_nHAlign = obj.m_nHAlign;
	m_nVBorder = obj.m_nVBorder;
	m_nHBorder = obj.m_nHBorder;
	m_nCustomDockPoint = obj.m_nCustomDockPoint;

	m_nHoverColor = obj.m_nHoverColor;
	m_nRelativePosition = obj.m_nRelativePosition;

	WX_APPEND_ARRAY(m_arrAcceptedChildren, obj.m_arrAcceptedChildren);
	WX_APPEND_ARRAY(m_arrAcceptedSrcNeighbours, obj.m_arrAcceptedSrcNeighbours);
	WX_APPEND_ARRAY(m_arrAcceptedTrgNeighbours, obj.m_arrAcceptedTrgNeighbours);
	WX_APPEND_ARRAY(m_arrAcceptedConnections, obj.m_arrAcceptedConnections);

	if( obj.m_pUserData )
	{
		SetUserData((xsSerializable*)obj.m_pUserData->Clone());
	}
	else
		m_pUserData = NULL;

	// copy handles
	wxSFShapeHandle *pHandle;
	HandleList::compatibility_iterator node = obj.m_lstHandles.GetFirst();
	while(node)
	{
		pHandle = new wxSFShapeHandle(*node->GetData());
		pHandle->SetParentShape(this);
		m_lstHandles.Append(pHandle);

		node = node->GetNext();
	}
    m_lstHandles.DeleteContents(true);
	
	// copy connection points
	wxSFConnectionPoint *pConnPt;
	ConnectionPointList::compatibility_iterator node2 = obj.m_lstConnectionPts.GetFirst();
	while(node2)
	{
		pConnPt = new wxSFConnectionPoint(*(wxSFConnectionPoint*)node2->GetData());
		pConnPt->SetParentShape(this);
		m_lstConnectionPts.Append(pConnPt);

		node2 = node2->GetNext();
	}
	m_lstConnectionPts.DeleteContents(true);

	// mark serialized properties
	MarkSerializableDataMembers();
}

wxSFShapeBase::~wxSFShapeBase(void)
{
	// clear handles
	m_lstHandles.Clear();
	// clear connection points
	m_lstConnectionPts.Clear();
	
	// delete user data
	if(m_pUserData && (m_nStyle & sfsDELETE_USER_DATA))delete m_pUserData;
}

void wxSFShapeBase::MarkSerializableDataMembers()
{
    // mark serialized properties
    XS_SERIALIZE_EX(m_fActive, wxT("active"), sfdvBASESHAPE_ACTIVITY);
    XS_SERIALIZE_EX(m_fVisible, wxT("visibility"), sfdvBASESHAPE_VISIBILITY);
    XS_SERIALIZE_EX(m_nStyle, wxT("style"), sfdvBASESHAPE_DEFAULT_STYLE);
    XS_SERIALIZE(m_arrAcceptedChildren, wxT("accepted_children"));
    XS_SERIALIZE(m_arrAcceptedConnections, wxT("accepted_connections"));
    XS_SERIALIZE(m_arrAcceptedSrcNeighbours, wxT("accepted_src_neighbours"));
    XS_SERIALIZE(m_arrAcceptedTrgNeighbours, wxT("accepted_trg_neighbours"));
    XS_SERIALIZE_EX(m_nHoverColor, wxT("hover_color"), sfdvBASESHAPE_HOVERCOLOUR);
    XS_SERIALIZE(m_nRelativePosition, wxT("relative_position"));
    XS_SERIALIZE_LONG_EX(m_nHAlign, wxT("halign"), sfdvBASESHAPE_HALIGN);
    XS_SERIALIZE_LONG_EX(m_nVAlign, wxT("valign"), (long)sfdvBASESHAPE_VALIGN);
    XS_SERIALIZE_EX(m_nHBorder, wxT("hborder"), sfdvBASESHAPE_HBORDER);
    XS_SERIALIZE_EX(m_nVBorder, wxT("vborder"), sfdvBASESHAPE_VBORDER);
	XS_SERIALIZE_EX(m_nCustomDockPoint, wxT("custom_dock_point"), sfdvBASESHAPE_DOCK_POINT);
	XS_SERIALIZE(m_lstConnectionPts, wxT("connection_points"));
    XS_SERIALIZE(m_pUserData, wxT("user_data"));
}

//----------------------------------------------------------------------------------//
// Public functions
//----------------------------------------------------------------------------------//

wxRect wxSFShapeBase::GetBoundingBox()
{
	// HINT: overload it for custom actions...

	return wxRect();
}

void wxSFShapeBase::GetCompleteBoundingBox(wxRect &rct, int mask)
{
    m_lstProcessed.Clear();
    _GetCompleteBoundingBox(rct, mask);
}

void wxSFShapeBase::_GetCompleteBoundingBox(wxRect &rct, int mask)
{
    //wxASSERT(m_pParentManager);
    if(!m_pParentManager)return;

    if( m_lstProcessed.IndexOf(this) != wxNOT_FOUND )return;
    else
        m_lstProcessed.Append(this);

	ShapeList lstChildren;
	//SerializableList lstConnections;

	// firts, get bounding box of the current shape
	if(mask & bbSELF)
	{
		if(rct.IsEmpty())rct = this->GetBoundingBox().Inflate( abs(m_nHBorder), abs(m_nVBorder) );
		else
			rct.Union(this->GetBoundingBox().Inflate( abs(m_nHBorder), abs(m_nVBorder)) );

		// add also shadow offset if neccessary
        if( (mask & bbSHADOW) && (m_nStyle & sfsSHOW_SHADOW) && GetParentCanvas() )
        {
            wxRealPoint nOffset = GetParentCanvas()->GetShadowOffset();

            if( nOffset.x < 0 )
            {
                rct.SetX(rct.GetX() + (int)nOffset.x);
                rct.SetWidth(rct.GetWidth() - (int)nOffset.x);
            }
            else
                rct.SetWidth(rct.GetWidth() + (int)nOffset.x);

            if( nOffset.y < 0 )
            {
                rct.SetY(rct.GetY() + (int)nOffset.y);
                rct.SetHeight(rct.GetHeight() - (int)nOffset.y);
            }
            else
                rct.SetHeight(rct.GetHeight() + (int)nOffset.y);;
        }
	}
	else
		mask |= bbSELF;

	// get list of all connection lines assigned to the shape and find their child shapes
	if(mask & bbCONNECTIONS)
	{
		wxSFShapeBase *pLine;

        ShapeList lstLines;
        GetAssignedConnections( CLASSINFO(wxSFLineShape), lineBOTH, lstLines );

		ShapeList::compatibility_iterator node = lstLines.GetFirst();
		while(node)
		{
			pLine = node->GetData();

			//rct.Union(pLine->GetBoundingBox());
			lstChildren.Append(pLine);

			// get children of the connections
			pLine->GetChildShapes(sfANY, lstChildren);

			node = node->GetNext();
		}
	}

	// get children of this shape
	if(mask & bbCHILDREN)
	{
		this->GetChildShapes(sfANY, lstChildren, sfNORECURSIVE);

		// now, call this function for all children recursively...
		ShapeList::compatibility_iterator node = lstChildren.GetFirst();
		while(node)
		{
		    node->GetData()->_GetCompleteBoundingBox(rct, mask);
			node = node->GetNext();
		}
	}
}

bool wxSFShapeBase::Contains(const wxPoint& pos)
{
	// HINT: overload it for custom actions...

	return this->GetBoundingBox().Contains(pos);
}

bool wxSFShapeBase::IsInside(const wxRect& rct)
{
	// HINT: overload it for custom actions...

	return rct.Contains(this->GetBoundingBox());
}

bool wxSFShapeBase::Intersects(const wxRect& rct)
{
	// HINT: overload it for custom actions...

	return rct.Intersects(this->GetBoundingBox());
}

wxRealPoint wxSFShapeBase::GetAbsolutePosition()
{
	// HINT: overload it for custom actions...

	wxSFShapeBase* pParentShape = GetParentShape();
	if(pParentShape)
	{
		return m_nRelativePosition + GetParentAbsolutePosition();
	}
	else
		return m_nRelativePosition;
}

wxRealPoint wxSFShapeBase::GetCenter()
{
    // HINT: override it for custom actions

    wxRect shpBB = this->GetBoundingBox();
    return wxRealPoint(shpBB.GetLeft() + shpBB.GetWidth()/2, shpBB.GetTop() + shpBB.GetHeight()/2);
}

wxRealPoint wxSFShapeBase::GetBorderPoint(const wxRealPoint& start, const wxRealPoint& end)
{
    // HINT: override it for custom actions
	
	wxUnusedVar( start );
	wxUnusedVar( end );

    return wxRealPoint();
}

void wxSFShapeBase::ShowHandles(bool show)
{
	HandleList::compatibility_iterator node = m_lstHandles.GetFirst();
	while(node)
	{
		node->GetData()->Show(show);
		node = node->GetNext();
	}
}

void wxSFShapeBase::MoveTo(double x, double y)
{
	// HINT: overload it for custom actions...
	
	m_nRelativePosition = wxRealPoint(x, y) - GetParentAbsolutePosition();

	/*wxSFShapeBase* m_pParentShape = GetParentShape();
	if(m_pParentShape)
	{
		m_nRelativePosition = wxRealPoint(x, y) - m_pParentShape->GetAbsolutePosition();
	}
	else
	{
		m_nRelativePosition.x = x;
		m_nRelativePosition.y = y;
	}*/
}

void wxSFShapeBase::MoveTo(const wxRealPoint& pos)
{
	this->MoveTo(pos.x, pos.y);
}

void wxSFShapeBase::MoveBy(double x, double y)
{
	// HINT: overload it for custom actions...

	m_nRelativePosition.x += x;
	m_nRelativePosition.y += y;
}

void wxSFShapeBase::MoveBy(const wxRealPoint& delta)
{
	this->MoveBy(delta.x, delta.y);
}

void wxSFShapeBase::Scale(double x, double y, bool children)
{
	// HINT: overload it for custom actions...

	if(children)
	{
	    ScaleChildren(x, y);
	}

    //this->Update();
}

void wxSFShapeBase::Scale(const wxRealPoint& scale, bool children)
{
	this->Scale(scale.x, scale.y, children);
}

void wxSFShapeBase::ScaleChildren(double x, double y)
{
	ShapeList m_lstChildren;
	GetChildShapes(sfANY, m_lstChildren, sfRECURSIVE);

	ShapeList::compatibility_iterator node = m_lstChildren.GetFirst();
	while(node)
	{
		wxSFShapeBase* pShape = node->GetData();

        if((pShape->GetStyle() & sfsSIZE_CHANGE) && !pShape->IsKindOf(CLASSINFO(wxSFTextShape)))
		{
		    pShape->Scale(x, y, sfWITHOUTCHILDREN);
		}

		if( (pShape->GetStyle() & sfsPOSITION_CHANGE) && ((pShape->GetVAlign() == valignNONE) || (pShape->GetHAlign() == halignNONE)) )
		{
            pShape->SetRelativePosition(pShape->m_nRelativePosition.x*x, pShape->m_nRelativePosition.y*y);
		}

        // re-align shapes which have set any alignment mode
		pShape->DoAlignment();

		node = node->GetNext();
	}
}

void wxSFShapeBase::Update()
{
    // do self-alignment
    DoAlignment();

    // do alignment of shape's children (if required)
    //if( !this->IsKindOf(CLASSINFO(wxSFLineShape)) )
    //{
	SerializableList::compatibility_iterator node = GetFirstChildNode();
	while(node)
	{
		((wxSFShapeBase*)node->GetData())->DoAlignment();
		node = node->GetNext();
	}
    //}

    // fit the shape to its children
    this->FitToChildren();

    // do it recursively on all parent shapes
    if( GetParentShape() )GetParentShape()->Update();
}

bool wxSFShapeBase::AcceptCurrentlyDraggedShapes()
{
    //wxASSERT(m_pParentManager);
    //wxASSERT(m_pParentManager->GetShapeCanvas());

	if(!m_pParentManager || !GetShapeManager()->GetShapeCanvas())return false;

    if(!IsChildAccepted(wxT("All")))
    {
        ShapeList lstSelection;
        GetShapeManager()->GetShapeCanvas()->GetSelectedShapes(lstSelection);

        ShapeList::compatibility_iterator node = lstSelection.GetFirst();
        while(node)
        {
            if(m_arrAcceptedChildren.Index(node->GetData()->GetClassInfo()->GetClassName()) == wxNOT_FOUND)return false;
            node = node->GetNext();
        }
    }
	return true;
}

bool wxSFShapeBase::IsChildAccepted(const wxString& type)
{
    if( m_arrAcceptedChildren.Index(type) != wxNOT_FOUND )return true;
    else if( m_arrAcceptedChildren.Index(wxT("All")) != wxNOT_FOUND )return true;
    else
        return false;
}

bool wxSFShapeBase::IsSrcNeighbourAccepted(const wxString& type)
{
    if( m_arrAcceptedSrcNeighbours.Index(type) != wxNOT_FOUND )return true;
    else if( m_arrAcceptedSrcNeighbours.Index(wxT("All")) != wxNOT_FOUND )return true;
    else
        return false;
}

bool wxSFShapeBase::IsTrgNeighbourAccepted(const wxString& type)
{
    if( m_arrAcceptedTrgNeighbours.Index(type) != wxNOT_FOUND )return true;
    else if( m_arrAcceptedTrgNeighbours.Index(wxT("All")) != wxNOT_FOUND )return true;
    else
        return false;
}

bool wxSFShapeBase::IsConnectionAccepted(const wxString& type)
{
    if( m_arrAcceptedConnections.Index(type) != wxNOT_FOUND )return true;
    else if( m_arrAcceptedConnections.Index(wxT("All")) != wxNOT_FOUND )return true;
    else
        return false;
}

void wxSFShapeBase::FitToChildren()
{
	// HINT: overload it for custom actions...
}

wxSFShapeBase* wxSFShapeBase::GetParentShape()
{
    //wxASSERT(m_pParentManager);

    if(m_pParentManager && (m_pParentManager->GetRootItem() != m_pParentItem) )
    {
        return (wxSFShapeBase*)m_pParentItem;
    }
    else
        return NULL;
}

wxSFShapeBase* wxSFShapeBase::GetGrandParentShape()
{
	wxSFShapeBase *pGrandPa = NULL;

    if(m_pParentManager)
	{
		if( m_pParentItem && (m_pParentManager->GetRootItem() != m_pParentItem) )
		{
			pGrandPa = ((wxSFShapeBase*)m_pParentItem)->GetGrandParentShape();
		}
		else if( m_pParentItem && (m_pParentManager->GetRootItem() == m_pParentItem) )
		{
			pGrandPa = this;
		}
    }

	return pGrandPa;
}

bool wxSFShapeBase::IsDescendant(wxSFShapeBase *parent)
{
	ShapeList lstChildren;
	
	if( parent ) parent->GetChildShapes( NULL, lstChildren, sfRECURSIVE );
	
	return ( lstChildren.IndexOf(this) != wxNOT_FOUND );
}

bool wxSFShapeBase::IsAncestor(wxSFShapeBase *child)
{
	ShapeList lstChildren;
	
	GetChildShapes( NULL, lstChildren, sfRECURSIVE );
	
	return ( lstChildren.IndexOf(child) != wxNOT_FOUND );	
}

void wxSFShapeBase::SetUserData(xsSerializable* data)
{
    m_pUserData = data;
    if( m_pUserData )
    {
        // let the associated data to know its parent shape
        m_pUserData->SetParent(this);
    }
}

//----------------------------------------------------------------------------------//
// Drawing functions
//----------------------------------------------------------------------------------//

void wxSFShapeBase::Refresh()
{
    Refresh(this->GetBoundingBox());
}

void wxSFShapeBase::Draw(wxDC& dc, bool children)
{
	if(!m_pParentManager || !GetShapeManager()->GetShapeCanvas())return;
    if(!m_fVisible)return;

    // draw the shape shadow if required
    if( !m_fSelected && (m_nStyle & sfsSHOW_SHADOW) )
    {
        this->DrawShadow(dc);
    }

	// first, draw itself
	if(m_fMouseOver && ( m_fHighlighParent || (m_nStyle & sfsHOVERING) ))
	{
		if(m_fHighlighParent)
		{
			this->DrawHighlighted(dc);
		}
		else
			this->DrawHover(dc);
	}
	else
	    this->DrawNormal(dc);

	if(m_fSelected)this->DrawSelected(dc);
	
	// ... then draw connection points ...
	for( ConnectionPointList::iterator it = m_lstConnectionPts.begin(); it != m_lstConnectionPts.end(); ++it )
	{
		((wxSFConnectionPoint*)(*it))->Draw(dc);
	}

	// ... then draw child shapes
	if(children)
	{
        SerializableList::compatibility_iterator node = GetFirstChildNode();
        while(node)
        {
            ((wxSFShapeBase*)node->GetData())->Draw(dc);
            node = node->GetNext();
        }
	}
}

void wxSFShapeBase::DrawNormal(wxDC& dc)
{
	// HINT: overload it for custom actions...
	
	wxUnusedVar( dc );
}

void wxSFShapeBase::DrawSelected(wxDC& dc)
{
	// HINT: overload it for custom actions...

	if( m_nStyle & sfsSHOW_HANDLES )
	{
		HandleList::compatibility_iterator node = m_lstHandles.GetFirst();
		while(node)
		{
			node->GetData()->Draw(dc);
			node = node->GetNext();
		}
	}
}

void wxSFShapeBase::DrawHover(wxDC& dc)
{
	// HINT: overload it for custom actions...
	
	wxUnusedVar( dc );
}

void wxSFShapeBase::DrawHighlighted(wxDC& dc)
{
	// HINT: overload it for custom actions...
	
	wxUnusedVar( dc );
}

void wxSFShapeBase::DrawShadow(wxDC& dc)
{
	// HINT: overload it for custom actions...
	
	wxUnusedVar( dc );
}

//----------------------------------------------------------------------------------//
// other functions
//----------------------------------------------------------------------------------//

void wxSFShapeBase::CreateHandles()
{
	// HINT: overload it for custom actions...
}

void wxSFShapeBase::GetChildShapes(wxClassInfo *type, ShapeList& children, bool recursive, xsSerializable::SEARCHMODE mode )
{
    if( recursive ) GetChildrenRecursively(type, (SerializableList&)children, mode);
    else
        GetChildren(type, (SerializableList&)children);
}

void wxSFShapeBase::GetNeighbours(ShapeList& neighbours, wxClassInfo *shapeInfo, CONNECTMODE condir, bool direct)
{
    if( !this->IsKindOf(CLASSINFO(wxSFLineShape)) )
    {
        m_lstProcessed.Clear();
        this->_GetNeighbours(neighbours, shapeInfo, condir, direct);
        // delete starting object if necessary (can be added in a case of complex connection network)
        neighbours.DeleteObject(this);
    }
}

void wxSFShapeBase::GetAssignedConnections(wxClassInfo* shapeInfo, wxSFShapeBase::CONNECTMODE mode, ShapeList& lines)
{
	wxASSERT(m_pParentManager);
	
	if( m_pParentManager ) GetShapeManager()->GetAssignedConnections( this, shapeInfo, mode, lines);
}

void wxSFShapeBase::_GetNeighbours(ShapeList& neighbours, wxClassInfo *shapeInfo, CONNECTMODE condir, bool direct)
{
    //wxASSERT(m_pParentManager);

    if( m_pParentManager )
    {
        if( m_lstProcessed.IndexOf(this) != wxNOT_FOUND )return;

        ShapeList lstConnections;
        wxSFLineShape *pLine;
        wxSFShapeBase *pOposite = NULL;

        GetAssignedConnections(shapeInfo, condir, lstConnections);

        // find oposite shpes in direct branches
        ShapeList::compatibility_iterator node = lstConnections.GetFirst();
        while(node)
        {
            pLine = (wxSFLineShape*)node->GetData();
            switch(condir)
            {
                case lineSTARTING:
                    pOposite = GetShapeManager()->FindShape(pLine->GetTrgShapeId());
                    break;

                case lineENDING:
                    pOposite = GetShapeManager()->FindShape(pLine->GetSrcShapeId());
                    break;

                case lineBOTH:
                    {
                        if(GetId() == pLine->GetSrcShapeId())pOposite = GetShapeManager()->FindShape(pLine->GetTrgShapeId());
                        else
                            pOposite = GetShapeManager()->FindShape(pLine->GetSrcShapeId());
                    }
            }

            // add oposite shape to the list (if applicable)
            if(pOposite && !pOposite->IsKindOf(CLASSINFO(wxSFLineShape)) && ( neighbours.IndexOf(pOposite) == wxNOT_FOUND )) neighbours.Append(pOposite);

            // find next shapes
            if( !direct && pOposite )
            {
                // in the case of indirect branches we must differentiate between connections
                // and ordinary shapes
                m_lstProcessed.Append(this);

                if( pOposite->IsKindOf(CLASSINFO(wxSFLineShape)) )
                {
                    pLine = (wxSFLineShape*)pOposite;
                    switch(condir)
                    {
                        case lineSTARTING:
                            {
                                pOposite = GetShapeManager()->FindShape( pLine->GetSrcShapeId() );

                                if( pOposite->IsKindOf(CLASSINFO(wxSFLineShape)) )pOposite->_GetNeighbours(neighbours, shapeInfo, condir, direct);
                                else if( neighbours.IndexOf(pOposite) == wxNOT_FOUND )neighbours.Append(pOposite);
                            }
                            break;

                        case lineENDING:
                            {
                                pOposite = GetShapeManager()->FindShape( pLine->GetTrgShapeId() );

                                if( pOposite->IsKindOf(CLASSINFO(wxSFLineShape)) )pOposite->_GetNeighbours(neighbours, shapeInfo, condir, direct);
                                else if( neighbours.IndexOf(pOposite) == wxNOT_FOUND )neighbours.Append(pOposite);
                            }
                            break;

                        case lineBOTH:
                            {
                                pOposite = GetShapeManager()->FindShape( pLine->GetSrcShapeId() );
                                if( pOposite->IsKindOf(CLASSINFO(wxSFLineShape)) )pOposite->_GetNeighbours(neighbours, shapeInfo, condir, direct);
                                else if( neighbours.IndexOf(pOposite) == wxNOT_FOUND )neighbours.Append(pOposite);

                                pOposite = GetShapeManager()->FindShape( pLine->GetTrgShapeId() );
                                if( pOposite->IsKindOf(CLASSINFO(wxSFLineShape)) )pOposite->_GetNeighbours(neighbours, shapeInfo, condir, direct);
                                else if( neighbours.IndexOf(pOposite) == wxNOT_FOUND )neighbours.Append(pOposite);
                            }
                            break;
                    }
                }
                else
                    pLine->_GetNeighbours(neighbours, shapeInfo, condir, direct);
            }

            node = node->GetNext();
        }
    }
}

wxSFShapeCanvas* wxSFShapeBase::GetParentCanvas()
{
    if( !m_pParentManager ) return NULL;

    return GetShapeManager()->GetShapeCanvas();
}

wxRealPoint wxSFShapeBase::GetParentAbsolutePosition()
{
	wxSFShapeBase * pParentShape = GetParentShape();
	if(pParentShape)
	{
		if( m_pParentItem->IsKindOf(CLASSINFO(wxSFLineShape)) && m_nCustomDockPoint != sfdvBASESHAPE_DOCK_POINT)
		{
			return ((wxSFLineShape*) m_pParentItem)->GetDockPointPosition( m_nCustomDockPoint );
		}
		else
			return pParentShape->GetAbsolutePosition();
	}
	
	return wxRealPoint( 0, 0 );
}


void wxSFShapeBase::Refresh(const wxRect& rct)
{
	if(m_pParentManager && GetShapeManager()->GetShapeCanvas())
	{
		GetShapeManager()->GetShapeCanvas()->RefreshCanvas(false, rct);
	}
}

wxSFShapeHandle* wxSFShapeBase::GetHandle(wxSFShapeHandle::HANDLETYPE type, long id)
{
	HandleList::compatibility_iterator node = m_lstHandles.GetFirst();
	while(node)
	{
		wxSFShapeHandle *hnd = node->GetData();
		if((hnd->GetType() == type) && ((id == -1)||(id == hnd->GetId())))return hnd;
		node = node->GetNext();
	}
	return NULL;
}

void wxSFShapeBase::AddHandle(wxSFShapeHandle::HANDLETYPE type, long id)
{
	wxSFShapeHandle* pHnd = GetHandle(type, id);
	if(!pHnd)
	{
		m_lstHandles.Append(new wxSFShapeHandle(this, type, id));
	}
}

void wxSFShapeBase::RemoveHandle(wxSFShapeHandle::HANDLETYPE type, long id)
{
	wxSFShapeHandle* pHnd = GetHandle(type, id);
	if(pHnd)
	{
		m_lstHandles.DeleteObject(pHnd);
	}
}

wxSFConnectionPoint* wxSFShapeBase::GetConnectionPoint(wxSFConnectionPoint::CPTYPE type, long id)
{
	for( ConnectionPointList::iterator it = m_lstConnectionPts.begin(); it != m_lstConnectionPts.end(); ++it )
	{
		wxSFConnectionPoint *pCp = (wxSFConnectionPoint*)(*it);
		if( pCp->GetType() == type && pCp->GetId() == id ) return pCp;
	}
	
	return NULL;
}

wxSFConnectionPoint* wxSFShapeBase::GetNearestConnectionPoint(const wxRealPoint& pos)
{
	int nMinDist = INT_MAX;
	
	wxSFConnectionPoint *pConnPt = NULL;
	
	for( ConnectionPointList::iterator it = m_lstConnectionPts.begin(); it != m_lstConnectionPts.end(); ++it )
	{
		int nCurrDistance = Distance( pos, ((wxSFConnectionPoint*)(*it))->GetConnectionPoint() );
		if( nCurrDistance < nMinDist )
		{
			nMinDist = nCurrDistance;
			pConnPt = ((wxSFConnectionPoint*)(*it));
		}
	}
	
	return pConnPt;
}

void wxSFShapeBase::AddConnectionPoint(wxSFConnectionPoint::CPTYPE type, bool persistent)
{
	if( !GetConnectionPoint( type ) )
	{
		wxSFConnectionPoint *cp = new wxSFConnectionPoint( this, type);
		cp->EnableSerialization( persistent );
		m_lstConnectionPts.Append( cp );
	}
}

void wxSFShapeBase::AddConnectionPoint(const wxRealPoint& relpos, long id, bool persistent)
{
	wxSFConnectionPoint *cp = new wxSFConnectionPoint( this, relpos, id );
	cp->EnableSerialization( persistent );
	m_lstConnectionPts.Append( cp );
}

void wxSFShapeBase::AddConnectionPoint(wxSFConnectionPoint* cp, bool persistent)
{
	if( cp )
	{
		cp->EnableSerialization( persistent );
		m_lstConnectionPts.Append( cp );
	}
}

void wxSFShapeBase::RemoveConnectionPoint(wxSFConnectionPoint::CPTYPE type)
{
	wxSFConnectionPoint *pConnPt = GetConnectionPoint( type );
	if( pConnPt )
	{
		m_lstConnectionPts.DeleteObject( pConnPt );
	}
}

void wxSFShapeBase::DoAlignment()
{
    wxSFShapeBase *pParent = this->GetParentShape();

	if(pParent && !pParent->IsKindOf(CLASSINFO(wxSFGridShape)))
    {
        wxRect parentBB;
		wxRealPoint linePos, lineStart, lineEnd;

		wxSFLineShape *pParentLine =  wxDynamicCast( pParent, wxSFLineShape );
        if( pParentLine )
        {
            //linePos = pParentLine->GetAbsolutePosition();
            linePos = GetParentAbsolutePosition();
            parentBB = wxRect((int)linePos.x, (int)linePos.y, 1, 1);
        }
        else
            parentBB = pParent->GetBoundingBox();

        wxRect shapeBB = this->GetBoundingBox();

        // do vertical alignment
        switch(m_nVAlign)
        {
            case valignTOP:
                m_nRelativePosition.y = m_nVBorder;
                break;

            case valignMIDDLE:
                m_nRelativePosition.y = parentBB.GetHeight()/2 - shapeBB.GetHeight()/2;
                break;

            case valignBOTTOM:
                m_nRelativePosition.y = parentBB.GetHeight() - shapeBB.GetHeight() - m_nVBorder;
                break;

            case valignEXPAND:
                if( ContainsStyle( sfsSIZE_CHANGE ) )
                {
                    m_nRelativePosition.y = m_nVBorder;
                    this->Scale( 1.f, double(parentBB.GetHeight() - 2*m_nVBorder)/shapeBB.GetHeight() );
                }
                break;
				
			case valignLINE_START:
				if( pParentLine )
				{
					pParentLine->GetLineSegment(0, lineStart, lineEnd);
					
					if( lineEnd.y >= lineStart.y ) m_nRelativePosition.y = lineStart.y - linePos.y + m_nVBorder;
					else
						m_nRelativePosition.y = lineStart.y - linePos.y - shapeBB.GetHeight() - m_nVBorder;
				}
				break;
				
			case valignLINE_END:
				if( pParentLine )
				{
					pParentLine->GetLineSegment(pParentLine->GetControlPoints().GetCount(), lineStart, lineEnd);
					
					if( lineEnd.y >= lineStart.y ) m_nRelativePosition.y = lineEnd.y - linePos.y - shapeBB.GetHeight() - m_nVBorder;
					else
						m_nRelativePosition.y = lineEnd.y - linePos.y + m_nVBorder;
				}
				break;

            default:
                break;
        }

        // do horizontal alignment
        switch(m_nHAlign)
        {
            case halignLEFT:
                m_nRelativePosition.x = m_nHBorder;
                break;

            case halignCENTER:
                m_nRelativePosition.x = parentBB.GetWidth()/2 - shapeBB.GetWidth()/2;
                break;

            case halignRIGHT:
                m_nRelativePosition.x = parentBB.GetWidth() - shapeBB.GetWidth() - m_nHBorder;
                break;

            case halignEXPAND:
                if( ContainsStyle( sfsSIZE_CHANGE ) )
                {
                    m_nRelativePosition.x = m_nHBorder;
                    this->Scale( double(parentBB.GetWidth() - 2*m_nHBorder)/shapeBB.GetWidth(), 1.f );
                }
                break;

			case halignLINE_START:
				if( pParentLine )
				{
					pParentLine->GetLineSegment(0, lineStart, lineEnd);
					
					if( lineEnd.x >= lineStart.x ) m_nRelativePosition.x = lineStart.x - linePos.x + m_nHBorder;
					else
						m_nRelativePosition.x = lineStart.x - linePos.x - shapeBB.GetWidth() - m_nHBorder;
				}
				break;
				
			case halignLINE_END:
				if( pParentLine )
				{
					pParentLine->GetLineSegment(pParentLine->GetControlPoints().GetCount(), lineStart, lineEnd);
					
					if( lineEnd.x >= lineStart.x ) m_nRelativePosition.x = lineEnd.x - linePos.x - shapeBB.GetWidth() - m_nHBorder;
					else
						m_nRelativePosition.x = lineEnd.x - linePos.x + m_nHBorder;
				}
				break;

            default:
                break;
        }
    }
}

//----------------------------------------------------------------------------------//
// Events handling
//----------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------//
// Public virtual event handlers

void wxSFShapeBase::OnLeftClick(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_LEFT_DOWN, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnRightClick(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_RIGHT_DOWN, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnLeftDoubleClick(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_LEFT_DCLICK, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnRightDoubleClick(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_RIGHT_DCLICK, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );;
	}
}

void wxSFShapeBase::OnBeginDrag(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_DRAG_BEGIN, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnDragging(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_DRAG, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnEndDrag(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_DRAG_END, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeHandleEvent evt( wxEVT_SF_SHAPE_HANDLE, this->GetId() );
		evt.SetShape( this );
		evt.SetHandle( handle );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnBeginHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeHandleEvent evt( wxEVT_SF_SHAPE_HANDLE_BEGIN, this->GetId() );
		evt.SetShape( this );
		evt.SetHandle( handle );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnEndHandle(wxSFShapeHandle& handle)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeHandleEvent evt( wxEVT_SF_SHAPE_HANDLE_END, this->GetId() );
		evt.SetShape( this );
		evt.SetHandle( handle );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

bool wxSFShapeBase::OnKey(int key)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeKeyEvent evt( wxEVT_SF_SHAPE_KEYDOWN, this->GetId() );
		evt.SetShape( this );
		evt.SetKeyCode( key );
		GetParentCanvas()->ProcessEvent( evt );
	}

    return TRUE;
}

void wxSFShapeBase::OnMouseEnter(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_MOUSE_ENTER, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnMouseOver(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_MOUSE_OVER, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnMouseLeave(const wxPoint& pos)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeMouseEvent evt( wxEVT_SF_SHAPE_MOUSE_LEAVE, this->GetId() );
		evt.SetShape( this );
		evt.SetMousePosition( pos );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

void wxSFShapeBase::OnChildDropped(const wxRealPoint& pos, wxSFShapeBase* child)
{
	// HINT: overload it for custom actions...
	
	if( this->ContainsStyle(sfsEMIT_EVENTS) && GetParentCanvas() )
	{
		wxSFShapeChildDropEvent evt( wxEVT_SF_SHAPE_CHILD_DROP, this->GetId() );
		evt.SetShape( this );
		evt.SetChildShape( child );
		GetParentCanvas()->ProcessEvent( evt );
	}
}

//----------------------------------------------------------------------------------//
// Original private event handlers

void wxSFShapeBase::_OnBeginDrag(const wxPoint& pos)
{
	if ( !m_fActive ) return;

	m_fFirstMove = true;
	this->OnBeginDrag(pos);
	
	if( GetParentShape() && (m_nStyle & sfsPROPAGATE_DRAGGING) )
	{
		GetParentShape()->_OnBeginDrag( pos );
	}
}

void wxSFShapeBase::_OnDragging(const wxPoint& pos)
{
    //wxASSERT(m_pParentManager);
    if( !m_pParentManager )return;

	if(m_fVisible && m_fActive && (m_nStyle & sfsPOSITION_CHANGE))
	{
		if(m_fFirstMove)
		{
			m_nMouseOffset = wxRealPoint(pos.x, pos.y) - this->GetAbsolutePosition();
		}

        // get shape BB BEFORE movement and combine it with BB of assigned lines
		wxRect prevBB;
		GetCompleteBoundingBox(prevBB, bbSELF | bbCONNECTIONS | bbCHILDREN | bbSHADOW);

		this->MoveTo(pos.x - m_nMouseOffset.x, pos.y - m_nMouseOffset.y);
        this->OnDragging(pos);
		
		// GUI controls in child control shapes must be updated explicitely
		wxSFControlShape *pCtrl;
		ShapeList lstChildCtrls;
		
		GetChildShapes( CLASSINFO(wxSFControlShape), lstChildCtrls, sfRECURSIVE );
		ShapeList::compatibility_iterator node = lstChildCtrls.GetFirst();
		while( node )
		{
			pCtrl = (wxSFControlShape*) node->GetData();
			pCtrl->UpdateControl();
			
			node = node->GetNext();
		}

        // get shape BB AFTER movement and combine it with BB of assigned lines
		wxRect currBB;
		GetCompleteBoundingBox(currBB, bbSELF | bbCONNECTIONS | bbCHILDREN | bbSHADOW);

		// update canvas
		Refresh( prevBB.Union(currBB) );

		m_fFirstMove = false;
	}
	
	if( GetParentShape() && (m_nStyle & sfsPROPAGATE_DRAGGING) )
	{
		GetParentShape()->_OnDragging( pos );
	}
}

void wxSFShapeBase::_OnEndDrag(const wxPoint& pos)
{
	if ( !m_fActive ) return;

    this->OnEndDrag(pos);
	
	if( GetParentShape() && (m_nStyle & sfsPROPAGATE_DRAGGING) )
	{
		GetParentShape()->_OnEndDrag( pos );
	}
}

void wxSFShapeBase::_OnMouseMove(const wxPoint& pos)
{
    //wxASSERT(m_pParentManager);
    if( !m_pParentManager )return;

	if(m_fVisible && m_fActive)
	{
		bool fUpdateShape = false;
		wxSFShapeCanvas *pCanvas = GetShapeManager()->GetShapeCanvas();

		// send the event to the shape handles too...
		HandleList::compatibility_iterator node = m_lstHandles.GetFirst();
		while(node)
		{
			node->GetData()->_OnMouseMove(pos);
			node = node->GetNext();
		}
		
		// send the event to the connection points too...
		ConnectionPointList::compatibility_iterator node2 = m_lstConnectionPts.GetFirst();
		while(node2)
		{
			((wxSFConnectionPoint*) node2->GetData())->_OnMouseMove(pos);
			node2 = node2->GetNext();
		}

		// determine, whether the shape should be highlighted for any reason
		if(pCanvas)
		{
		    switch(pCanvas->GetMode())
		    {
            case wxSFShapeCanvas::modeSHAPEMOVE:
                {
					if(ContainsStyle(sfsHIGHLIGHTING) && pCanvas->ContainsStyle(wxSFShapeCanvas::sfsHIGHLIGHTING))
                    {
                        if(pCanvas->GetShapeUnderCursor(wxSFShapeCanvas::searchUNSELECTED) == this)
                        {
                            fUpdateShape = m_fHighlighParent = AcceptCurrentlyDraggedShapes();
                        }
                    }
                }
                break;

            case wxSFShapeCanvas::modeHANDLEMOVE:
                {
                    if(ContainsStyle(sfsHOVERING) && pCanvas->ContainsStyle(wxSFShapeCanvas::sfsHOVERING))
                    {
                        if(pCanvas->GetShapeUnderCursor(wxSFShapeCanvas::searchUNSELECTED) == this)fUpdateShape = true;
                        m_fHighlighParent = false;
                    }
                }
                break;

            default:
                {
                    if(ContainsStyle(sfsHOVERING) && pCanvas->ContainsStyle(wxSFShapeCanvas::sfsHOVERING))
                    {
                        if(pCanvas->GetShapeUnderCursor() == this)fUpdateShape = true;
                        m_fHighlighParent = false;
                    }
                }
                break;
		    }
		}
		
		if(Contains(pos) && fUpdateShape)
		{
			if(!m_fMouseOver)
			{
				m_fMouseOver = true;
				this->OnMouseEnter(pos);
			}
			else
			    this->OnMouseOver(pos);

			Refresh();
		}
		else
		{
			if(m_fMouseOver)
			{
				m_fMouseOver = false;
				this->OnMouseLeave(pos);
				Refresh();
			}
		}
	}
}

void wxSFShapeBase::_OnKey(int key)
{
    if(!m_pParentManager)return;

    wxSFShapeCanvas *pCanvas = GetShapeManager()->GetShapeCanvas();

    if(!pCanvas)return;

	if( m_fVisible && m_fActive )
	{
		double dx = 1, dy = 1;
		bool fRefreshAll = false;
		wxRect prevBB;


		if( pCanvas->ContainsStyle(wxSFShapeCanvas::sfsGRID_USE) )
		{
			dx = pCanvas->GetGrid().x;
			dy = pCanvas->GetGrid().y;
		}

		ShapeList lstSelection;
		pCanvas->GetSelectedShapes(lstSelection);
		if((lstSelection.GetCount() > 1) && (lstSelection.IndexOf(this) != wxNOT_FOUND))
		{
		    fRefreshAll = true;
		}

		if(!fRefreshAll)
		{
            GetCompleteBoundingBox(prevBB, bbSELF | bbCONNECTIONS | bbCHILDREN | bbSHADOW);
		}

        if(this->OnKey(key))
        {
            switch(key)
            {
            case WXK_LEFT:
                if(ContainsStyle(sfsPOSITION_CHANGE))this->MoveBy(-dx, 0);
                break;

            case WXK_RIGHT:
                if(ContainsStyle(sfsPOSITION_CHANGE))this->MoveBy(dx, 0);
                break;

            case WXK_UP:
                if(ContainsStyle(sfsPOSITION_CHANGE))this->MoveBy(0, -dy);
                break;

            case WXK_DOWN:
                if(ContainsStyle(sfsPOSITION_CHANGE))this->MoveBy(0, dy);
                break;
            }
        }

        if(!fRefreshAll)
        {
            wxRect currBB;
            GetCompleteBoundingBox(currBB, bbSELF | bbCONNECTIONS | bbCHILDREN | bbSHADOW);

            prevBB.Union(currBB);
            Refresh(prevBB);
        }
        else
            pCanvas->Refresh(false);
	}
}

void wxSFShapeBase::_OnHandle(wxSFShapeHandle& handle)
{
	if( !m_pParentManager )return;

    wxSFShapeBase *pChild;
	wxRect prevBB, currBB;
	
	if( m_pParentItem )
	{
		GetGrandParentShape()->GetCompleteBoundingBox( prevBB );
	}
	else
		this->GetCompleteBoundingBox( prevBB );

    // call appropriate user-defined handler
	this->OnHandle(handle);

    // align children
    SerializableList::compatibility_iterator node = GetFirstChildNode();
    while(node)
    {
        pChild = (wxSFShapeBase*)node->GetData();

        if((pChild->GetVAlign() != valignNONE) || (pChild->GetHAlign() != halignNONE))
        {
            pChild->DoAlignment();
        }
        node = node->GetNext();
    }
    // update shape
    this->Update();
	
	if( m_pParentItem )
	{
		GetGrandParentShape()->GetCompleteBoundingBox( currBB );
	}
	else
		this->GetCompleteBoundingBox( currBB );

    // refresh shape
    Refresh( currBB.Union( prevBB ) );
}
