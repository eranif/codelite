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

enum class WorkspaceType {
    LOCAL,
    REMOTE,
};

struct WelcomePageItemData : public wxTreeItemData {
    WorkspaceType type = WorkspaceType::LOCAL;
    wxString path;
    wxString account; // for remote workspaces
    WelcomePageItemData() {}
    virtual ~WelcomePageItemData() {}
};

class WelcomePage : public WelcomePageBase
{
protected:
    void OnNewWorkspace(wxCommandEvent& event) override;
    void OnOpenWorkspace(wxCommandEvent& event) override;
    void OnGitHHub(wxCommandEvent& event) override;
    void OnGitter(wxCommandEvent& event) override;
    void OnWorkspaceActivated(wxTreeEvent& event) override;
    void InitialiseUI();
    WelcomePageItemData* GetWorkspaceItemData(const wxTreeItemId& item);
    void OpenLocalWorkspace(WelcomePageItemData* cd);
    void OpenRemoteWorkspace(WelcomePageItemData* cd);

public:
    WelcomePage(wxWindow* parent);
    virtual ~WelcomePage();

protected:
    void OnSize(wxSizeEvent& event) override;
    void OnThemeChanged(clCommandEvent& e);
};
#endif // WELCOMEPAGE_H
