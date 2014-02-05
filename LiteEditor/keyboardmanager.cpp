//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : keyboardmanager.cpp
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

#include <wx/ffile.h>
#include "newkeyshortcutdlg.h"
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>
#include "keyboardmanager.h"
#include "manager.h"

KeyboardManager::KeyboardManager()
{
}

KeyboardManager::~KeyboardManager()
{
}

bool KeyboardManager::AddAccelerator(MenuItemDataMap &accelMap, const MenuItemData& menuItemData)
{
    if ( menuItemData.accel.IsEmpty() == false && IsDuplicate(accelMap, menuItemData.accel) ) {
        return false;
    }

    accelMap[wxXmlResource::GetXRCID(menuItemData.id)] = menuItemData;
    return true;
}

void KeyboardManager::GetAccelerators(MenuItemDataMap& accelMap)
{
    MenuItemDataMap defAccelMap;
    ManagerST::Get()->GetAcceleratorMap(accelMap);

    // load the default accelerator map
    ManagerST::Get()->GetDefaultAcceleratorMap(defAccelMap);

    // loop over default accelerators map, and search for items that does not exist in the user's list
    std::map< int, MenuItemData >::iterator it = defAccelMap.begin();
    for (; it != defAccelMap.end(); it++) {
        if (accelMap.find(it->first) == accelMap.end()) {
            // this item does not exist in the users accelerators
            // probably a new accelerator that was added to the default
            // files directly via update/manully modified it
            accelMap[it->first] = it->second;
        }
    }
}

bool KeyboardManager::IsDuplicate(const MenuItemDataMap& accelMap, const wxString& accelerator)
{
    // search the list for similar accelerator
    MenuItemDataMap::const_iterator iter = accelMap.begin();

    for(; iter != accelMap.end(); iter ++) {
        MenuItemData mid = iter->second;
        if(Compare(accelerator, mid.accel)) {
            return true;
        }
    }
    return false;
}

int KeyboardManager::PopupNewKeyboardShortcutDlg(wxWindow* parent, MenuItemData& menuItemData)
{
    NewKeyShortcutDlg dlg(parent, menuItemData);
    if(dlg.ShowModal() == wxID_OK) {
        menuItemData.accel = dlg.GetAccel();
        return wxID_OK;
    }
    return wxID_CANCEL;
}

void KeyboardManager::Update(const MenuItemDataMap& accelMap)
{
    //export the content of table, and apply the changes
    wxString content;
    MenuItemDataMap::const_iterator iter = accelMap.begin();
    for(; iter != accelMap.end(); iter++) {
        MenuItemData mid = iter->second;
        content << mid.id;
        content << wxT("|");
        content << mid.parent;
        content << wxT("|");
        content << mid.action;
        content << wxT("|");
        content << mid.accel;
        content << wxT("\n");
    }

    wxString fileName = clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config/accelerators.conf");
    wxFFile file;
    if (!file.Open(fileName, wxT("w+b"))) {
        return;
    }

    file.Write(content);
    file.Close();

    // Apply changes
    ManagerST::Get()->UpdateMenuAccelerators(); // Main frame
}

bool KeyboardManager::Compare(const wxString& accel1, const wxString& accel2)
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
