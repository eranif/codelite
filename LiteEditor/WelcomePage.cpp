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
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(WelcomePage::OnThemeChanged), NULL,
                                  this);
    m_staticBitmap->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("codelite-logo", 32));
    m_staticBitmap->Hide();
    m_cmdLnkBtnFilesMenu->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("show_current_line"));
    m_cmdLnkBtnForum->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("show_current_line"));
    m_cmdLnkBtnNewProject->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("show_current_line"));
    m_cmdLnkBtnNewWorkspace->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("show_current_line"));
    m_cmdLnkBtnWiki->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("show_current_line"));
    m_cmdLnkBtnWorkspaces->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("show_current_line"));
    GetSizer()->Fit(this);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
}

WelcomePage::~WelcomePage()
{
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(WelcomePage::OnThemeChanged), NULL,
                                     this);
}

void WelcomePage::OnOpenForums(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ::wxLaunchDefaultBrowser("https://forums.codelite.org/");
}

void WelcomePage::OnOpenWiki(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ::wxLaunchDefaultBrowser("http://wiki.codelite.org");
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
        wxString filename = m_idToName[id];
        clMainFrame::Get()->GetMainBook()->OpenFile(filename);
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

int WelcomePage::DoGetPopupMenuSelection(wxCommandLinkButton* btn, const wxArrayString& strings,
                                         const wxString& menuTitle)
{
    BitmapLoader* loader = PluginManager::Get()->GetStdIcons();

    m_idToName.clear();
    wxUnusedVar(menuTitle);
    wxMenu menu;

    for(size_t i = 0; i < strings.GetCount(); ++i) {

        wxBitmap bmp = loader->GetBitmapForFile("a.txt");
        wxString filename = strings.Item(i);
        if(filename.Find("@") != wxNOT_FOUND) { filename = filename.AfterFirst('@'); }
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

void WelcomePage::OnThemeChanged(wxCommandEvent& e) { e.Skip(); }

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
