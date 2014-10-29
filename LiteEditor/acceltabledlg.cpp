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
#include <wx/imaglist.h>
#include "windowattrmanager.h"
#include <wx/tokenzr.h>

struct AccelItemData : public wxClientData {
    MenuItemData menuItemData;
    bool isPluginAccel;
    AccelItemData()
        : isPluginAccel(false)
    {
    }
    AccelItemData(const MenuItemData& data, bool fromPlugin = false)
        : menuItemData(data)
        , isPluginAccel(fromPlugin)
    {
    }
};

//-------------------------------------------------------------------------------
// Helper classes for sorting
//-------------------------------------------------------------------------------
struct AccelSorter {
    bool operator()(const MenuItemData& rStart, const MenuItemData& rEnd)
    {
        return rEnd.accel.CmpNoCase(rStart.accel) < 0;
    }
};

struct ActionSorter {
    bool operator()(const MenuItemData& rStart, const MenuItemData& rEnd)
    {
        return rEnd.action.CmpNoCase(rStart.action) < 0;
    }
};

struct AccelRSorter {
    bool operator()(const MenuItemData& rStart, const MenuItemData& rEnd)
    {
        return rEnd.accel.CmpNoCase(rStart.accel) > 0;
    }
};

struct ActionRSorter {
    bool operator()(const MenuItemData& rStart, const MenuItemData& rEnd)
    {
        return rEnd.action.CmpNoCase(rStart.action) > 0;
    }
};

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

AccelTableDlg::AccelTableDlg(wxWindow* parent)
    : AccelTableBaseDlg(parent)
{
    wxImageList* imageList = new wxImageList(16, 16);
    imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("list-control/16/sort_down")));
    imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("list-control/16/sort_up")));

    clKeyboardManager::Get()->GetAllAccelerators(m_accelMap);
    PopulateTable();

    // center the dialog
    Centre();

    m_textCtrlFilter->SetFocus();

    WindowAttrManager::Load(this, "AccelTableDlg", NULL);
}

void AccelTableDlg::PopulateTable(const wxString& filter)
{
    m_dataviewModel->Clear();

    // Add core entries
    for(MenuItemDataMap_t::iterator iter = m_accelMap.begin(); iter != m_accelMap.end(); ++iter) {
        if(IsMatchesFilter(filter, iter->second)) {
            wxVector<wxVariant> cols;
            wxString actionStr;
            if(!iter->second.parentMenu.IsEmpty()) {
                actionStr << iter->second.parentMenu << "::";
            }
            actionStr << iter->second.action;
            cols.push_back(actionStr);
            cols.push_back(iter->second.accel);
            m_dataviewModel->AppendItem(wxDataViewItem(0), cols, new AccelItemData(iter->second, false));
        }
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
    PopulateTable();
}

void AccelTableDlg::OnEditButton(wxCommandEvent& e)
{
    wxUnusedVar(e);
    DoItemActivated();
}

void AccelTableDlg::DoItemActivated()
{
    wxDataViewItem sel = m_dataview->GetSelection();
    CHECK_ITEM_RET(sel);

    AccelItemData* itemData = dynamic_cast<AccelItemData*>(m_dataviewModel->GetClientObject(sel));

    // build the selected entry
    MenuItemData mid = itemData->menuItemData;
    if(clKeyboardManager::Get()->PopupNewKeyboardShortcutDlg(this, mid) == wxID_OK) {
        // search the list for similar accelerator
        MenuItemData who;
        if(HasAccelerator(mid.accel, who)) {
            wxMessageBox(wxString::Format(_("'%s' is already assigned to: '%s'"), mid.accel, who.action),
                         _("CodeLite"),
                         wxOK | wxCENTER | wxICON_WARNING,
                         this);
            return;
        }

        // Update the client data
        itemData->menuItemData = mid;

        // Update the UI
        wxVector<wxVariant> cols;
        cols.push_back(mid.action);
        cols.push_back(mid.accel);
        m_dataviewModel->UpdateItem(sel, cols);
        
        // and update the map
        MenuItemDataMap_t::iterator iter = m_accelMap.find(itemData->menuItemData.resourceID);
        if(iter != m_accelMap.end()) {
            iter->second.accel = itemData->menuItemData.accel;
        }
    }
}

void AccelTableDlg::OnText(wxCommandEvent& event)
{
    wxUnusedVar(event);
    PopulateTable(m_textCtrlFilter->GetValue());
}

AccelTableDlg::~AccelTableDlg() { WindowAttrManager::Save(this, "AccelTableDlg", NULL); }
void AccelTableDlg::OnDVItemActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    DoItemActivated();
}

void AccelTableDlg::OnEditUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dataview->GetSelectedItemsCount());
}

bool AccelTableDlg::IsMatchesFilter(const wxString& filter, const MenuItemData& item)
{
    wxString lcFilter = filter.Lower();
    lcFilter.Trim().Trim(false);
    if(lcFilter.IsEmpty()) return true;
    
    wxString label = item.parentMenu + " :: " + item.action;
    wxString action = label.Lower();
    wxString accel = item.accel.Lower();
    
    wxArrayString filters = ::wxStringTokenize(lcFilter, " ", wxTOKEN_STRTOK);
    for(size_t i=0; i<filters.GetCount(); ++i) {
        if(!action.Contains(filters.Item(i)) && !accel.Contains(filters.Item(i)))
            return false;
    }
    return true;
}

bool AccelTableDlg::HasAccelerator(const wxString& accel, MenuItemData& who)
{
    for(MenuItemDataMap_t::iterator iter = m_accelMap.begin(); iter != m_accelMap.end(); ++iter) {
        if(iter->second.accel == accel) {
            who = iter->second;
            return true;
        }
    }
    return false;
}
