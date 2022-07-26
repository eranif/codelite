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

#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include "clThemedButton.h"
#include "wxcrafter.h"

#include <map>

enum class WorkspaceSource {
    BUILTIN,
    PLUGIN,
};

struct WelcomePageItemData : public wxTreeItemData {
    WorkspaceSource type = WorkspaceSource::BUILTIN;
    wxString path;
    wxString account; // for remote workspaces
    WelcomePageItemData() {}
    virtual ~WelcomePageItemData() {}
};

class WelcomePage : public WelcomePageBase
{
    clThemedButton* m_buttonGithub = nullptr;
    clThemedButton* m_buttonNewWorkspace = nullptr;
    clThemedButton* m_buttonOpenWorkspace = nullptr;
    clThemedButton* m_buttonGitter = nullptr;

protected:
    void OnNewWorkspace(wxCommandEvent& event);
    void OnOpenWorkspace(wxCommandEvent& event);
    void OnGitHHub(wxCommandEvent& event);
    void OnGitter(wxCommandEvent& event);
    void OnWorkspaceActivated(wxTreeEvent& event) override;
    void UpdateRecentWorkspaces();
    WelcomePageItemData* GetWorkspaceItemData(const wxTreeItemId& item);
    void OpenBuiltinWorkspace(WelcomePageItemData* cd);
    void OpenPluginWorkspace(WelcomePageItemData* cd);

public:
    WelcomePage(wxWindow* parent);
    virtual ~WelcomePage();
    bool Show(bool show = true) override;

protected:
    void OnSize(wxSizeEvent& event) override;
    void OnThemeChanged(clCommandEvent& e);
};
#endif // WELCOMEPAGE_H
