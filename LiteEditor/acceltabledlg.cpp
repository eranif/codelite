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

struct ParentSorter {
    bool operator()(const MenuItemData& rStart, const MenuItemData& rEnd)
    {
        return rEnd.parent.CmpNoCase(rStart.parent) < 0;
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

struct ParentRSorter {
    bool operator()(const MenuItemData& rStart, const MenuItemData& rEnd)
    {
        return rEnd.parent.CmpNoCase(rStart.parent) > 0;
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

    PluginManager::Get()->GetKeyboardManager()->GetAccelerators(m_coreAccelMap);
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
    for(MenuItemDataMap::iterator iter = m_coreAccelMap.begin(); iter != m_coreAccelMap.end(); ++iter) {
        if(IsMatchesFilter(filter, iter->second)) {
            wxVector<wxVariant> cols;
            cols.push_back(iter->second.action);
            cols.push_back(iter->second.accel);
            m_dataviewModel->AppendItem(wxDataViewItem(0), cols, new AccelItemData(iter->second, false));
        }
    }

    // Add plugins entries
    for(MenuItemDataMap::iterator iter = m_pluginsAccelMap.begin(); iter != m_pluginsAccelMap.end(); ++iter) {
        if(IsMatchesFilter(filter, iter->second)) {
            wxVector<wxVariant> cols;
            cols.push_back(iter->second.action);
            cols.push_back(iter->second.accel);
            m_dataviewModel->AppendItem(wxDataViewItem(0), cols, new AccelItemData(iter->second, true));
        }
    }
}

void AccelTableDlg::OnButtonOk(wxCommandEvent& e)
{
    // export the content of table, and apply the changes
    wxString content;
    wxDataViewItemArray children;
    m_dataviewModel->GetChildren(wxDataViewItem(NULL), children);
    for(size_t i = 0; i < children.GetCount(); ++i) {
        AccelItemData* itemData = dynamic_cast<AccelItemData*>(m_dataviewModel->GetClientObject(children.Item(i)));
        if(!itemData->isPluginAccel) {
            MenuItemData mid = itemData->menuItemData;
            content << mid.id;
            content << wxT("|");
            content << mid.parent;
            content << wxT("|");
            content << mid.action;
            content << wxT("|");
            content << mid.accel;
            content << wxT("\n");
        }
    }

    wxString fileName;
    fileName =
        clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config/accelerators.conf");

    wxFFile file;
    if(!file.Open(fileName, wxT("w+b"))) {
        return;
    }

    file.Write(content);
    file.Close();

    // apply changes
    ManagerST::Get()->UpdateMenuAccelerators();

    EndModal(wxID_OK);
}

void AccelTableDlg::OnButtonDefaults(wxCommandEvent& e)
{
    // re-load the default key bindings settings
    m_coreAccelMap.clear();
    m_pluginsAccelMap.clear();
    m_textCtrlFilter->ChangeValue(""); // Clear the filter
    ManagerST::Get()->GetDefaultAcceleratorMap(m_coreAccelMap);
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
    if(PluginManager::Get()->GetKeyboardManager()->PopupNewKeyboardShortcutDlg(this, mid) == wxID_OK) {
        // search the list for similar accelerator
        wxDataViewItemArray children;
        m_dataviewModel->GetChildren(wxDataViewItem(NULL), children);
        for(size_t i = 0; i < children.GetCount(); ++i) {
            AccelItemData* cd = dynamic_cast<AccelItemData*>(m_dataviewModel->GetClientObject(children.Item(i)));
            if(cd->menuItemData.accel == mid.accel) {
                wxString action = cd->menuItemData.action;
                wxMessageBox(wxString::Format(_("'%s' is already assigned to: '%s'"), mid.accel, action),
                             _("CodeLite"),
                             wxOK | wxCENTER | wxICON_WARNING,
                             this);
                return;
            }
        }

        // Update the client data
        itemData->menuItemData = mid;

        // Update the UI
        wxVector<wxVariant> cols;
        cols.push_back(mid.action);
        cols.push_back(mid.accel);
        m_dataviewModel->UpdateItem(sel, cols);
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
    
    wxString action = item.action.Lower();
    wxString accel = item.accel.Lower();
    
    wxArrayString filters = ::wxStringTokenize(lcFilter, " ", wxTOKEN_STRTOK);
    for(size_t i=0; i<filters.GetCount(); ++i) {
        if(!action.Contains(filters.Item(i)) || accel.Contains(filters.Item(i)))
            return false;
    }
    return true;
}
