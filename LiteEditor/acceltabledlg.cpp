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
#include "acceltabledlg.h"

#include "fileutils.h"
#include "globals.h"
#include "manager.h"
#include "newkeyshortcutdlg.h"
#include "pluginmanager.h"
#include "windowattrmanager.h"

#include <algorithm>
#include <wx/ffile.h>
#include <wx/imaglist.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

AccelTableDlg::AccelTableDlg(wxWindow* parent)
    : AccelTableBaseDlg(parent)
{
    wxImageList* imageList = new wxImageList(16, 16);
    imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap("list-control/16/sort"));
    imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap("list-control/16/sort"));

    clKeyboardManager::Get()->GetAllAccelerators(m_accelMap);
    PopulateTable("");

    // center the dialog
    CentreOnParent();

    m_textCtrlFilter->SetFocus();

    SetName("AccelTableDlg");
    WindowAttrManager::Load(this);
}

void AccelTableDlg::PopulateTable(const wxString& filter)
{
    m_dvListCtrl->DeleteAllItems();

    MenuItemDataMap_t filteredMap;
    if(filter.IsEmpty()) {
        filteredMap = m_accelMap;
    } else {
        for(MenuItemDataMap_t::iterator iter = m_accelMap.begin(); iter != m_accelMap.end(); ++iter) {
            if(!IsMatchesFilter(filter, iter->second)) {
                continue;
            }
            filteredMap.insert(std::make_pair(iter->first, iter->second));
        }
    }

    if(filteredMap.empty()) {
        return;
    }

    // Add core entries
    std::vector<std::tuple<wxString, clKeyboardShortcut, AccelItemData*>> V;
    for(MenuItemDataMap_t::const_iterator iter = filteredMap.begin(); iter != filteredMap.end(); ++iter) {
        const MenuItemData& mid = iter->second;

        wxString desc = mid.parentMenu;
        if(!desc.IsEmpty()) {
            desc << " | ";
        }
        desc << mid.action;
        V.push_back(std::make_tuple(desc, mid.accel, new AccelItemData(mid)));
    }

    // Sort the items in the list, based on the description
    std::sort(V.begin(), V.end());
    for(const std::tuple<wxString, clKeyboardShortcut, AccelItemData*>& entry : V) {
        const wxString& desc = std::get<0>(entry);
        const clKeyboardShortcut& shortcut = std::get<1>(entry);
        AccelItemData* itemData = std::get<2>(entry);

        wxVector<wxVariant> cols;
        cols.push_back(shortcut.ToString());
        cols.push_back(desc);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)itemData);
    }
}

void AccelTableDlg::OnButtonOk(wxCommandEvent& e)
{
    // apply changes
    clKeyboardManager::Get()->SetAccelerators(m_accelMap);
    clKeyboardManager::Get()->Update();
    EndModal(wxID_OK);
}

void AccelTableDlg::OnButtonDefaults(wxCommandEvent& e)
{
    // re-load the default key bindings settings
    m_accelMap.clear();
    m_textCtrlFilter->ChangeValue(""); // Clear the filter
    clKeyboardManager::Get()->RestoreDefaults();
    clKeyboardManager::Get()->GetAllAccelerators(m_accelMap);
    CallAfter(&AccelTableDlg::PopulateTable, wxString());
}

void AccelTableDlg::OnEditButton(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoItemActivated();
}

void AccelTableDlg::DoItemActivated()
{
    wxDataViewItem sel = m_dvListCtrl->GetSelection();
    CHECK_ITEM_RET(sel);

    AccelItemData* itemData = DoGetItemData(sel);
    if(!itemData) {
        return;
    }

    // build the selected entry
    MenuItemData mid = itemData->m_menuItemData;
    if(clKeyboardManager::Get()->PopupNewKeyboardShortcutDlg(this, mid) == wxID_OK) {
        // search the list for similar accelerator
        MenuItemData who;
        if(HasAccelerator(mid.accel, who)) {
            if(who.action == mid.action) {
                return;
            }
            if(wxMessageBox(wxString::Format(_("'%s' is already assigned to: '%s'\nWould you like to replace it?"),
                                             mid.accel.ToString(), who.action),
                            _("CodeLite"), wxYES_NO | wxCENTER | wxICON_QUESTION, this) != wxYES) {
                return;
            }

            // Remove the old entry
            wxDataViewItem oldItem = FindAccel(mid);
            if(oldItem.IsOk()) {
                AccelItemData* cd = DoGetItemData(oldItem);
                if(cd) {
                    cd->m_menuItemData.accel.Clear();
                    int row = m_dvListCtrl->ItemToRow(oldItem);
                    m_dvListCtrl->SetValue(wxString(), row, 0);
                }
            }

            MenuItemDataMap_t::iterator iter = m_accelMap.find(who.resourceID);
            if(iter != m_accelMap.end()) {
                iter->second.accel.Clear(); // Clear the accelerator
            }
        }

        // Update the client data
        itemData->m_menuItemData = mid;

        // Update the UI
        int row = m_dvListCtrl->ItemToRow(sel);
        if(row == wxNOT_FOUND) {
            return;
        }
        m_dvListCtrl->SetValue(mid.accel.ToString(), row, 0);

        // and update the map
        MenuItemDataMap_t::iterator iter = m_accelMap.find(itemData->m_menuItemData.resourceID);
        if(iter != m_accelMap.end()) {
            iter->second.accel = itemData->m_menuItemData.accel;
        }
    }
}

void AccelTableDlg::OnText(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CallAfter(&AccelTableDlg::PopulateTable, m_textCtrlFilter->GetValue());
}

AccelTableDlg::~AccelTableDlg() {}

void AccelTableDlg::OnDVItemActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    DoItemActivated();
}

void AccelTableDlg::OnEditUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetSelectedItemsCount()); }

bool AccelTableDlg::IsMatchesFilter(const wxString& filter, const MenuItemData& item)
{
    wxString lcFilter = filter.Lower();
    lcFilter.Trim().Trim(false);
    if(lcFilter.IsEmpty()) {
        return true;
    }

    wxString parentMenu = item.parentMenu;

    wxString haystack;
    haystack << parentMenu << " " << item.accel.ToString() << " " << item.action;
    return FileUtils::FuzzyMatch(filter, haystack);
}

bool AccelTableDlg::HasAccelerator(const clKeyboardShortcut& accel, MenuItemData& who)
{
    if(!accel.IsOk()) {
        return false;
    }
    for(MenuItemDataMap_t::iterator iter = m_accelMap.begin(); iter != m_accelMap.end(); ++iter) {
        if(iter->second.accel == accel) {
            who = iter->second;
            return true;
        }
    }
    return false;
}

AccelItemData* AccelTableDlg::DoGetItemData(const wxDataViewItem& item)
{
    return reinterpret_cast<AccelItemData*>(m_dvListCtrl->GetItemData(item));
}

wxDataViewItem AccelTableDlg::FindAccel(const MenuItemData& mid)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->RowToItem(i);
        if(!item.IsOk()) {
            continue;
        }

        AccelItemData* cd = DoGetItemData(item);
        if(cd && cd->m_menuItemData.accel == mid.accel) {
            return item;
        }
    }
    return wxDataViewItem();
}
