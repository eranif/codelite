//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CreateForeignKey.cpp
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

#include "CreateForeignKey.h"

CreateForeignKey::CreateForeignKey(wxWindow* parent, ErdTable* pSourceTable, ErdTable* pDestTable, const wxString& srcColName, const wxString& dstColName):_CreateForeignKey(parent)
{
	m_pSrcTable = pSourceTable;
	m_pDstTable = pDestTable;
	m_srcColName = srcColName;
	m_dstColName = dstColName;
	
	m_cmbDstCol->SetStringSelection(dstColName);
	m_cmbSrcCol->SetStringSelection(srcColName);
	m_txSrcTable->SetValue(pSourceTable->GetTable()->GetName());
	m_txDstTable->SetValue(pDestTable->GetTable()->GetName());	
	SerializableList::compatibility_iterator node = pSourceTable->GetTable()->GetFirstChildNode();
	while( node ) {
		if( node->GetData()->IsKindOf( CLASSINFO(Column)) )  m_cmbSrcCol->AppendString(wxString::Format(wxT("%s"),((Column*) node->GetData())->GetName().c_str()));
		node = node->GetNext();
	}
	node = pDestTable->GetTable()->GetFirstChildNode();
	while( node ) {
		if( node->GetData()->IsKindOf( CLASSINFO(Column)) )  m_cmbDstCol->AppendString(wxString::Format(wxT("%s"),((Column*) node->GetData())->GetName().c_str()));
		node = node->GetNext();
	}
}

CreateForeignKey::~CreateForeignKey()
{
}

void CreateForeignKey::OnCancelClick(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}

void CreateForeignKey::OnOKClick(wxCommandEvent& event)
{
	wxString srcColName;
	wxString srcLocColName;
	wxString dstColName;
	wxString dstLocColName;
	
	if (m_radioRelation->GetSelection() == 0){
		Table* pTable = m_pSrcTable->GetTable();		
		Constraint* pConstr = new Constraint();
		pConstr->SetName(wxString::Format(wxT("FK_%s_%s"), pTable->GetName().c_str(),m_pDstTable->GetTable()->GetName().c_str()));
		pConstr->SetLocalColumn(m_cmbSrcCol->GetStringSelection());
		pConstr->SetRefCol(m_cmbDstCol->GetStringSelection());
		pConstr->SetRefTable(m_txDstTable->GetValue());
		pConstr->SetType(Constraint::foreignKey);
		pConstr->SetOnDelete((Constraint::constraintAction) m_radioOnDelete->GetSelection());
		pConstr->SetOnUpdate((Constraint::constraintAction) m_radioOnUpdate->GetSelection());
		pTable->AddChild(pConstr);
		m_pSrcTable->UpdateColumns();
		m_pSrcTable->Refresh();
	}	

	if (m_radioRelation->GetSelection() == 1){
		// Get old tables   -------------------------------------------------------------
		Table* pSrcTable = m_pSrcTable->GetTable();	
		Table* pDstTable = m_pDstTable->GetTable();	
		// Create new table -------------------------------------------------------------
		//wxString sr1 = pSrcTable->getName();
 		//wxString sr2 = pDstTable->getName();
		//DBETable* newTab = new DBETable();//new DBETable(pSrcTable->GetDbAdapter(),wxString::Format(wxT("FKT_%s_%s"),pSrcTable->getName().c_str(),pDstTable->getName().c_str() ), wxT(""),0);
		Table* newTab = new Table();
		newTab->SetName(wxString::Format(wxT("FKT_%s_%s"),pSrcTable->GetName().c_str(),pDstTable->GetName().c_str() ));
		// Copy selected columns --------------------------------------------------------
		SerializableList::compatibility_iterator node = pSrcTable->GetFirstChildNode();
		while( node ) {
			if( node->GetData()->IsKindOf( CLASSINFO(Column)) ){
				Column* col = wxDynamicCast(node->GetData(), Column);
				if (col->GetName() == m_cmbSrcCol->GetStringSelection()) {
					Column* colNew = (Column*) col->Clone();
					srcColName = col->GetName();
					srcLocColName = wxString::Format(wxT("%s_%s"), pSrcTable->GetName().c_str(), col->GetName().c_str());
					colNew->SetName(srcLocColName);
					newTab->AddChild(colNew);	
					}			
				} 
			node = node->GetNext();
		}
		
		node = pDstTable->GetFirstChildNode();
		while( node ) {
			if( node->GetData()->IsKindOf( CLASSINFO(Column)) ){
				Column* col = wxDynamicCast(node->GetData(), Column);
				if (col->GetName() == m_cmbDstCol->GetStringSelection()) {
					Column* colNew = (Column*) col->Clone();
					dstColName = col->GetName();
					dstLocColName = wxString::Format(wxT("%s_%s"), pDstTable->GetName().c_str(), col->GetName().c_str());
					colNew->SetName(dstLocColName);
					newTab->AddChild(colNew);	
					}			
				} 
			node = node->GetNext();
		}
		// create constraints ------------------------------------------------------------
		Constraint* pCSrc = new Constraint();
		pCSrc->SetName(wxString::Format(wxT("FK_%s_%s"),newTab->GetName().c_str(), pSrcTable->GetName().c_str()));
		pCSrc->SetLocalColumn(srcLocColName);
		pCSrc->SetRefTable(pSrcTable->GetName());
		pCSrc->SetRefCol(srcColName);
		pCSrc->SetType(Constraint::foreignKey);
		pCSrc->SetOnDelete((Constraint::constraintAction) m_radioOnDelete->GetSelection());
		pCSrc->SetOnUpdate((Constraint::constraintAction) m_radioOnUpdate->GetSelection());
		newTab->AddChild(pCSrc);		
		
		Constraint* pCDest = new Constraint();
		pCDest->SetName(wxString::Format(wxT("FK_%s_%s"),newTab->GetName().c_str(), pDstTable->GetName().c_str()));
		pCDest->SetLocalColumn(dstLocColName);
		pCDest->SetRefTable(pDstTable->GetName());
		pCDest->SetRefCol(dstColName);
		pCDest->SetType(Constraint::foreignKey);
		pCDest->SetOnDelete((Constraint::constraintAction) m_radioOnDelete->GetSelection());
		pCDest->SetOnUpdate((Constraint::constraintAction) m_radioOnUpdate->GetSelection());
		newTab->AddChild(pCDest);			
		
		
		int x = (m_pSrcTable->GetAbsolutePosition().x + m_pDstTable->GetAbsolutePosition().x) / 2;
		int y = (m_pSrcTable->GetAbsolutePosition().y + m_pDstTable->GetAbsolutePosition().y) / 2;
		
		ErdTable* newErdTable = (ErdTable*) m_pSrcTable->GetShapeManager()->AddShape(new ErdTable(), NULL, wxPoint(x,y), true);
		newErdTable->SetUserData(newTab);
		newErdTable->UpdateColumns();
		newErdTable->Refresh();
	}
		
	EndModal(wxID_OK);
}

void CreateForeignKey::OnOKUI(wxUpdateUIEvent& event)
{
	event.Enable(false);
	if ((!m_cmbSrcCol->GetStringSelection().empty())&&(!m_cmbDstCol->GetStringSelection().empty())) event.Enable(true);
}
