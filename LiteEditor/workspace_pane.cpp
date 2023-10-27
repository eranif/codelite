//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : workspace_pane.cpp
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
#include "workspace_pane.h"

#include "Notebook.h"
#include "clTabTogglerHelper.h"
#include "clWorkspaceView.h"
#include "cl_aui_dock_art.h"
#include "cl_config.h"
#include "cl_defs.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include "configuration_manager_dlg.h"
#include "cpp_symbol_tree.h"
#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "dockablepanemenumanager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileexplorer.h"
#include "fileview.h"
#include "frame.h"
#include "globals.h"
#include "macros.h"
#include "manager.h"
#include "openwindowspanel.h"
#include "pluginmanager.h"
#include "tabgroupspane.h"
#include "windowstack.h"
#include "workspacetab.h"

#include <algorithm>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

WorkspacePane::WorkspacePane(wxWindow* parent, const wxString& caption, wxAuiManager* mgr, long style)
    : m_caption(caption)
    , m_mgr(mgr)
{
    if(!wxPanel::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)) {
        return;
    }
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());

    Hide();
    CreateGUIControls();
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &WorkspacePane::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &WorkspacePane::OnSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &WorkspacePane::OnToggleWorkspaceTab, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, [this](clCommandEvent& e) {
        e.Skip();
        SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
        Refresh();
    });
}

WorkspacePane::~WorkspacePane()
{
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &WorkspacePane::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &WorkspacePane::OnSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &WorkspacePane::OnToggleWorkspaceTab, this);
}

#define IS_DETACHED(name) (detachedPanes.Index(name) != wxNOT_FOUND) ? true : false

void WorkspacePane::CreateGUIControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    long style = 0;
    if(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection() == wxRIGHT) {
        style = wxBK_RIGHT;
    }
    m_book = new SidebarBook(this, wxID_ANY, wxDefaultPosition, wxSize(300, -1), style);

    // Calculate the widest tab (the one with the 'Workspace' label)
    int xx, yy;
    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxWindow::GetTextExtent(_("Workspace"), &xx, &yy, NULL, NULL, &fnt);

    mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 0);

    // Add the parsing progress controls
    m_staticText = new wxStaticText(this, wxID_ANY, _("Parsing workspace..."));
    mainSizer->Add(m_staticText, 0, wxEXPAND | wxALL, 2);

    m_parsingProgress =
        new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL | wxGA_SMOOTH);
    mainSizer->Add(m_parsingProgress, 0, wxEXPAND | wxALL, 1);
    m_parsingProgress->Hide();
    m_staticText->Hide();

    // create tabs (possibly detached)
    DetachedPanesInfo dpi;
    EditorConfigST::Get()->ReadObject(wxT("DetachedPanesList"), &dpi);

    wxArrayString detachedPanes;
    detachedPanes = dpi.GetPanes();

    // Add the workspace tab
    wxString name;

    // the IManager instance
    IManager* mgr = PluginManager::Get();

    name = _("Workspace");
    m_workspaceTab = new WorkspaceTab(m_book, name);
    m_book->AddPage(m_workspaceTab, name, clLoadBitmap("workspace-button"), true);

    m_tabs.insert(std::make_pair(name, Tab(name, m_workspaceTab)));
    mgr->AddWorkspaceTab(name);

    // Add the explorer tab
    name = _("Explorer");
    m_explorer = new FileExplorer(m_book, name);
    m_book->AddPage(m_explorer, name, clLoadBitmap("file-explorer-button"), false);

    m_tabs.insert(std::make_pair(name, Tab(name, m_explorer)));
    mgr->AddWorkspaceTab(name);

    // Add the "File Explorer" view to the list of files managed by the workspace-view
    m_workspaceTab->GetView()->AddPage(m_explorer, _("File Explorer"), false);

    // Add the Open Windows Panel (Tabs)
    // #ifndef __WXOSX__
    name = _("Tabs");
    m_openWindowsPane = new OpenWindowsPanel(m_book, name);
    m_book->AddPage(m_openWindowsPane, name, clLoadBitmap("tabs-button"));

    m_tabs.insert(std::make_pair(name, Tab(name, m_openWindowsPane)));
    mgr->AddWorkspaceTab(name);
    // #endif

    // Add the Tabgroups tab
    name = _("Groups");
    m_TabgroupsPane = new TabgroupsPane(m_book, name);
    m_book->AddPage(m_TabgroupsPane, name, clLoadBitmap("groups-button"));

    m_tabs.insert(std::make_pair(name, Tab(name, m_TabgroupsPane)));
    mgr->AddWorkspaceTab(name);

    if(m_book->GetPageCount() > 0) {
        m_book->SetSelection((size_t)0);
    }
    m_mgr->Update();
}

void WorkspacePane::ClearProgress()
{
    m_parsingProgress->SetValue(0);
    m_parsingProgress->Hide();

    m_staticText->SetLabel(_("Parsing workspace..."));
    m_staticText->Hide();
    Layout();
}

void WorkspacePane::UpdateProgress(int val)
{
    if(m_parsingProgress->IsShown() == false) {
        m_parsingProgress->Show();
        m_staticText->Show();
        Layout();
    }

    m_staticText->SetLabel(wxString::Format(_("Parsing workspace: %d%% completed"), val));
    m_parsingProgress->SetValue(val);
    m_parsingProgress->Update();
}

typedef struct _tagTabInfo {
    wxString text;
    wxWindow* win = nullptr;
    wxBitmap bmp;
    bool selected = false;
} tagTabInfo;

void WorkspacePane::ApplySavedTabOrder(bool update_ui) const
{
    return;
    wxWindowUpdateLocker locker{ m_book };
    wxArrayString tabs;
    int index = -1;
    if(!clConfig::Get().GetWorkspaceTabOrder(tabs, index))
        return;

    std::vector<tagTabInfo> tabs_in_order;
    for(const wxString& tab : tabs) {
        tagTabInfo ti;
        int pos = m_book->GetPageIndex(tab);
        if(pos == wxNOT_FOUND) {
            clWARNING() << "error while restoring tab order. could not locate tab:" << tab << endl;
            continue;
        }
        ti.selected = (m_book->GetSelection() == pos);
        ti.text = tab;
        ti.win = m_book->GetPage(pos);
        ti.bmp = m_book->GetPageBitmap(pos);
        tabs_in_order.push_back(ti);
    }

    // Remove all buttons (this does not delete them)
    m_book->RemoveAll();

    // Add re-add them in-order
    for(auto& d : tabs_in_order) {
        m_book->AddPage(d.win, d.text, d.bmp, d.selected);
    }

    if(update_ui) {
        m_mgr->Update();
    }
}

void WorkspacePane::SaveWorkspaceViewTabOrder() const
{
    wxArrayString panes;
    panes.reserve(m_book->GetPageCount());

    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        panes.Add(m_book->GetPageText(i));
    }
    clConfig::Get().SetWorkspaceTabOrder(panes, m_book->GetSelection());
}

void WorkspacePane::DoShowTab(bool show, const wxString& title)
{
    wxUnusedVar(show);
    wxUnusedVar(title);
}

wxWindow* WorkspacePane::DoGetControlByName(const wxString& title)
{
    if(title == _("Explorer"))
        return m_explorer;
    else if(title == _("Workspace"))
        return m_workspaceTab;
#ifndef __WXOSX__
    else if(title == _("Tabs"))
        return m_openWindowsPane;
#endif
    else if(title == _("Groups"))
        return m_TabgroupsPane;
    return NULL;
}

bool WorkspacePane::IsTabVisible(int flag)
{
    wxWindow* win(NULL);
    wxString title;

    switch(flag) {
    case View_Show_Workspace_Tab:
        title = _("Workspace");
        win = DoGetControlByName(_("Workspace"));
        break;
    case View_Show_Explorer_Tab:
        title = _("Explorer");
        win = DoGetControlByName(_("Explorer"));
        break;
#ifndef __WXOSX__
    case View_Show_Tabs_Tab:
        title = _("Tabs");
        win = DoGetControlByName(_("Tabs"));
        break;
#endif
    case View_Show_Tabgroups_Tab:
        title = _("Groups");
        win = DoGetControlByName(_("Groups"));
        break;
    }

    if(!win || title.IsEmpty())
        return false;

    // if the control exists in the notebook, return true
    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if(m_book->GetPageText(i) == title) {
            return true;
        }
    }
    return win && win->IsShown();
}

void WorkspacePane::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    m_captionEnabler.Initialize(this, "Workspace View", &clMainFrame::Get()->GetDockingManager());
}

void WorkspacePane::SelectTab(const wxString& tabTitle)
{
    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        if(m_book->GetPageText(i) == tabTitle) {
            // requested to add a page which already exists
            m_book->SetSelection(i);
        }
    }
}

void WorkspacePane::OnSettingsChanged(wxCommandEvent& event)
{
    event.Skip();

    auto direction = EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection();
    m_book->SetOrientationOnTheRight(direction == wxRIGHT);
}

void WorkspacePane::OnToggleWorkspaceTab(clCommandEvent& event)
{
    // Handle the core tabs
    wxUnusedVar(event);
}

clTabRenderer::Ptr_t WorkspacePane::GetNotebookRenderer() { return clTabRenderer::Ptr_t{}; }

void WorkspacePane::OnNativeBookContextMenu(wxContextMenuEvent& event) { wxUnusedVar(event); }

void WorkspacePane::OnWorkspaceBookFileListMenu(clContextMenuEvent& event) { wxUnusedVar(event); }

void WorkspacePane::ShowTab(const wxString& name, bool show)
{
    clCommandEvent show_event(wxEVT_SHOW_WORKSPACE_TAB);
    show_event.SetString(name);
    show_event.SetSelected(show);
    EventNotifier::Get()->ProcessEvent(show_event);
}

bool WorkspacePane::BuildTabListMenu(wxMenu& menu)
{
    DetachedPanesInfo dpi;
    EditorConfigST::Get()->ReadObject("DetachedPanesList", &dpi);

    wxMenu* hiddenTabsMenu = new wxMenu();
    const wxArrayString& tabs = clGetManager()->GetWorkspaceTabs();
    for(size_t i = 0; i < tabs.size(); ++i) {
        const wxString& label = tabs.Item(i);
        if((m_book->GetPageIndex(label) != wxNOT_FOUND)) {
            // Tab is visible, dont show it
            continue;
        }

        if(menu.GetMenuItemCount() > 0 && hiddenTabsMenu->GetMenuItemCount() == 0) {
            // we are adding the first menu item
            menu.AppendSeparator();
        }

        int tabId = wxXmlResource::GetXRCID(wxString() << "workspace_tab_" << label);
        hiddenTabsMenu->Append(tabId, label);

        // If the tab is detached, disable it's menu entry
        if(dpi.GetPanes().Index(label) != wxNOT_FOUND) {
            hiddenTabsMenu->Enable(tabId, false);
        }

        // Bind the event
        hiddenTabsMenu->Bind(
            wxEVT_MENU,
            // Use lambda by value here so we make a copy
            [=](wxCommandEvent& e) {
                clCommandEvent eventShow(wxEVT_SHOW_WORKSPACE_TAB);
                eventShow.SetSelected(true).SetString(label);
                EventNotifier::Get()->AddPendingEvent(eventShow);
            },
            tabId);
    }
    if(hiddenTabsMenu->GetMenuItemCount() == 0) {
        wxDELETE(hiddenTabsMenu);
        return false;
    } else {
        menu.AppendSubMenu(hiddenTabsMenu, _("Hidden Tabs"), _("Hidden Tabs"));
        return true;
    }
}
