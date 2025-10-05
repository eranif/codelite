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
#include "wxcrafter.h"

#include <map>
#include <wx/commandlinkbutton.h>

enum class WorkspaceSource {
    BUILTIN,
    PLUGIN,
};

struct WelcomePageItemData : public wxTreeItemData {
    WorkspaceSource type = WorkspaceSource::BUILTIN;
    wxString path;
    wxString account; // for remote workspaces
    WelcomePageItemData() = default;
    ~WelcomePageItemData() override = default;
};

class WelcomePage : public WelcomePageBase
{
public:
    WelcomePage(wxWindow* parent);
    virtual ~WelcomePage();
    bool Show(bool show = true) override;
    wxWindow* GetList() { return m_dvTreeCtrlWorkspaces; }
    void SelectSomething();
    void GrabFocus();

protected:
    void OnCloseButtonClicked(wxCommandEvent& event) override;
    void OnNewWorkspace(wxCommandEvent& event);
    void OnOpenWorkspace(wxCommandEvent& event);
    void OnGitHHub(wxCommandEvent& event);
    void OnGitter(wxCommandEvent& event);
    void OnWorkspaceActivated(wxTreeEvent& event) override;
    void UpdateRecentWorkspaces();
    WelcomePageItemData* GetWorkspaceItemData(const wxTreeItemId& item);
    void OpenBuiltinWorkspace(WelcomePageItemData* cd);
    void OpenPluginWorkspace(WelcomePageItemData* cd);
    void DoSomethingSomething();
    void OnSize(wxSizeEvent& event) override;
    void OnThemeChanged(clCommandEvent& e);

    wxCommandLinkButton* m_buttonGithub{nullptr};
    wxCommandLinkButton* m_buttonNewWorkspace{nullptr};
    wxCommandLinkButton* m_buttonOpenWorkspace{nullptr};
    wxCommandLinkButton* m_buttonGitter{nullptr};
};

wxDECLARE_EVENT(wxEVT_WELCOMEPAGE_CLOSE_BUTTON_CLICKED, clCommandEvent);
