//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : tabgroupdlg.h
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

#ifndef __tabgroupdlg__
#define __tabgroupdlg__

#include "sessionmanager.h"
#include "tabgroupbasedlgs.hpp"

class LoadTabGroupDlg : public LoadTabGroupBaseDlg
{
    void OnBrowse(wxCommandEvent& WXUNUSED(event)) override;
    void OnItemActivated(wxCommandEvent& WXUNUSED(event)) override;

public:
    LoadTabGroupDlg(wxWindow* parent, const wxString& path, const wxArrayString& previousgroups);
    ~LoadTabGroupDlg() override = default;

    void SetListTabs(const wxArrayString& tabs) { m_listBox->Set(tabs); }

    void InsertListItem(const wxString& item);

    void EnableReplaceCheck(bool value) { m_replaceCheck->Enable(value); }

    wxListBox* GetListBox() { return m_listBox; }

    bool GetReplaceCheck() { return m_replaceCheck->IsChecked(); }
};

class SaveTabGroupDlg : public SaveTabGroupBaseDlg
{
    void OnCheckAll(wxCommandEvent& event) override;
    void OnCheckAllUpdateUI(wxUpdateUIEvent& event) override;
    void OnClearAll(wxCommandEvent& event) override;
    void OnClearAllUpdateUI(wxUpdateUIEvent& event) override;

public:
    SaveTabGroupDlg(wxWindow* parent, const wxArrayString& previousgroups);
    ~SaveTabGroupDlg() override = default;
    void SetListTabs(const wxArrayString& tabs)
    {
        m_ListTabs->Set(tabs);
        for (unsigned int n = 0; n < m_ListTabs->GetCount(); ++n) {
            m_ListTabs->Check(n, true);
        }
    }

    void SetTextName(const wxString& name) { m_textName->SetValue(name); }

    bool GetChoices(wxArrayInt& intArr) const;

    wxString GetTabgroupName() const { return m_textName->GetValue(); }

    bool GetSaveInWorkspace() const
    {
        return m_radioBoxWorkspaceOrGlobal->IsShown() && m_radioBoxWorkspaceOrGlobal->GetSelection() == 0;
    }
};
#endif // __tabgroupdlg__
