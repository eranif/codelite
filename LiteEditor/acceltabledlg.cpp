//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : acceltabledlg.cpp              
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
 #include "globals.h"
#include <wx/ffile.h>
#include "newkeyshortcutdlg.h"
#include "acceltabledlg.h"
#include "manager.h"

//-------------------------------------------------------------------------------
//Helper classes for sorting
//-------------------------------------------------------------------------------
struct AccelSorter 
{
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd)
	{
		return rEnd.accel.CmpNoCase(rStart.accel) < 0;
	}
};

struct ActionSorter 
{
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd)
	{
		return rEnd.action.CmpNoCase(rStart.action) < 0;
	}
};

struct ParentSorter 
{
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd)
	{
		return rEnd.parent.CmpNoCase(rStart.parent) < 0;
	}
};
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

AccelTableDlg::AccelTableDlg( wxWindow* parent )
		:
		AccelTableBaseDlg( parent )
{
	//add two columns to the list ctrl
	m_listCtrl1->InsertColumn(0, wxT("ID"));
	m_listCtrl1->InsertColumn(1, wxT("Menu"));
	m_listCtrl1->InsertColumn(2, wxT("Action"));
	m_listCtrl1->InsertColumn(3, wxT("Accelerator"));

	PopulateTable();
	m_listCtrl1->SetFocus();
}

void AccelTableDlg::OnItemActivated( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	
	//build the selected entry
	MenuItemData mid;
	mid.id = GetColumnText(m_listCtrl1, m_selectedItem, 0);
	mid.parent = GetColumnText(m_listCtrl1, m_selectedItem, 1);
	mid.action = GetColumnText(m_listCtrl1, m_selectedItem, 2);
	mid.accel = GetColumnText(m_listCtrl1, m_selectedItem, 3);
	
	
	NewKeyShortcutDlg *dlg = new NewKeyShortcutDlg(this, mid);
	if( dlg->ShowModal() == wxID_OK ) {
		mid.accel = dlg->GetAccel();
	}
	dlg->Destroy();
	
	//update the acceleration table
	SetColumnText(m_listCtrl1, m_selectedItem, 3, mid.accel);
	m_listCtrl1->SetColumnWidth(3, wxLIST_AUTOSIZE);
}

void AccelTableDlg::OnItemSelected( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
}

void AccelTableDlg::OnItemDeselected( wxListEvent& event )
{
	m_selectedItem = wxNOT_FOUND;
}

void AccelTableDlg::PopulateTable()
{
	m_listCtrl1->Freeze();
	m_listCtrl1->DeleteAllItems();

	MenuItemDataMap accelMap;
	ManagerST::Get()->GetAcceleratorMap(accelMap);

	MenuItemDataMap::iterator iter = accelMap.begin();
	std::vector< MenuItemData > itemsVec;
	//convert the map into vector
	for (; iter != accelMap.end(); iter++ ) {
		itemsVec.push_back( iter->second );
	}
	
	//by default sort the items according to the parent 
	std::sort(itemsVec.begin(), itemsVec.end(), ParentSorter());
	
	for (size_t i=0; i< itemsVec.size(); i++){
		
		MenuItemData item = itemsVec.at(i);
		long row = AppendListCtrlRow(m_listCtrl1);
		
		SetColumnText(m_listCtrl1, row, 0, item.id);
		SetColumnText(m_listCtrl1, row, 1, item.parent);
		SetColumnText(m_listCtrl1, row, 2, item.action);
		SetColumnText(m_listCtrl1, row, 3, item.accel);
	}

	m_listCtrl1->SetColumnWidth(0, 0);
	m_listCtrl1->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_listCtrl1->SetColumnWidth(2, wxLIST_AUTOSIZE);
	m_listCtrl1->SetColumnWidth(3, wxLIST_AUTOSIZE);

	m_listCtrl1->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	m_listCtrl1->SetItemState(0, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
	
	m_listCtrl1->Thaw();
}

void AccelTableDlg::OnColClicked(wxListEvent &event)
{
	wxUnusedVar(event);
}

void AccelTableDlg::OnButtonOk(wxCommandEvent &e)
{
	//export the content of table, and apply the changes
	wxString content;
	size_t count = m_listCtrl1->GetItemCount();
	for(size_t i=0; i<count; i++) {
		content << GetColumnText(m_listCtrl1, i, 0);
		content << wxT("|");
		content << GetColumnText(m_listCtrl1, i, 1);
		content << wxT("|");
		content << GetColumnText(m_listCtrl1, i, 2);
		content << wxT("|");
		content << GetColumnText(m_listCtrl1, i, 3);
		content << wxT("\n");
	}
	
	wxString fileName = ManagerST::Get()->GetStarupDirectory();
	fileName << wxT("/config/accelerators.conf");
	
	wxFFile file;
	if (!file.Open(fileName, wxT("w+b"))) {
		return;
	}

	file.Write(content);
	file.Close();
	
	//apply changes
	ManagerST::Get()->UpdateMenuAccelerators();
	
	EndModal( wxID_OK );
}


