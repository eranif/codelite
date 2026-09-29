//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : acceltabledlg.h
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
#ifndef __acceltabledlg__
#define __acceltabledlg__

#include "AccelTableBaseDlg.hpp"
#include "Keyboard/clKeyboardManager.h"

#include <map>
#include <vector>

struct AccelItemData : public wxClientData {
    MenuItemData m_menuItemData;
    AccelItemData(const MenuItemData& data)
        : m_menuItemData(data)
    {
    }
};

class AccelTableDlg : public AccelTableBaseDlg
{
    MenuItemDataMap_t m_accelMap;

protected:
    void OnEditUI(wxUpdateUIEvent& event) override;
    void OnDVItemActivated(wxDataViewEvent& event) override;

    // Handlers for AccelTableBaseDlg events
    void PopulateTable(const wxString& filter);
    void OnButtonOk(wxCommandEvent& e) override;
    void OnButtonDefaults(wxCommandEvent& e) override;
    void OnEditButton(wxCommandEvent& e) override;
    void OnText(wxCommandEvent& event) override;

    void DoItemActivated();
    bool IsMatchesFilter(const wxString& filter, const MenuItemData& item);
    bool HasAccelerator(const clKeyboardShortcut& accel, MenuItemData& who);
    AccelItemData* DoGetItemData(const wxDataViewItem& item);
    wxDataViewItem FindAccel(const MenuItemData& mid);

public:
    /** Constructor */
    AccelTableDlg(wxWindow* parent);
    ~AccelTableDlg() override = default;
};

#endif // __acceltabledlg__
