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
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include "globals.h"
#include <wx/ffile.h>
#include "newkeyshortcutdlg.h"
#include "acceltabledlg.h"
#include "manager.h"
#include <wx/imaglist.h>
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
struct AccelRSorter {
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd) {
		return rEnd.accel.CmpNoCase(rStart.accel) > 0;
	}
};

struct ActionRSorter {
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd) {
		return rEnd.action.CmpNoCase(rStart.action) > 0;
	}
};

struct ParentRSorter {
	bool operator()(const MenuItemData &rStart, const MenuItemData &rEnd) {
		return rEnd.parent.CmpNoCase(rStart.parent) > 0;
	}
};
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

AccelTableDlg::AccelTableDlg( wxWindow* parent )
		:
		AccelTableBaseDlg( parent )
{
	wxImageList* imageList= new wxImageList(16, 16);
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("list-control/16/sort_down")));
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("list-control/16/sort_up")));
	
	m_listCtrl1->AssignImageList(imageList, wxIMAGE_LIST_SMALL);

	m_listCtrl1->InsertColumn(LC_Menu, _("Menu"));
	m_listCtrl1->InsertColumn(LC_Action, _("Action"));
	m_listCtrl1->InsertColumn(LC_Accel, _("Accelerator"));

	// By default sort the items according to the parent
	m_SortCol = LC_Menu;
	m_direction = 0;

	MenuItemDataMap accelMap;
	PluginManager::Get()->GetKeyboardManager()->GetAccelerators(accelMap);
	PopulateTable(&accelMap);

	// center the dialog
	Centre();

	m_textCtrlFilter->SetFocus();
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

void AccelTableDlg::PopulateTable(MenuItemDataMap *accelMap)
{
	m_listCtrl1->Freeze();
	m_listCtrl1->DeleteAllItems();
	m_IDarray.Clear();

	// If we have an accelerator map, replace the current one
	if(accelMap) {
		MenuItemDataMap::const_iterator iter = accelMap->begin();
		m_itemsVec.clear();

		// Convert the map into vector
		for (; iter != accelMap->end(); iter++ ) {
			m_itemsVec.push_back( iter->second );
		}
	}

	if (!m_direction) {
	switch(m_SortCol) {
		case LC_Action:		std::sort(m_itemsVec.begin(), m_itemsVec.end(), ActionRSorter()); break;
		case LC_Accel:		std::sort(m_itemsVec.begin(), m_itemsVec.end(), AccelRSorter()); break;
		 default:			std::sort(m_itemsVec.begin(), m_itemsVec.end(), ParentRSorter());
		}
	} else {
	switch(m_SortCol) {
		case LC_Action:		std::sort(m_itemsVec.begin(), m_itemsVec.end(), ActionSorter()); break;
		case LC_Accel:		std::sort(m_itemsVec.begin(), m_itemsVec.end(), AccelSorter()); break;
		 default:			std::sort(m_itemsVec.begin(), m_itemsVec.end(), ParentSorter());
		}
	}

	DisplayCorrectColumnImage();

	wxString filterString = m_textCtrlFilter->GetValue();
	filterString.MakeLower().Trim().Trim(false);

	for (size_t i=0; i< m_itemsVec.size(); i++) {
		MenuItemData item = m_itemsVec.at(i);
		wxString action = item.action;

		action.MakeLower();
		if(filterString.IsEmpty() || action.Find(filterString) != wxNOT_FOUND) {
			long row = AppendListCtrlRow(m_listCtrl1);

			// We got a filter and there is a match, show this item
			SetColumnText(m_listCtrl1, row, LC_Menu, item.parent);
			SetColumnText(m_listCtrl1, row, LC_Action, item.action);
			SetColumnText(m_listCtrl1, row, LC_Accel, item.accel);

			// Store the id, which we'll need to identify the item later
			m_IDarray.Add(item.id);

		}
		// else hide this entry

	}

	m_listCtrl1->SetColumnWidth(LC_Menu, wxLIST_AUTOSIZE);
	m_listCtrl1->SetColumnWidth(LC_Action, wxLIST_AUTOSIZE);
	m_listCtrl1->SetColumnWidth(LC_Accel, wxLIST_AUTOSIZE);

	if (m_listCtrl1->GetItemCount()) { // Protect against all the items being filtered away
	m_listCtrl1->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	m_listCtrl1->SetItemState(0, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
	}

	m_listCtrl1->Thaw();
}

void AccelTableDlg::OnColClicked(wxListEvent &event)
{
	ListctrlCols col = (ListctrlCols)event.GetColumn();
	if (col >= LC_End) { 
		// Protect against a click in the spare space after the last real column
		return;
	}
	
	if (col == m_SortCol) {
		// Subsequent click on same col, so toggle direction
		m_direction = !m_direction;
	} else {
		// Different col, so direction is down again
		m_direction = 0;
	}
	
	m_SortCol = col;
	PopulateTable(NULL);
}

void AccelTableDlg::OnButtonOk(wxCommandEvent &e)
{
	//export the content of table, and apply the changes
	wxString content;
	for (size_t i=0; i<m_itemsVec.size(); i++) {
		MenuItemData mid = m_itemsVec.at(i);
		content << mid.id;
		content << wxT("|");
		content << mid.parent;
		content << wxT("|");
		content << mid.action;
		content << wxT("|");
		content << mid.accel;
		content << wxT("\n");
	}

	wxString fileName;
	fileName = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config/accelerators.conf");

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

	PopulateTable(&accelMap);
}

void AccelTableDlg::OnEditButton(wxCommandEvent& e)
{
	if (m_selectedItem != wxNOT_FOUND) {
		DoItemActivated();
	}
}

void AccelTableDlg::DoItemActivated()
{
	wxCHECK_RET(((size_t)m_selectedItem) < m_IDarray.GetCount(), wxT("listctrl selection not in IDarray"));
	
	//build the selected entry
	MenuItemData mid;
	mid.id     = m_IDarray.Item(m_selectedItem);
	mid.parent = GetColumnText(m_listCtrl1, m_selectedItem, LC_Menu);
	mid.action = GetColumnText(m_listCtrl1, m_selectedItem, LC_Action);
	mid.accel  = GetColumnText(m_listCtrl1, m_selectedItem, LC_Accel);

	if (PluginManager::Get()->GetKeyboardManager()->PopupNewKeyboardShortcutDlg(this, mid) == wxID_OK) {
		// search the list for similar accelerator
		for (size_t i=0; i<m_itemsVec.size(); i++) {
			if (Compare(m_itemsVec.at(i).accel, mid.accel) && m_selectedItem != static_cast<int>(i) && mid.accel.IsEmpty() == false) {
				wxString action = m_itemsVec.at(i).action;
				wxMessageBox(wxString::Format(_("'%s' is already assigned to: '%s'"), mid.accel.c_str(), action.c_str()), _("CodeLite"), wxOK|wxCENTER|wxICON_WARNING, this);
				return;
			}
		}

		// Update the acceleration table
		SetColumnText(m_listCtrl1, m_selectedItem, LC_Accel, mid.accel);
		m_listCtrl1->SetColumnWidth(LC_Accel, wxLIST_AUTOSIZE);

		// Update the vector as well
		for (size_t i=0; i<m_itemsVec.size(); i++) {
			if (m_itemsVec.at(i).id == mid.id) {
				m_itemsVec.at(i).accel = mid.accel;
				break;
			}
		}
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

void AccelTableDlg::OnText(wxCommandEvent& event)
{
	wxUnusedVar(event);
	PopulateTable(NULL);
}

void AccelTableDlg::DisplayCorrectColumnImage() const
{
    // Set an 'up' or 'down' image to the clicked column
	// Unset any existing image


	for (int n = LC_Start; n < LC_End; ++n) {
		int image = ( (n==m_SortCol) ? m_direction : wxNOT_FOUND );

		wxListItem item;
		item.SetMask(wxLIST_MASK_IMAGE);
		item.SetImage(image);
		m_listCtrl1->SetColumn(n, item);
	}
}
