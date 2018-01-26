//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : tabgroupdlg.cpp
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

#include "tabgroupdlg.h"
#include "windowattrmanager.h"
#include "manager.h"
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/dir.h>

LoadTabGroupDlg::LoadTabGroupDlg(wxWindow* parent, const wxString& path, const wxArrayString& previousgroups)
    : LoadTabGroupBaseDlg(parent)
{
    for(size_t n = 0; n < previousgroups.GetCount(); ++n) {
        InsertListItem(previousgroups.Item(n));
    }

    // Add any groups in the passed dir (which is probably the workspace dir)
    wxArrayString pwdfiles;
    wxDir::GetAllFiles(path, &pwdfiles, wxT("*.tabgroup"), wxDIR_FILES);
    for(size_t n = 0; n < pwdfiles.GetCount(); ++n) {
        InsertListItem(pwdfiles.Item(n));
    }
    SetName("LoadTabGroupDlg");
    WindowAttrManager::Load(this);
}

LoadTabGroupDlg::~LoadTabGroupDlg() {}

void LoadTabGroupDlg::InsertListItem(const wxString& item)
{
    if(item.IsEmpty() == false) {
        int AlreadyThere = m_listBox->FindString(item, true);

        if(AlreadyThere != wxNOT_FOUND) {
            m_listBox->SetSelection(AlreadyThere);
        } else {
            m_listBox->Insert(item, 0);
            m_listBox->SetSelection(0);
        }
    }
}

void LoadTabGroupDlg::OnBrowse(wxCommandEvent& WXUNUSED(event))
{
    wxString group = wxFileSelector(_("Select a tab group:"),
                                    wxT(""),
                                    wxT(""),
                                    tabgroupTag,
                                    wxString(_("Tab groups")) + wxT(" (*.tabgroup)|*.tabgroup"),
                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                                    this);
    if(!group.IsEmpty()) {
        // Need to insert/select the new item, otherwise it won't be used
        InsertListItem(group);
        EndModal(wxID_OK);
    }
}

void LoadTabGroupDlg::OnItemActivated(wxCommandEvent& WXUNUSED(event))
{
    int idx = m_listBox->GetSelection();
    EndModal((idx == wxNOT_FOUND) ? wxID_CANCEL : wxID_OK);
}

//---------------------------------------------

SaveTabGroupDlg::SaveTabGroupDlg(wxWindow* parent, const wxArrayString& previousgroups)
    : SaveTabGroupBaseDlg(parent)
{
    SetName("SaveTabGroupDlg");
    WindowAttrManager::Load(this);
    
    m_radioBoxWorkspaceOrGlobal->Show(ManagerST::Get()->IsWorkspaceOpen());
}

SaveTabGroupDlg::~SaveTabGroupDlg() {}

bool SaveTabGroupDlg::GetChoices(wxArrayInt& intArr) const
{
    bool SomeChecked = false;
    for(unsigned int n = 0; n < m_ListTabs->GetCount(); ++n) {
        bool item = m_ListTabs->IsChecked(n);
        intArr.Add(item);
        if(item) {
            SomeChecked = true;
        }
    }

    return SomeChecked;
}

void SaveTabGroupDlg::OnCheckAll(wxCommandEvent& WXUNUSED(event))
{
    if(m_ListTabs == NULL) return;

    for(unsigned int n = 0; n < m_ListTabs->GetCount(); ++n) {
        m_ListTabs->Check(n, true);
    }
}

void SaveTabGroupDlg::OnCheckAllUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_ListTabs && m_ListTabs->GetCount()); }

void SaveTabGroupDlg::OnClearAll(wxCommandEvent& WXUNUSED(event))
{
    if(m_ListTabs == NULL) return;

    for(unsigned int n = 0; n < m_ListTabs->GetCount(); ++n) {
        m_ListTabs->Check(n, false);
    }
}

void SaveTabGroupDlg::OnClearAllUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_ListTabs && m_ListTabs->GetCount()); }
