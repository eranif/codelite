//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : WelcomePage.h
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

#pragma once

#include "clThemedButton.h"
#include "wxcrafter.hpp"

#include <map>
#include <memory>
#include <vector>
#include <wx/commandlinkbutton.h>

class DataViewTypeHelper;

enum class WorkspaceSource {
    BUILTIN,
    PLUGIN,
};

struct WelcomePageItemData {
    WorkspaceSource type = WorkspaceSource::BUILTIN;
    wxString path;
    wxString account; // for remote workspaces
    wxString name;
    wxString workspace_type;
    int image_index = wxNOT_FOUND;
};

class WelcomePage : public WelcomePageBase
{
public:
    WelcomePage(wxWindow* parent);
    ~WelcomePage() override;
    bool Show(bool show = true) override;
    wxWindow* GetList() { return m_dvTreeCtrlWorkspaces; }
    void SelectSomething();
    void GrabFocus();

protected:
    void OnCloseButtonClicked(wxCommandEvent& event) override;
    void OnNewWorkspace(wxCommandEvent& event);
    void OnOpenWorkspace(wxCommandEvent& event);
    void OnGitHub(wxCommandEvent& event);
    void OnGitter(wxCommandEvent& event);
    void OnWorkspaceActivated(wxDataViewEvent& event) override;
    void UpdateRecentWorkspaces();
    /// Sort `m_items` by the current sort column and fill the list. Each row keeps the index of its data in `m_items`
    void PopulateList();
    void OnColumnHeaderClick(wxDataViewEvent& event);
    const WelcomePageItemData* GetWorkspaceItemData(const wxDataViewItem& item) const;
    void OpenBuiltinWorkspace(const WelcomePageItemData* cd);
    void OpenPluginWorkspace(const WelcomePageItemData* cd);
    void DoSomethingSomething();
    void OnSize(wxSizeEvent& event) override;
    void OnThemeChanged(clCommandEvent& e);

    wxCommandLinkButton* m_buttonGithub{nullptr};
    wxCommandLinkButton* m_buttonNewWorkspace{nullptr};
    wxCommandLinkButton* m_buttonOpenWorkspace{nullptr};
    wxCommandLinkButton* m_buttonGitter{nullptr};
    std::vector<WelcomePageItemData> m_items;
    int m_sortColumn = wxNOT_FOUND; // wxNOT_FOUND: keep the original order
    bool m_sortAscending = true;
    std::unique_ptr<DataViewTypeHelper> m_typeHelper;
};

wxDECLARE_EVENT(wxEVT_WELCOMEPAGE_CLOSE_BUTTON_CLICKED, clCommandEvent);
