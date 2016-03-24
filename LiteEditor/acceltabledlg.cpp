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
#include <wx/tokenzr.h>

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
    CentreOnParent();

    m_textCtrlFilter->SetFocus();
    m_dataview->SetIndent(16);

    SetName("AccelTableDlg");
    WindowAttrManager::Load(this);
}

void AccelTableDlg::PopulateTable(const wxString& filter)
{
    m_dataviewModel->Clear();

    std::map<wxString, wxDataViewItem> parents;
    // Add core entries
    for(MenuItemDataMap_t::iterator iter = m_accelMap.begin(); iter != m_accelMap.end(); ++iter) {
        if(!IsMatchesFilter(filter, iter->second)) continue;

        MenuItemData& mid = iter->second;
        wxString parentNodeKey, childKey;
        if(mid.parentMenu.IsEmpty()) {
            wxString strAction = mid.action;
            strAction.Replace("::", "@");
            parentNodeKey = strAction.BeforeLast('@');
            childKey = strAction.AfterLast('@');
        } else {
            parentNodeKey = mid.parentMenu;
            childKey = mid.action;

            parentNodeKey.Replace("::", "@");
            childKey.Replace("::", "@");
        }

        std::map<wxString, wxDataViewItem>::iterator parentIter = parents.find(parentNodeKey);
        wxDataViewItem parentItem;
        if(parentIter == parents.end()) {

            // this parent does not yet exist, add it (this function also updates the cache)
            parentItem = DoAddParentNode(parents, parentNodeKey);

        } else {
            parentItem = parentIter->second;
        }

        wxVector<wxVariant> cols;
        cols.push_back(childKey);
        cols.push_back(mid.accel);
        m_dataviewModel->AppendItem(parentItem, cols, new AccelItemData(mid, false, childKey));
    }

    if(!filter.IsEmpty()) {
        std::map<wxString, wxDataViewItem>::iterator iter = parents.begin();
        for(; iter != parents.end(); ++iter) {
            if(m_dataviewModel->HasChildren(iter->second)) {
                m_dataview->Expand(iter->second);
            }
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

    AccelItemData* itemData = DoGetItemData(sel);
    if(itemData->m_isParent) return;

    // build the selected entry
    MenuItemData mid = itemData->m_menuItemData;
    if(clKeyboardManager::Get()->PopupNewKeyboardShortcutDlg(this, mid) == wxID_OK) {
        // search the list for similar accelerator
        MenuItemData who;
        if(HasAccelerator(mid.accel, who)) {
            if(wxMessageBox(wxString::Format(_("'%s' is already assigned to: '%s'\nWould you like to replace it?"),
                                mid.accel, who.action),
                   _("CodeLite"), wxYES_NO | wxCENTER | wxICON_QUESTION, this) != wxYES) {
                return;
            }

            // Remove the old entry
            wxDataViewItem oldItem = FindAccel(mid);
            if(oldItem.IsOk()) {
                AccelItemData* cd = DoGetItemData(oldItem);
                if(cd) {
                    cd->m_menuItemData.accel.clear();
                    wxVector<wxVariant> cols;
                    cols.push_back(cd->m_menuItemData.action.AfterLast(':'));
                    cols.push_back(wxString());
                    m_dataviewModel->UpdateItem(oldItem, cols);

                    MenuItemDataMap_t::iterator iter = m_accelMap.find(cd->m_menuItemData.resourceID);
                    if(iter != m_accelMap.end()) {
                        iter->second.accel = cd->m_menuItemData.accel;
                    }
                }
            }
        }

        // Update the client data
        itemData->m_menuItemData = mid;

        // Update the UI
        wxVector<wxVariant> cols;
        cols.push_back(mid.action.AfterLast(':'));
        cols.push_back(mid.accel);
        m_dataviewModel->UpdateItem(sel, cols);

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
    PopulateTable(m_textCtrlFilter->GetValue());
}

AccelTableDlg::~AccelTableDlg() {}
void AccelTableDlg::OnDVItemActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    DoItemActivated();
}

void AccelTableDlg::OnEditUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dataview->GetSelectedItemsCount() && !DoGetItemData(m_dataview->GetSelection())->m_isParent);
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
    for(size_t i = 0; i < filters.GetCount(); ++i) {
        if(!action.Contains(filters.Item(i)) && !accel.Contains(filters.Item(i))) return false;
    }
    return true;
}

bool AccelTableDlg::HasAccelerator(const wxString& accel, MenuItemData& who)
{
    if(accel.IsEmpty()) return false;
    for(MenuItemDataMap_t::iterator iter = m_accelMap.begin(); iter != m_accelMap.end(); ++iter) {
        if(iter->second.accel == accel) {
            who = iter->second;
            return true;
        }
    }
    return false;
}

wxDataViewItem AccelTableDlg::DoAddParentNode(std::map<wxString, wxDataViewItem>& parentsMap, const wxString& parentKey)
{
    wxString currentKey;
    wxArrayString parts = ::wxStringTokenize(parentKey, "@", wxTOKEN_STRTOK);
    wxDataViewItem parent = wxDataViewItem(0); // starting from the top
    for(size_t i = 0; i < parts.GetCount(); ++i) {
        if(!currentKey.IsEmpty()) {
            currentKey << "@";
        }

        currentKey << parts.Item(i);
        if(parentsMap.count(currentKey)) {
            parent = parentsMap.find(currentKey)->second;
            continue;
        }

        wxDataViewItemArray children;
        m_dataviewModel->GetChildren(parent, children);

        bool parentExists = false;
        for(size_t j = 0; j < children.GetCount(); ++j) {
            AccelItemData* cd = DoGetItemData(children.Item(j));
            if(cd->m_displayName == parts.Item(i)) {
                // we got a match
                parentExists = true;
                break;
            }
        }

        if(parentExists) continue;
        // Add it
        wxVector<wxVariant> cols;
        cols.push_back(parts.Item(i));
        cols.push_back(wxString(""));
        parent = m_dataviewModel->AppendItem(parent, cols, new AccelItemData(parts.Item(i)));
        parentsMap.insert(std::make_pair(currentKey, parent));
    }
    return parent;
}

AccelItemData* AccelTableDlg::DoGetItemData(const wxDataViewItem& item)
{
    return static_cast<AccelItemData*>(m_dataviewModel->GetClientObject(item));
}

wxDataViewItem AccelTableDlg::FindAccel(const MenuItemData& mid)
{
    wxDataViewItemArray children;
    m_dataviewModel->GetChildren(wxDataViewItem(0), children);
    std::vector<wxDataViewItemArray> q;
    q.push_back(children);
    while(!q.empty()) {
        children = q.back();
        q.pop_back();
        for(size_t i = 0; i < children.GetCount(); ++i) {
            wxDataViewItem item = children.Item(i);
            AccelItemData* cd = DoGetItemData(item);
            if(cd && cd->m_menuItemData.accel == mid.accel) {
                return item;
            }

            // if this item has more children, add them to the queue
            if(m_dataviewModel->HasChildren(item)) {
                wxDataViewItemArray c;
                m_dataviewModel->GetChildren(item, c);
                q.push_back(c);
            }
        }
    }
    return wxDataViewItem();
}
