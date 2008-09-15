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
#include "pluginmanager.h"
#include <wx/tokenzr.h>
#include "globals.h"
#include <wx/ffile.h>
#include "newkeyshortcutdlg.h"
#include "acceltabledlg.h"
#include "manager.h"
#include <algorithm>

//-------------------------------------------------------------------------------
//Helper classes for sorting
//-------------------------------------------------------------------------------
struct AccelSorter {
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd) {
		return rEnd.accel.CmpNoCase(rStart.accel) < 0;
	}
};

struct ActionSorter {
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd) {
		return rEnd.action.CmpNoCase(rStart.action) < 0;
	}
};

struct ParentSorter {
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd) {
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

	MenuItemDataMap accelMap;
	PluginManager::Get()->GetKeyboardManager()->GetAccelerators(accelMap);
	PopulateTable(accelMap);

	// center the dialog
	Centre();

	m_listCtrl1->SetFocus();
}

void AccelTableDlg::OnItemActivated( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	DoItemActivated();
}

void AccelTableDlg::OnItemSelected( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
}

void AccelTableDlg::PopulateTable(const MenuItemDataMap &accelMap)
{
	m_listCtrl1->Freeze();
	m_listCtrl1->DeleteAllItems();

	MenuItemDataMap::const_iterator iter = accelMap.begin();
	std::vector< MenuItemData > itemsVec;
	//convert the map into vector
	for (; iter != accelMap.end(); iter++ ) {
		itemsVec.push_back( iter->second );
	}

	//by default sort the items according to the parent
	std::sort(itemsVec.begin(), itemsVec.end(), ParentSorter());

	for (size_t i=0; i< itemsVec.size(); i++) {

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
	for (size_t i=0; i<count; i++) {
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

void AccelTableDlg::OnButtonDefaults(wxCommandEvent& e)
{
	// re-load the default key bindings settings
	MenuItemDataMap accelMap;
	ManagerST::Get()->GetDefaultAcceleratorMap(accelMap);

	PopulateTable(accelMap);
}

void AccelTableDlg::OnEditButton(wxCommandEvent& e)
{
	if (m_selectedItem != wxNOT_FOUND) {
		DoItemActivated();
	}
}

void AccelTableDlg::DoItemActivated()
{
	//build the selected entry
	MenuItemData mid;
	mid.id = GetColumnText(m_listCtrl1, m_selectedItem, 0);
	mid.parent = GetColumnText(m_listCtrl1, m_selectedItem, 1);
	mid.action = GetColumnText(m_listCtrl1, m_selectedItem, 2);
	mid.accel = GetColumnText(m_listCtrl1, m_selectedItem, 3);


	if (PluginManager::Get()->GetKeyboardManager()->PopupNewKeyboardShortcutDlg(this, mid) == wxID_OK) {
		// search the list for similar accelerator
		int count = m_listCtrl1->GetItemCount();
		for (int i=0; i<count; i++) {
			if (Compare(GetColumnText(m_listCtrl1, i, 3), mid.accel) && m_selectedItem != i && mid.accel.IsEmpty() == false) {
				wxString action = GetColumnText(m_listCtrl1, i, 2);
				wxMessageBox(wxString::Format(wxT("'%s' is already assigned to: '%s'"), mid.accel.c_str(), action.c_str()), wxT("CodeLite"), wxOK|wxCENTER|wxICON_WARNING, this);
				return;
			}
		}

		//update the acceleration table
		SetColumnText(m_listCtrl1, m_selectedItem, 3, mid.accel);
		m_listCtrl1->SetColumnWidth(3, wxLIST_AUTOSIZE);
	}
}

bool AccelTableDlg::Compare(const wxString& accel1, const wxString& accel2)
{
	wxArrayString accel1Tokens = wxStringTokenize(accel1, wxT("-"));
	wxArrayString accel2Tokens = wxStringTokenize(accel2, wxT("-"));

	if (accel1Tokens.GetCount() != accel2Tokens.GetCount()) {
		return false;
	}

	for (size_t i=0; i<accel1Tokens.GetCount(); i++) {
		if (accel2Tokens.Index(accel1Tokens.Item(i), false) == wxNOT_FOUND) {
			return false;
		}
	}
	return true;
}
