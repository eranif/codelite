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

class WelcomePage : public WelcomePageBase
{
    typedef std::map<int, wxString> IntStringMap_t;
    IntStringMap_t m_idToName;
    clThemedButton* m_cmdLnkBtnNewWorkspace = nullptr;
    clThemedButton* m_cmdLnkBtnNewProject = nullptr;
    clThemedButton* m_cmdLnkBtnWorkspaces = nullptr;
    clThemedButton* m_cmdLnkBtnFilesMenu = nullptr;
    clThemedButton* m_cmdLnkBtnForum = nullptr;
    clThemedButton* m_cmdLnkBtnWiki = nullptr;

protected:
    virtual void OnOpenWorkspace(wxCommandEvent& event);
    virtual void OnNewWorkspace(wxCommandEvent& event);
    virtual void OnRecentFileUI(wxUpdateUIEvent& event);
    virtual void OnRecentProjectUI(wxUpdateUIEvent& event);
    int DoGetPopupMenuSelection(clThemedButton* btn, const wxArrayString& strings, const wxString& menuTitle);
    void DoOpenFile(const wxString& filename);
    void AddButtons();

public:
    WelcomePage(wxWindow* parent);
    virtual ~WelcomePage();

protected:
    virtual void OnShowFileseMenu(wxCommandEvent& event);
    virtual void OnShowWorkspaceMenu(wxCommandEvent& event);
    virtual void OnSize(wxSizeEvent& event);
    virtual void OnOpenForums(wxCommandEvent& event);
    virtual void OnOpenWiki(wxCommandEvent& event);
    void OnThemeChanged(clCommandEvent& e);
};
#endif // WELCOMEPAGE_H
