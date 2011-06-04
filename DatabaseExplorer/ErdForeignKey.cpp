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
