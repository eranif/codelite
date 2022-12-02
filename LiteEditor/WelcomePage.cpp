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
#include "imanager.h"
#include "macros.h"
#include "manager.h"
#include "plugin.h"
#include "pluginmanager.h"
#include "window_locker.h"
#include "wxStringHash.h"

#include <wx/arrstr.h>
#include <wx/clntdata.h>

namespace
{
void get_caption_colours(wxColour* bg_colour, wxColour* text_colour)
{
    *bg_colour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    *text_colour = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
}
} // namespace

WelcomePage::WelcomePage(wxWindow* parent)
    : WelcomePageBase(parent)
{
#ifndef __WXMAC__
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    GetMainPanel()->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
#endif

    GetSizer()->Fit(this);
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, &WelcomePage::OnThemeChanged, this);

    m_dvTreeCtrlWorkspaces->AddHeader(_("Name"));
    m_dvTreeCtrlWorkspaces->AddHeader(_("Type"));
    m_dvTreeCtrlWorkspaces->AddHeader(_("Path"));

    m_dvTreeCtrlWorkspaces->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());

    long curstyle = m_dvTreeCtrlWorkspaces->GetTreeStyle();
    curstyle |= wxTR_HIDE_ROOT;
    curstyle &= ~wxTR_FULL_ROW_HIGHLIGHT;

    m_dvTreeCtrlWorkspaces->SetTreeStyle(curstyle);
    m_dvTreeCtrlWorkspaces->SetShowHeader(false);
    m_dvTreeCtrlWorkspaces->AddRoot(_("Workspaces"));

    m_dvTreeCtrlWorkspaces->SetSortFunction(nullptr);

    // create the buttons
    m_buttonOpenWorkspace = new clThemedButton(m_buttonsPage, wxID_ANY, _("Open"), _("Open an existing workspace"),
                                               wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
    m_buttonNewWorkspace = new clThemedButton(m_buttonsPage, wxID_ANY, _("New"), _("Create a new workspace"),
                                              wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
    m_buttonGithub = new clThemedButton(m_buttonsPage, wxID_ANY, _("GitHub"), _("Visit our GitHub project page"),
                                        wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
    m_buttonGitter = new clThemedButton(m_buttonsPage, wxID_ANY, _("Gitter"), _("Join the chat!"), wxDefaultPosition,
                                        wxDefaultSize, wxBU_LEFT);

    // events
    m_buttonOpenWorkspace->Bind(wxEVT_BUTTON, &WelcomePage::OnOpenWorkspace, this);
    m_buttonNewWorkspace->Bind(wxEVT_BUTTON, &WelcomePage::OnNewWorkspace, this);
    m_buttonGithub->Bind(wxEVT_BUTTON, &WelcomePage::OnGitHHub, this);
    m_buttonGitter->Bind(wxEVT_BUTTON, &WelcomePage::OnGitter, this);

    buttons_sizer->Add(m_buttonOpenWorkspace, wxSizerFlags(0).Expand().Border(wxALL, 5));
    buttons_sizer->Add(m_buttonNewWorkspace, wxSizerFlags(0).Expand().Border(wxALL, 5));
    buttons_sizer->Add(m_buttonGithub, wxSizerFlags(0).Expand().Border(wxALL, 5));
    buttons_sizer->Add(m_buttonGitter, wxSizerFlags(0).Expand().Border(wxALL, 5));

    UpdateRecentWorkspaces();
#if CL_USE_NATIVEBOOK
    Show();
#endif
    GetSizer()->Layout();
    // set the focus to the tree
    m_dvTreeCtrlWorkspaces->CallAfter(&clTreeCtrl::SetFocus);

    wxColour bg_colour, text_colour;
    get_caption_colours(&bg_colour, &text_colour);

    m_panelList->SetBackgroundColour(bg_colour);
    m_panelList->SetForegroundColour(text_colour);
    m_staticText0->SetForegroundColour(text_colour);
}

WelcomePage::~WelcomePage()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &WelcomePage::OnThemeChanged, this);
}

void WelcomePage::OnSize(wxSizeEvent& event) { event.Skip(); }

void WelcomePage::OnThemeChanged(clCommandEvent& e)
{
    e.Skip();
    wxColour bg_colour, text_colour;
    get_caption_colours(&bg_colour, &text_colour);

    m_panelList->SetBackgroundColour(bg_colour);
    m_panelList->SetForegroundColour(text_colour);
    m_staticText0->SetForegroundColour(text_colour);

#ifndef __WXMAC__
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    GetMainPanel()->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    GetDvTreeCtrlWorkspaces()->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    GetDvTreeCtrlWorkspaces()->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());
    Refresh();
#endif
}

void WelcomePage::OnNewWorkspace(wxCommandEvent& event)
{
    wxCommandEvent e(wxEVT_MENU, XRCID("new_workspace"));
    e.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
}

void WelcomePage::OnOpenWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent e(wxEVT_MENU, XRCID("switch_to_workspace"));
    e.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
}

// void WelcomePage::DoOpenFile(const wxString& filename) { clMainFrame::Get()->GetMainBook()->OpenFile(filename); }

void WelcomePage::UpdateRecentWorkspaces()
{
    m_dvTreeCtrlWorkspaces->Begin();
    m_dvTreeCtrlWorkspaces->DeleteChildren(m_dvTreeCtrlWorkspaces->GetRootItem());

    // get the recently opened workspaces (local)
    wxArrayString files;
    ManagerST::Get()->GetRecentlyOpenedWorkspaces(files);

    // request workspaces from the plugins
    clRecentWorkspaceEvent other_workspaces_event{ wxEVT_RECENT_WORKSPACE };
    EventNotifier::Get()->ProcessEvent(other_workspaces_event);

    // TODO: fire event here to collect other workspaces as well
    auto locals = m_dvTreeCtrlWorkspaces->AppendItem(m_dvTreeCtrlWorkspaces->GetRootItem(), _("Local workspaces"));
    int image_index = wxNOT_FOUND;
    for(const wxString& filepath : files) {
        wxFileName fn(filepath);
        wxString workspace_type;
        auto type = FileExtManager::GetType(fn.GetFullPath());
        image_index = clGetManager()->GetStdIcons()->GetMimeImageId(type);
        switch(type) {
        case FileExtManager::TypeWorkspaceFileSystem:
            workspace_type = _("File System");
            break;
        case FileExtManager::TypeWorkspaceDocker:
            workspace_type = _("Docker");
            break;
        case FileExtManager::TypeWorkspaceNodeJS:
            workspace_type = _("NodeJS");
            break;
        case FileExtManager::TypeWorkspacePHP:
            workspace_type = _("PHP");
            break;
        case FileExtManager::TypeWorkspace:
            workspace_type = _("C/C++");
            break;
        default:
            workspace_type = _("Other");
            image_index = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeWorkspaceFileSystem);
            break;
        }

        auto cd = new WelcomePageItemData();
        cd->type = WorkspaceSource::BUILTIN;
        cd->path = fn.GetFullPath();
        auto workspaceItem = m_dvTreeCtrlWorkspaces->AppendItem(locals, fn.GetName(), image_index, image_index, cd);
        m_dvTreeCtrlWorkspaces->SetItemText(workspaceItem, workspace_type, 1);
        m_dvTreeCtrlWorkspaces->SetItemText(workspaceItem, filepath, 2);
    }
    m_dvTreeCtrlWorkspaces->Expand(locals);

    // add the plugin base workspaces
    auto other_workspaces = other_workspaces_event.GetWorkspaces();
    std::unordered_map<wxString, std::vector<RecentWorkspace>> M;
    for(const auto& e : other_workspaces) {
        if(M.count(e.m_category) == 0) {
            M.insert({ e.m_category, {} });
        }
        M[e.m_category].push_back(e);
    }

    image_index = clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeWorkspaceFileSystem);
    for(const auto& vt : M) {
        const wxString& category = vt.first;
        auto parent_item = m_dvTreeCtrlWorkspaces->AppendItem(m_dvTreeCtrlWorkspaces->GetRootItem(), category);
        for(const auto& w : vt.second) {
            auto cd = new WelcomePageItemData();
            cd->path = w.path;
            cd->account = w.m_account;
            cd->type = WorkspaceSource::PLUGIN;

            wxString name = cd->path;
            name.Replace("\\", "/");
            name = name.AfterLast('/');
            name = name.BeforeLast('.');

            auto workspaceItem = m_dvTreeCtrlWorkspaces->AppendItem(parent_item, name, image_index, image_index, cd);
            m_dvTreeCtrlWorkspaces->SetItemText(workspaceItem, _("Other"), 1);
            m_dvTreeCtrlWorkspaces->SetItemText(workspaceItem, w.path, 2);
        }
        m_dvTreeCtrlWorkspaces->Expand(parent_item);
    }
    m_dvTreeCtrlWorkspaces->Commit();
}

void WelcomePage::OnWorkspaceActivated(wxTreeEvent& event)
{
    event.Skip();
    auto cd = GetWorkspaceItemData(event.GetItem());
    CHECK_PTR_RET(cd);

    switch(cd->type) {
    case WorkspaceSource::BUILTIN:
        OpenBuiltinWorkspace(cd);
        break;
    case WorkspaceSource::PLUGIN:
        OpenPluginWorkspace(cd);
        break;
    }
}

WelcomePageItemData* WelcomePage::GetWorkspaceItemData(const wxTreeItemId& item)
{
    CHECK_ITEM_RET_NULL(item);

    auto cd = m_dvTreeCtrlWorkspaces->GetItemData(item);
    CHECK_PTR_RET_NULL(cd);

    return dynamic_cast<WelcomePageItemData*>(cd);
}

void WelcomePage::OpenBuiltinWorkspace(WelcomePageItemData* cd)
{
    // post an event to the main frame requesting a workspace open
    wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("switch_to_workspace"));
    evtOpenworkspace.SetString(cd->path);
    evtOpenworkspace.SetEventObject(clMainFrame::Get());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
}

void WelcomePage::OpenPluginWorkspace(WelcomePageItemData* cd)
{
    clWorkspaceEvent open_event{ wxEVT_WORKSPACE_PLUGIN_OPEN };
    open_event.SetIsRemote(!cd->account.empty());
    open_event.SetFileName(cd->path);
    open_event.SetRemoteAccount(cd->account);
    EventNotifier::Get()->AddPendingEvent(open_event);
}

void WelcomePage::OnGitHHub(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxLaunchDefaultBrowser("https://github.com/eranif/codelite");
}

void WelcomePage::OnGitter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxLaunchDefaultBrowser("https://gitter.im/eranif/codelite");
}

bool WelcomePage::Show(bool show)
{
    UpdateRecentWorkspaces();
    return wxPanel::Show(show);
}