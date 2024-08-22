//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ErdView.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "ErdView.h"

#include <wx/recguard.h>
#include <wx/wxsf/CommonFcn.h>

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(ErdView,wxSFRoundRectShape);

ErdView::ErdView():wxSFRoundRectShape()
{
	Initialize();
}

ErdView::ErdView(View* pView):wxSFRoundRectShape()
{
	SetUserData(pView);
	Initialize();
}

ErdView::ErdView(const ErdView& obj):wxSFRoundRectShape(obj)
{
	m_pLabel = (wxSFTextShape*) obj.m_pLabel->Clone();
	if (m_pLabel){
		m_pLabel->SetStyle(sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION);
		SF_ADD_COMPONENT( m_pLabel, wxT("title") );	
		}
}

ErdView::~ErdView()
{
}

void ErdView::DrawHover(wxDC& dc)
{
	wxSFRoundRectShape::DrawHover(dc);
	DrawDetails(dc);
}
void ErdView::DrawNormal(wxDC& dc)
{
	wxSFRoundRectShape::DrawNormal(dc);
	DrawDetails(dc);
}

void ErdView::DrawHighlighted(wxDC& dc)
{
	wxSFRoundRectShape::DrawHighlighted(dc);
	DrawDetails(dc);
}

void ErdView::DrawDetails(wxDC& dc)
{
	dc.SetPen( *wxWHITE_PEN );
	dc.SetBrush( *wxWHITE_BRUSH );
	
	dc.DrawRectangle( Conv2Point(GetAbsolutePosition() + wxRealPoint(1, m_nRadius)),
					  Conv2Size(m_nRectSize - wxRealPoint(2, 2*m_nRadius - 4)) );
}

void ErdView::Initialize()
{
	AcceptConnection(wxT("All"));
	AcceptTrgNeighbour(wxT("All"));
	AcceptSrcNeighbour(wxT("All"));
	
	AddStyle( sfsLOCK_CHILDREN );
	AddStyle( sfsSHOW_SHADOW );
	
	SetBorder( wxPen( wxColour(220, 219, 140), 1, wxPENSTYLE_SOLID ) );
	SetFill( wxBrush( wxColour(255, 250, 200) ) );
	
	SetRadius(15);
	
	m_pLabel = new wxSFTextShape();
	if (m_pLabel) {
		m_pLabel->SetVAlign( wxSFShapeBase::valignTOP );
        m_pLabel->SetHAlign( wxSFShapeBase::halignCENTER );
        m_pLabel->SetVBorder( 1 );
		m_pLabel->SetHBorder( 5 );
		m_pLabel->GetFont().SetPointSize( 8 );
		m_pLabel->GetFont().SetWeight(wxFONTWEIGHT_BOLD);

		m_pLabel->SetText(wxT("DBETable name"));
		m_pLabel->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION);
		
        SF_ADD_COMPONENT( m_pLabel, wxT("title") );		
	}
	
	m_pGrid = new wxSFFlexGridShape();
	if (m_pGrid){
		// set grid
		m_pGrid->SetRelativePosition( 0, 17 );
		m_pGrid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION  );
		m_pGrid->SetDimensions( 1, 1 );
		
		m_pGrid->SetFill( *wxTRANSPARENT_BRUSH );
		m_pGrid->SetBorder( *wxTRANSPARENT_PEN );
		
		m_pGrid->SetHAlign( wxSFShapeBase::halignLEFT );
		m_pGrid->SetCellSpace( 1 );
		m_pGrid->SetVBorder( 13 );
		m_pGrid->SetHBorder( 10 );
		m_pGrid->AcceptChild( wxT("wxSFTextShape") );
		m_pGrid->Activate( false );
		
        SF_ADD_COMPONENT( m_pGrid, wxT("SelectGrid") );		
	}
}

void ErdView::UpdateView()
{
	m_pGrid->RemoveChildren();
	m_pGrid->ClearGrid();
	m_pGrid->SetDimensions( 1, 1 );	

	SetRectSize(GetRectSize().x, 0);

	View* pView = wxDynamicCast(GetUserData(), View);
	if (pView){
		m_pLabel->SetText(pView->GetName());

		wxSFTextShape* m_pCol = new wxSFTextShape();
		if (m_pCol){	
			m_pCol->SetStyle( sfsALWAYS_INSIDE | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
			//m_pCol->SetId(1);
			if (m_pGrid->AppendToGrid(m_pCol)){
				m_pCol->GetFont().SetPointSize(8);
				if (pView->GetSelect().length()>100){					
					m_pCol->SetText(wxString::Format(wxT("%s  ...\n\t... %s"), pView->GetSelect().Mid(0,50).c_str(), pView->GetSelect().Right(50).c_str()));
					}else m_pCol->SetText( pView->GetSelect() );			
				m_pCol->SetHAlign(wxSFShapeBase::halignCENTER);
				m_pCol->SetVAlign(wxSFShapeBase::valignMIDDLE);
				m_pCol->SetVBorder(0);
				m_pCol->SetHBorder(0);
				m_pCol->Activate(false);
				
			}else{			
				delete m_pCol;
				m_pCol = NULL;
			}
		}	
	}
		
	m_pGrid->Update();
	Update();
}
