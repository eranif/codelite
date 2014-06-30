//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : ErdForeignKey.cpp
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

#include "ErdForeignKey.h"

XS_IMPLEMENT_CLONABLE_CLASS(ErdForeignKey,wxSFRoundOrthoLineShape);

ErdForeignKey::ErdForeignKey():wxSFRoundOrthoLineShape()
{
	m_pConstraint = NULL;
	
	EnableSerialization( false );
}

ErdForeignKey::ErdForeignKey(Constraint* pConstraint):wxSFRoundOrthoLineShape()
{
	m_pConstraint = pConstraint;
	wxSFTextShape* pLabel = new wxSFTextShape();
	
	if( pLabel )
	{
		pLabel->GetFont().SetPointSize(8);
		pLabel->GetFont().SetWeight(wxFONTWEIGHT_BOLD);

		pLabel->SetText(pConstraint->GetName());
		pLabel->SetVAlign(valignMIDDLE);
		pLabel->SetHAlign(halignCENTER);
		pLabel->SetFill(*wxTRANSPARENT_BRUSH);
		pLabel->SetStyle(sfsLOCK_CHILDREN | sfsPARENT_CHANGE);
		pLabel->RemoveStyle(sfsPARENT_CHANGE);
		pLabel->RemoveStyle(sfsSHOW_HANDLES);
		
		AddChild(pLabel);
	}

	SetTrgArrow(CLASSINFO(OneArrow));
	SetSrcArrow(CLASSINFO(NArrow));

	SetDockPoint(sfdvLINESHAPE_DOCKPOINT_CENTER);
	
	EnableSerialization( false );
}

ErdForeignKey::ErdForeignKey(const ErdForeignKey& obj):wxSFRoundOrthoLineShape(obj)
{
	m_pConstraint = obj.m_pConstraint;
}

ErdForeignKey::~ErdForeignKey()
{
}

void ErdForeignKey::CreateHandles()
{
	wxSFRoundOrthoLineShape::CreateHandles();
	
	RemoveHandle( wxSFShapeHandle::hndLINESTART );
	RemoveHandle( wxSFShapeHandle::hndLINEEND );
}
