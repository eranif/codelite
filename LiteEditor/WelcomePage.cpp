//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : WelcomePage.cpp
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

#include "WelcomePage.h"

#include "bitmap_loader.h"
#include "clSystemSettings.h"
#include "cl_defs.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "frame.h"
#include "globals.h"
#include "manager.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "window_locker.h"

#include <wx/arrstr.h>
#include <wx/clntdata.h>

WelcomePage::WelcomePage(wxWindow* parent)
    : WelcomePageBase(parent)
{
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    GetPanel191()->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    AddButtons();

    m_staticBitmap->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("codelite-logo", 32));
    m_staticBitmap->Hide();
    GetSizer()->Fit(this);

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &WelcomePage::OnThemeChanged, this);

#if CL_USE_NATIVEBOOK
    Show();
#endif
}

WelcomePage::~WelcomePage()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &WelcomePage::OnThemeChanged, this);
}

void WelcomePage::OnOpenForums(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ::wxLaunchDefaultBrowser("https://forums.codelite.org/");
}

void WelcomePage::OnOpenWiki(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ::wxLaunchDefaultBrowser("https://docs.codelite.org");
}

void WelcomePage::OnSize(wxSizeEvent& event)
{
    event.Skip();
    m_staticBitmap->Refresh();
}

void WelcomePage::OnShowFileseMenu(wxCommandEvent& event)
{
    wxArrayString recentFiles;
    clMainFrame::Get()->GetMainBook()->GetRecentlyOpenedFiles(recentFiles);

    int id = DoGetPopupMenuSelection(m_cmdLnkBtnFilesMenu, recentFiles, _("Select file to open"));
    if(id != wxID_NONE) {
        CallAfter(&WelcomePage::DoOpenFile, m_idToName[id]);
    }
}

void WelcomePage::OnShowWorkspaceMenu(wxCommandEvent& event)
{
    wxArrayString files;
    ManagerST::Get()->GetRecentlyOpenedWorkspaces(files);
    //  files.Sort();

    wxArrayString recentWorkspaces;
    for(size_t i = 0; i < files.size(); ++i) {
        wxFileName fn(files.Item(i));
        recentWorkspaces.Add(fn.GetName() + " @ " + fn.GetFullPath());
    }

    int id = DoGetPopupMenuSelection(m_cmdLnkBtnWorkspaces, recentWorkspaces, _("Open workspace"));
    if(id != wxID_NONE) {
        wxString filename = m_idToName[id];
        filename = filename.AfterFirst('@');
        filename.Trim().Trim(false);

        // post an event to the main frame requesting a workspace open
        wxCommandEvent evtOpenworkspace(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
        evtOpenworkspace.SetString(filename);
        evtOpenworkspace.SetEventObject(clMainFrame::Get());
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
    }
}

int WelcomePage::DoGetPopupMenuSelection(clThemedButton* btn, const wxArrayString& strings, const wxString& menuTitle)
{
    BitmapLoader* loader = PluginManager::Get()->GetStdIcons();

    m_idToName.clear();
    wxUnusedVar(menuTitle);
    wxMenu menu;

    for(size_t i = 0; i < strings.GetCount(); ++i) {

        wxBitmap bmp = loader->GetBitmapForFile("a.txt");
        wxString filename = strings.Item(i);
        if(filename.Find("@") != wxNOT_FOUND) {
            filename = filename.AfterFirst('@');
        }
        filename.Trim().Trim(false);

        // Ensure that the file exists...
        if(!wxFileName(filename).Exists()) {
            // Don't show non existing files in the menu
            continue;
        }
        bmp = loader->GetBitmapForFile(filename);
        wxMenuItem* item = new wxMenuItem(&menu, wxID_ANY, strings.Item(i));
        item->SetBitmap(bmp);
        m_idToName.insert(std::make_pair(item->GetId(), strings.Item(i)));
        menu.Append(item);
    }

    // get the best position to show the menu
    wxPoint pos = btn->GetPosition();
    pos = m_scrollWin247->CalcScrolledPosition(pos);

    pos.y += btn->GetSize().y;
#ifdef __WXGTK__
    pos.y += 5;
    pos.x += 5;
#elif defined(__WXMAC__)
    pos.y += 10;
    pos.x += 10;
#else // MSW
    pos.y += 5;
    pos.x += 5;
#endif
    return GetPopupMenuSelectionFromUser(menu, pos);
}

void WelcomePage::OnRecentFileUI(wxUpdateUIEvent& event)
{
    wxArrayString files;
    clMainFrame::Get()->GetMainBook()->GetRecentlyOpenedFiles(files);
    event.Enable(!files.IsEmpty());
}

void WelcomePage::OnRecentProjectUI(wxUpdateUIEvent& event)
{
    wxArrayString files;
    ManagerST::Get()->GetRecentlyOpenedWorkspaces(files);
    event.Enable(!files.IsEmpty());
}

void WelcomePage::OnThemeChanged(clCommandEvent& e)
{
    e.Skip();
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    GetPanel191()->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    Refresh();
}

void WelcomePage::OnNewWorkspace(wxCommandEvent& event)
{
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("new_workspace"));
    e.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
}
void WelcomePage::OnOpenWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
    e.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
}

void WelcomePage::DoOpenFile(const wxString& filename) { clMainFrame::Get()->GetMainBook()->OpenFile(filename); }

void WelcomePage::AddButtons()
{
    m_cmdLnkBtnNewWorkspace = new clThemedButton(m_panel191, wxID_ANY, _("New Workspace"), _("Create a new workspace"),
                                                 wxDefaultPosition, wxDLG_UNIT(m_panel191, wxSize(-1, -1)), wxBU_LEFT);
    m_cmdLnkBtnNewWorkspace->SetToolTip(_("Create a new workspace"));

    gridSizer629->Add(m_cmdLnkBtnNewWorkspace, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    m_cmdLnkBtnNewWorkspace->SetMinSize(wxSize(-1, 50));

    m_cmdLnkBtnNewProject =
        new clThemedButton(m_panel191, wxID_ANY, _("Open Workspace"), _("Open an existing workspace"),
                           wxDefaultPosition, wxDLG_UNIT(m_panel191, wxSize(-1, -1)), wxBU_LEFT);
    m_cmdLnkBtnNewProject->SetToolTip(_("Click to create a new project.\nIf NO workspace is open, it will auto create "
                                        "a workspace before creating the project"));

    gridSizer629->Add(m_cmdLnkBtnNewProject, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_cmdLnkBtnWorkspaces =
        new clThemedButton(m_panel191, wxID_ANY, _("Recent workspaces"), _("Open a recently used workspace"),
                           wxDefaultPosition, wxDLG_UNIT(m_panel191, wxSize(-1, -1)), wxBU_LEFT);
    m_cmdLnkBtnWorkspaces->SetToolTip(_("Open a workspace from a list of recently opened workspaces"));

    gridSizer629->Add(m_cmdLnkBtnWorkspaces, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_cmdLnkBtnFilesMenu = new clThemedButton(m_panel191, wxID_ANY, _("Recent files"), _("Open a recently opened file"),
                                              wxDefaultPosition, wxDLG_UNIT(m_panel191, wxSize(-1, -1)), wxBU_LEFT);
    m_cmdLnkBtnFilesMenu->SetToolTip(_("Open a file from the recently opened files list"));

    gridSizer629->Add(m_cmdLnkBtnFilesMenu, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_cmdLnkBtnForum = new clThemedButton(m_panel191, wxID_ANY, _("Forums"), _("Visit our forums"), wxDefaultPosition,
                                          wxDLG_UNIT(m_panel191, wxSize(-1, -1)), wxBU_LEFT);
    m_cmdLnkBtnForum->SetToolTip(_("Click to open a web browser in CodeLite's forums"));

    gridSizer629->Add(m_cmdLnkBtnForum, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_cmdLnkBtnWiki = new clThemedButton(m_panel191, wxID_ANY, _("Docs"), _("Browse the documentation"),
                                         wxDefaultPosition, wxDLG_UNIT(m_panel191, wxSize(-1, -1)), wxBU_LEFT);
    m_cmdLnkBtnWiki->SetToolTip(_("Click to open a web browser in CodeLite's documentation page"));

    gridSizer629->Add(m_cmdLnkBtnWiki, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    m_cmdLnkBtnNewWorkspace->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &WelcomePage::OnNewWorkspace, this);
    m_cmdLnkBtnNewProject->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &WelcomePage::OnOpenWorkspace, this);
    m_cmdLnkBtnWorkspaces->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &WelcomePage::OnShowWorkspaceMenu, this);
    m_cmdLnkBtnWorkspaces->Bind(wxEVT_UPDATE_UI, &WelcomePage::OnRecentProjectUI, this);
    m_cmdLnkBtnFilesMenu->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &WelcomePage::OnShowFileseMenu, this);
    m_cmdLnkBtnFilesMenu->Bind(wxEVT_UPDATE_UI, &WelcomePage::OnRecentFileUI, this);
    m_cmdLnkBtnForum->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &WelcomePage::OnOpenForums, this);
    m_cmdLnkBtnWiki->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &WelcomePage::OnOpenWiki, this);
}
