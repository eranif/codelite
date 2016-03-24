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

#include "AccelTableBaseDlg.h"
#include <map>
#include <vector>
#include "clKeyboardManager.h"

struct AccelItemData : public wxClientData {
    MenuItemData m_menuItemData;
    bool m_isParent;
    wxString m_displayName;
    AccelItemData(const wxString& displayName)
        : m_isParent(true)
        , m_displayName(displayName)
    {
    }

    AccelItemData(const MenuItemData& data, bool isParent, const wxString& displayName)
        : m_menuItemData(data)
        , m_isParent(isParent)
        , m_displayName(displayName)
    {
    }
};

class AccelTableDlg : public AccelTableBaseDlg
{
    MenuItemDataMap_t m_accelMap;

protected:
    virtual void OnEditUI(wxUpdateUIEvent& event);
    virtual void OnDVItemActivated(wxDataViewEvent& event);

    // Handlers for AccelTableBaseDlg events
    void PopulateTable(const wxString& filter = "");
    void OnButtonOk(wxCommandEvent& e);
    void OnButtonDefaults(wxCommandEvent& e);
    void OnEditButton(wxCommandEvent& e);
    void OnText(wxCommandEvent& event);

    void DoItemActivated();
    bool IsMatchesFilter(const wxString& filter, const MenuItemData& item);
    bool HasAccelerator(const wxString& accel, MenuItemData& who);
    wxDataViewItem DoAddParentNode(std::map<wxString, wxDataViewItem>& parentsMap, const wxString& parentKey);
    AccelItemData* DoGetItemData(const wxDataViewItem& item);
    wxDataViewItem FindAccel(const MenuItemData& mid);

public:
    /** Constructor */
    AccelTableDlg(wxWindow* parent);
    virtual ~AccelTableDlg();
};

#endif // __acceltabledlg__
