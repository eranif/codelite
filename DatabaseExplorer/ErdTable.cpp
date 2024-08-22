//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ErdTable.cpp
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

#include "ErdTable.h"

#include "res/gui/key-f.xpm"
#include "res/gui/key-p.xpm"

#include <wx/recguard.h>
#include <wx/wxsf/CommonFcn.h>
#include <wx/wxsf/ShapeCanvas.h>

using namespace wxSFCommonFcn;

#define constOffset MAX_ID

XS_IMPLEMENT_CLONABLE_CLASS(ErdTable,wxSFRoundRectShape);

ErdTable::ErdTable():wxSFRoundRectShape()
{
	Initialize();
}

ErdTable::ErdTable(Table* tab):wxSFRoundRectShape()
{
	SetUserData(tab);
	Initialize();
}

ErdTable::ErdTable(const ErdTable& obj):wxSFRoundRectShape(obj)
{

	m_pLabel = (wxSFTextShape*) obj.m_pLabel->Clone();
	if (m_pLabel)
	{
		m_pLabel->SetStyle(sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION);
		SF_ADD_COMPONENT( m_pLabel, wxT("title") );
	}
}

ErdTable::~ErdTable()
{
}

void ErdTable::Initialize()
{
	SetFill(wxBrush(wxColour(254, 253, 211)));

	AcceptConnection(wxT("All"));
	AcceptTrgNeighbour(wxT("All"));
	AcceptSrcNeighbour(wxT("All"));

	AddStyle( sfsLOCK_CHILDREN );
	AddStyle( sfsSHOW_SHADOW );

	SetBorder( wxPen( wxColour(70, 125, 170), 1, wxPENSTYLE_SOLID ) );
	SetFill( wxBrush( wxColour(210, 225, 245) ) );

	SetRadius(15);

	m_pLabel = new wxSFTextShape();
	m_pGrid = new wxSFFlexGridShape();

	if (m_pLabel && m_pGrid)
	{
		m_pLabel->SetVAlign(wxSFShapeBase::valignTOP);
		m_pLabel->SetHAlign(wxSFShapeBase::halignCENTER);
		m_pLabel->SetVBorder(1);
		m_pLabel->SetHBorder(5);

		m_pLabel->GetFont().SetPointSize( 8 );
		m_pLabel->GetFont().SetWeight(wxFONTWEIGHT_BOLD);

		m_pLabel->SetText(wxT("DBETable name"));
		m_pLabel->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );

		SF_ADD_COMPONENT( m_pLabel, wxT("title") );

		// set grid
		m_pGrid->SetRelativePosition( 0, 17 );
		m_pGrid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION  );
		m_pGrid->SetDimensions( 1, 2 );

		m_pGrid->SetFill( *wxTRANSPARENT_BRUSH );
		m_pGrid->SetBorder( *wxTRANSPARENT_PEN);

		m_pGrid->SetHAlign( wxSFShapeBase::halignLEFT );
		m_pGrid->SetVBorder( 13 );
		m_pGrid->SetHBorder( 5 );
		m_pGrid->AcceptChild( wxT("wxSFTextShape") );
		m_pGrid->AcceptChild( wxT("wxSFBitmapShape") );
		m_pGrid->AcceptChild( wxT("wxSFShapeBase") );
		m_pGrid->Activate( false );

		SF_ADD_COMPONENT( m_pGrid, wxT("main_grid") );
	}
}

void ErdTable::DrawHighlighted(wxDC& dc)
{
	wxSFRoundRectShape::DrawHighlighted(dc);
	DrawDetails(dc);
}

void ErdTable::DrawDetails(wxDC& dc)
{
	dc.SetPen( *wxWHITE_PEN );
	dc.SetBrush( *wxWHITE_BRUSH );

	dc.DrawRectangle( Conv2Point(GetAbsolutePosition() + wxRealPoint(1, m_nRadius)),
	                  Conv2Size(m_nRectSize - wxRealPoint(2, 2*m_nRadius - 2)) );
}

void ErdTable::DrawHover(wxDC& dc)
{
	wxSFRoundRectShape::DrawHover(dc);
	DrawDetails(dc);
}

void ErdTable::DrawNormal(wxDC& dc)
{
	wxSFRoundRectShape::DrawNormal(dc);
	DrawDetails(dc);
}

void ErdTable::UpdateColumns()
{
	ClearGrid();
	ClearConnections();

	SetRectSize(GetRectSize().x, 0);

	ShapeList list;
	if (GetShapeManager())
	{
		GetShapeManager()->GetShapes(CLASSINFO(ErdTable), list);
	}

	int i = 0;
	Constraint::constraintType type;

	Table* tab = (Table*) wxDynamicCast(GetUserData(),Table);
	if (tab)
	{
		m_pLabel->SetText(tab->GetName());
		SerializableList::compatibility_iterator node = tab->GetFirstChildNode();
		while( node )
		{
			Column* pCol = wxDynamicCast(node->GetData(), Column);
			if( pCol )
			{
				type = Constraint::noKey;

				SerializableList::compatibility_iterator cstrNode = tab->GetFirstChildNode();
				while( cstrNode )
				{
					Constraint* constr = wxDynamicCast(cstrNode->GetData(), Constraint);
					if (constr)
					{
						if (constr->GetLocalColumn() == pCol->GetName()) 
						{
							type = constr->GetType();
						}
					}
					cstrNode = cstrNode->GetNext();
				}

				AddColumnShape(pCol->GetName(), i, type);
				i += 2;

			}
			node = node->GetNext();
		}

		node = tab->GetFirstChildNode();
		while( node )
		{
			if( node->GetData()->IsKindOf( CLASSINFO(Constraint)) )
			{
				ErdTable* pSecondTab = NULL;
				Constraint* pConstr = wxDynamicCast(node->GetData(), Constraint);

				ShapeList::compatibility_iterator nodeTab = list.GetFirst();
				while(nodeTab)
				{
					ErdTable* pTab = wxDynamicCast(nodeTab->GetData(), ErdTable);
					if (pTab)
					{
						if (pTab->GetTable()->GetName() == pConstr->GetRefTable()) pSecondTab = pTab;
					}
					nodeTab = nodeTab->GetNext();
				}
				if (pSecondTab)
				{
					GetShapeManager()->CreateConnection(GetId(), pSecondTab->GetId(), new ErdForeignKey(pConstr), sfDONT_SAVE_STATE);
				}
			}
			node = node->GetNext();
		}
	}

	m_pGrid->Update();
	Update();
}

void ErdTable::ClearGrid()
{
	m_pGrid->RemoveChildren();
	// re-initialize grid control
	m_pGrid->ClearGrid();
	m_pGrid->SetDimensions( 1, 2 );
	m_pGrid->SetCellSpace( 2 );
	Refresh();
}

void ErdTable::AddColumnShape(const wxString& colName, int id, Constraint::constraintType type)
{
	if( type != Constraint::noKey )
	{
		// key bitmap
		wxSFBitmapShape* pBitmap = new wxSFBitmapShape();
		if( pBitmap )
		{
			pBitmap->SetId(id + 10000);
			if( m_pGrid->AppendToGrid(pBitmap) )
			{
				if( type == Constraint::primaryKey )
				{
					pBitmap->CreateFromXPM( key_p_xpm );
				}
				else
					pBitmap->CreateFromXPM( key_f_xpm );
				
				SetCommonProps(pBitmap);
			}
			else
				delete pBitmap;
		}
	}
	else
	{
		// spacer
		wxSFShapeBase* pSpacer = new wxSFShapeBase();
		if( pSpacer )
		{
			pSpacer->SetId(id+10000);
			if( m_pGrid->AppendToGrid(pSpacer) )
			{
				SetCommonProps(pSpacer);
			}
			else
				delete pSpacer;
		}
	}
	
	// label
	wxSFTextShape* pCol = new wxSFTextShape();
	if (pCol)
	{
		pCol->SetId(id +10000+ 1);
		if ( m_pGrid->AppendToGrid(pCol) )
		{
			SetCommonProps(pCol);
			pCol->GetFont().SetPointSize(8);
			pCol->SetText( colName );
		}
		else
			delete pCol;
	}
}

void ErdTable::SetCommonProps(wxSFShapeBase* shape)
{
	shape->EnableSerialization(false);
	shape->SetStyle(sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION);
	shape->SetHAlign(wxSFShapeBase::halignLEFT);
	shape->SetVAlign(wxSFShapeBase::valignMIDDLE);
	shape->SetVBorder(0);
	shape->SetHBorder(0);
	shape->Activate(false);
}

void ErdTable::AddColumn(const wxString& colName, IDbType* type)
{
	Table* tab = (Table*) wxDynamicCast(GetUserData(),Table);
	tab->AddColumn(new Column(colName,_("New table"),type));
}

void ErdTable::ClearConnections()
{
	ShapeList lstShapes;
	GetShapeManager()->GetAssignedConnections(this,CLASSINFO(ErdForeignKey),lineSTARTING ,lstShapes);

	// remove all child shapes
	ShapeList::compatibility_iterator node = lstShapes.GetFirst();
	while( node )
	{
		ErdForeignKey* key = wxDynamicCast(node->GetData(),ErdForeignKey);
		if (key)
		{
			GetShapeManager()->RemoveShape(key);
		}
		node = node->GetNext();
	}
}
