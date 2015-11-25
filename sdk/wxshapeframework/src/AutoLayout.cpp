/***************************************************************
 * Name:      AutoLayout.cpp
 * Purpose:   Implements class for autolayouting algoriths' provider
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2010-12-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include <wx/wxsf/AutoLayout.h>
#include <wx/wxsf/LineShape.h>
#include <wx/wxsf/ShapeCanvas.h>

#include <math.h>

// constructor and destructor //////////////////////////////////////////////////

wxSFAutoLayout::wxSFAutoLayout()
{
	InitializeAllAlgorithms();
}

wxSFAutoLayout::~wxSFAutoLayout()
{
}

// static stuff ////////////////////////////////////////////////////////////////

LayoutAlgoritmMap wxSFAutoLayout::m_mapAlgorithms;

bool wxSFAutoLayout::RegisterLayoutAlgorithm(const wxString& algname, wxSFLayoutAlgorithm* alg)
{
	if( alg && m_mapAlgorithms.find( algname ) == m_mapAlgorithms.end() )
	{
		m_mapAlgorithms[ algname ] = alg;
		return true;
	}
	else
		return false;
}

void wxSFAutoLayout::CleanUp()
{
	LayoutAlgoritmMap::iterator it = m_mapAlgorithms.begin();
    while( it != m_mapAlgorithms.end() )
    {
		if( it->second ) delete it->second;
        it++;
    }
    m_mapAlgorithms.clear();
}

// member functions ////////////////////////////////////////////////////////////

wxArrayString wxSFAutoLayout::GetRegisteredAlgorithms()
{
	wxArrayString arrAlgorithms;
	
	for( LayoutAlgoritmMap::iterator it = m_mapAlgorithms.begin(); it != m_mapAlgorithms.end(); ++ it )
	{
		arrAlgorithms.Add( it->first );
	}
	
	return arrAlgorithms;
}

void wxSFAutoLayout::InitializeAllAlgorithms()
{
	// register predefined algorithms
	RegisterLayoutAlgorithm( wxT("Circle"), new wxSFLayoutCircle() );
	RegisterLayoutAlgorithm( wxT("Horizontal Tree"), new wxSFLayoutHorizontalTree() );
	RegisterLayoutAlgorithm( wxT("Vertical Tree"), new wxSFLayoutVerticalTree() );
	RegisterLayoutAlgorithm( wxT("Mesh"), new wxSFLayoutMesh() );
}

void wxSFAutoLayout::Layout(ShapeList& shapes, const wxString& algname)
{
	wxSFLayoutAlgorithm *pAlg = m_mapAlgorithms[ algname ];
	if( pAlg )
	{
		ShapeList::compatibility_iterator it = shapes.GetFirst();
		if( it && it->GetData() && it->GetData()->GetShapeManager() ) {
			it->GetData()->GetShapeManager()->SetModified( true );
		}
		pAlg->DoLayout( shapes );
	}
}

void wxSFAutoLayout::Layout(wxSFDiagramManager& manager, const wxString& algname)
{
	wxSFLayoutAlgorithm *pAlg = m_mapAlgorithms[ algname ];
	if( pAlg )
	{
		ShapeList lstShapes;
		manager.GetShapes( CLASSINFO(wxSFShapeBase), lstShapes );
		
		// remove all child shapes
		ShapeList::compatibility_iterator it = lstShapes.GetFirst();
		while( it )
		{
			wxSFShapeBase *pShape = it->GetData();
			if( pShape->GetParentShape() || pShape->IsKindOf(CLASSINFO(wxSFLineShape)) )
			{
				lstShapes.DeleteNode( it );
				it = lstShapes.GetFirst();
			}
			else
				it = it->GetNext();
		}
		
		pAlg->DoLayout( lstShapes );
		
		manager.MoveShapesFromNegatives();
		manager.SetModified( true );
		
		if( manager.GetShapeCanvas() ) UpdateCanvas( manager.GetShapeCanvas() );
	}
}

void wxSFAutoLayout::Layout(wxSFShapeCanvas* canvas, const wxString& algname)
{
	if( canvas )
	{
		Layout( *canvas->GetDiagramManager(), algname );
	}
}

void wxSFAutoLayout::UpdateCanvas(wxSFShapeCanvas* canvas)
{
	canvas->CenterShapes();
	canvas->UpdateVirtualSize();
	canvas->UpdateMultieditSize();
	canvas->Refresh(false);
}

// layout algorithms helpers ///////////////////////////////////////////////////

wxRect wxSFLayoutAlgorithm::GetBoundingBox(const ShapeList& shapes)
{
	wxRect rctBB;
	
	for( ShapeList::const_iterator it = shapes.begin(); it != shapes.end(); ++ it )
	{
		wxSFShapeBase *pShape = *it;
		
		if( it == shapes.begin() ) rctBB = pShape->GetBoundingBox();
		else
			rctBB.Union( pShape->GetBoundingBox() );
	}
	
	return rctBB;
}

wxSize wxSFLayoutAlgorithm::GetShapesExtent(const ShapeList& shapes)
{
	int nTotalWidth = 0, nTotalHeight = 0;
	
	for( ShapeList::const_iterator it = shapes.begin(); it != shapes.end(); ++ it )
	{
		wxRect rctBB = (*it)->GetBoundingBox();
		
		nTotalWidth += rctBB.GetWidth();
		nTotalHeight += rctBB.GetHeight();
	}
	
	return wxSize( nTotalWidth, nTotalHeight );
}

wxRealPoint wxSFLayoutAlgorithm::GetShapesCenter(const ShapeList& shapes)
{
	wxRealPoint nCenter;
	
	for( ShapeList::const_iterator it = shapes.begin(); it != shapes.end(); ++ it )
	{
		nCenter = nCenter + (*it)->GetAbsolutePosition();
	}
	
	nCenter.x /= shapes.GetCount();
	nCenter.y /= shapes.GetCount();
	
	return nCenter;
}

wxRealPoint wxSFLayoutAlgorithm::GetTopLeft(const ShapeList& shapes)
{
	double startx = INT_MAX, starty = INT_MAX;
	
	for( ShapeList::const_iterator it = shapes.begin(); it != shapes.end(); ++ it )
	{
		wxSFShapeBase *pShape = *it;
		
		wxRealPoint nPos = pShape->GetAbsolutePosition();
		if( nPos.x < startx ) startx = nPos.x;
		if( nPos.y < starty ) starty = nPos.y;
	}
	
	return wxRealPoint( startx, starty );
}

// pre-defined layout algorithms ///////////////////////////////////////////////

void wxSFLayoutCircle::DoLayout(ShapeList& shapes)
{
	wxSize sizeShapes = GetShapesExtent( shapes );
	wxRealPoint nCenter = GetShapesCenter( shapes );
	
	double x, y;
	double step = 360.0 / shapes.GetCount();
	double degree = 0;
	double rx = ( sizeShapes.x / 2 ) * m_DistanceRatio;
	double ry = ( sizeShapes.y / 2 ) * m_DistanceRatio;
	
	for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++ it )
	{
		wxSFShapeBase *pShape = *it;
		
		x = nCenter.x + cos( degree * wxSF::PI / 180 ) * rx;
		y = nCenter.y + sin( degree * wxSF::PI / 180 ) * ry;
		degree += step;
		
		pShape->MoveTo( x, y );
	}
}

//------------------------------------------------------------------------------

void wxSFLayoutVerticalTree::DoLayout(ShapeList& shapes)
{
	ShapeList lstConnections;
	ShapeList lstRoots;
	
	wxRealPoint nStart = GetTopLeft( shapes );
	m_nMinX = nStart.x;
	
	// find root items
	for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++ it )
	{
		wxSFShapeBase *pShape = *it;
		
		lstConnections.Clear();
		pShape->GetAssignedConnections( CLASSINFO(wxSFLineShape), wxSFShapeBase::lineENDING, lstConnections );
		
		if( lstConnections.IsEmpty() )
		{
			m_nCurrMaxWidth = 0;
			ProcessNode( pShape, nStart.y );
		}
	}
}

void wxSFLayoutVerticalTree::ProcessNode(wxSFShapeBase* node, double y)
{
	wxASSERT( node );
	
	if( node )
	{
		node->MoveTo( m_nMinX, y );
		
		wxRect rctBB = node->GetBoundingBox();
		if( rctBB.GetWidth() > m_nCurrMaxWidth ) m_nCurrMaxWidth = rctBB.GetWidth();
		
		ShapeList lstNeighbours;
		node->GetNeighbours( lstNeighbours, CLASSINFO(wxSFShapeBase), wxSFShapeBase::lineSTARTING );

		if( lstNeighbours.IsEmpty() )
		{
			m_nMinX += m_nCurrMaxWidth + m_HSpace;
		}
		else
		{
			for( ShapeList::iterator it = lstNeighbours.begin(); it != lstNeighbours.end(); ++it )
			{
				if( ! (*it)->GetParentShape() )	ProcessNode( *it, y + rctBB.GetHeight() + m_VSpace );
			}
		}
	}
}

//------------------------------------------------------------------------------

void wxSFLayoutHorizontalTree::DoLayout(ShapeList& shapes)
{
	ShapeList lstConnections;
	ShapeList lstRoots;
	
	wxRealPoint nStart = GetTopLeft( shapes );
	m_nMinY = nStart.y;
	
	// find root items
	for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++ it )
	{
		wxSFShapeBase *pShape = *it;
		
		lstConnections.Clear();
		pShape->GetAssignedConnections( CLASSINFO(wxSFLineShape), wxSFShapeBase::lineENDING, lstConnections );
		
		if( lstConnections.IsEmpty() )
		{
			m_nCurrMaxHeight = 0;
			ProcessNode( pShape, nStart.x );
		}
	}
}

void wxSFLayoutHorizontalTree::ProcessNode(wxSFShapeBase* node, double x)
{
	wxASSERT( node );
	
	if( node )
	{
		node->MoveTo( x, m_nMinY );
		
		wxRect rctBB = node->GetBoundingBox();
		if( rctBB.GetHeight() > m_nCurrMaxHeight ) m_nCurrMaxHeight = rctBB.GetHeight();
		
		ShapeList lstNeighbours;
		node->GetNeighbours( lstNeighbours, CLASSINFO(wxSFShapeBase), wxSFShapeBase::lineSTARTING );

		if( lstNeighbours.IsEmpty() )
		{
			m_nMinY += m_nCurrMaxHeight + m_VSpace;
		}
		else
		{
			for( ShapeList::iterator it = lstNeighbours.begin(); it != lstNeighbours.end(); ++it )
			{
				if( ! (*it)->GetParentShape() )	ProcessNode( *it, x + rctBB.GetWidth() + m_HSpace );
			}
		}
	}
}

//------------------------------------------------------------------------------

void wxSFLayoutMesh::DoLayout(ShapeList& shapes)
{
	int i = 0, cols = floor( sqrt( shapes.GetCount() ) );
	
	double roffset, coffset, maxh = -m_HSpace;
	roffset = coffset = 0;

	wxRealPoint nStart = GetTopLeft( shapes );

	for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++ it )
	{
		wxSFShapeBase *pShape = *it;
		
		if( i++ % cols == 0 )
		{
			coffset = 0;
			roffset += maxh + m_HSpace;
			maxh = 0;
		}
		
		pShape->MoveTo( nStart.x + coffset, nStart.y + roffset );
		
		wxRect rctBB = pShape->GetBoundingBox();
		coffset += rctBB.GetWidth() + m_VSpace;
		
		if( rctBB.GetHeight() > maxh ) maxh = rctBB.GetHeight();
	}
}
