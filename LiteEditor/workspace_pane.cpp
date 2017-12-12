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
#include "Notebook.h"
#include "clTabRendererClassic.h"
#include "clTabRendererCurved.h"
#include "clTabRendererSquare.h"
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
#include "macros.h"
#include "manager.h"
#include "openwindowspanel.h"
#include "parse_thread.h"
#include "pluginmanager.h"
#include "tabgroupspane.h"
#include "windowstack.h"
#include "workspace_pane.h"
#include "workspacetab.h"
#include <algorithm>
#include <wx/app.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

WorkspacePane::WorkspacePane(wxWindow* parent, const wxString& caption, wxAuiManager* mgr)
    : wxPanel(parent)
    , m_caption(caption)
    , m_mgr(mgr)
{
    CreateGUIControls();
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &WorkspacePane::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &WorkspacePane::OnSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &WorkspacePane::OnToggleWorkspaceTab, this);
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

#if USE_AUI_NOTEBOOK
    long style = wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TAB_SPLIT;
#else
// add notebook for tabs
#ifdef __WXOSX__
    long style = (kNotebook_Default | kNotebook_AllowDnD | kNotebook_LeftTabs);
#else
    long style = (kNotebook_Default | kNotebook_AllowDnD);
#endif
    if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        style &= ~kNotebook_LightTabs;
        style |= kNotebook_DarkTabs;
    }
    style |= kNotebook_UnderlineActiveTab;
    if(EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs()) { style |= kNotebook_MouseScrollSwitchTabs; }
#endif

    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    m_book->SetTabDirection(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection());

#if !USE_AUI_NOTEBOOK
    m_book->SetArt(GetNotebookRenderer());
#endif

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
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
        m_workspaceTab = new WorkspaceTab(cp, name);
        cp->SetChildNoReparent(m_workspaceTab);
    } else {
        m_workspaceTab = new WorkspaceTab(m_book, name);
        m_book->AddPage(m_workspaceTab, name, true, wxNullBitmap);
    }
    m_tabs.insert(std::make_pair(name, Tab(name, m_workspaceTab)));
    mgr->AddWorkspaceTab(name);

    // Add the explorer tab
    name = _("Explorer");
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
        m_explorer = new FileExplorer(cp, name);
        cp->SetChildNoReparent(m_explorer);
    } else {
        m_explorer = new FileExplorer(m_book, name);
        m_book->AddPage(m_explorer, name, false);
    }
    m_tabs.insert(std::make_pair(name, Tab(name, m_explorer)));
    mgr->AddWorkspaceTab(name);

    // Add the "File Explorer" view to the list of files managed by the workspace-view
    m_workspaceTab->GetView()->AddPage(m_explorer, _("File Explorer"), false);

// Add the Open Windows Panel (Tabs)
#ifndef __WXOSX__
    name = _("Tabs");
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
        m_openWindowsPane = new OpenWindowsPanel(cp, name);
        cp->SetChildNoReparent(m_openWindowsPane);
    } else {
        m_openWindowsPane = new OpenWindowsPanel(m_book, name);
        m_book->AddPage(m_openWindowsPane, name, false);
    }
    m_tabs.insert(std::make_pair(name, Tab(name, m_openWindowsPane)));
    mgr->AddWorkspaceTab(name);
#endif

    // Add the Tabgroups tab
    name = _("Tabgroups");
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
        m_TabgroupsPane = new TabgroupsPane(cp, name);
        cp->SetChildNoReparent(m_TabgroupsPane);
    } else {
        m_TabgroupsPane = new TabgroupsPane(m_book, name);
        m_book->AddPage(m_TabgroupsPane, name, false);
    }
    m_tabs.insert(std::make_pair(name, Tab(name, m_TabgroupsPane)));
    mgr->AddWorkspaceTab(name);

    if(m_book->GetPageCount() > 0) { m_book->SetSelection((size_t)0); }
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

typedef struct {
    wxString text;
    wxWindow* win;
    wxBitmap bmp;
} tagTabInfo;

#include "file_logger.h"
void WorkspacePane::ApplySavedTabOrder() const
{

    wxArrayString tabs;
    int index = -1;
    if(!clConfig::Get().GetWorkspaceTabOrder(tabs, index)) return;

    // There are (currently) 4 'standard' panes and a variable number of plugin ones
    // NB Since we're only dealing with panes currently in the notebook, this shouldn't
    // be broken by floating panes or non-loaded plugins
    std::vector<tagTabInfo> vTempstore;
    for(size_t t = 0; t < tabs.GetCount(); ++t) {
        wxString title = tabs.Item(t);
        if(title.empty()) { continue; }
        for(size_t n = 0; n < m_book->GetPageCount(); ++n) {
            if(title == m_book->GetPageText(n)) {
                tagTabInfo Tab;
                Tab.text = title;
                Tab.win = m_book->GetPage(n);
                Tab.bmp = m_book->GetPageBitmap(n);

                vTempstore.push_back(Tab);
                m_book->RemovePage(n);
                break;
            }
        }
        // If we reach here without finding title, presumably that tab is no longer available and will just be ignored
    }

    // All the matched tabs are now stored in the vector. Any left in m_book are presumably new additions
    // Now prepend the ordered tabs, so that any additions will effectively be appended
    for(size_t n = 0; n < vTempstore.size(); ++n) {
        m_book->InsertPage(n, vTempstore.at(n).win, vTempstore.at(n).text, false, vTempstore.at(n).bmp);
    }

    // wxPrintf("After load");for (size_t n=0; n < m_book->GetPageCount(); ++n)  CL_DEBUG1(wxString::Format("Tab %i:
    // %zs",(int)n,m_book->GetPageText(n)));

    // Restore any saved last selection
    // NB: this doesn't actually work atm: the selection is set correctly, but presumably something else changes is
    // later
    // I've left the code in case anyone ever has time/inclination to fix it
    if((index >= 0) && (index < (int)m_book->GetPageCount())) {
        m_book->SetSelection(index);
    } else if(m_book->GetPageCount()) {
        m_book->SetSelection(0);
    }
    m_mgr->Update();
}

void WorkspacePane::SaveWorkspaceViewTabOrder() const
{
#if USE_AUI_NOTEBOOK
    wxArrayString panes = m_book->GetAllTabsLabels();
#else
    wxArrayString panes;
    clTabInfo::Vec_t tabs;
    m_book->GetAllTabs(tabs);
    std::for_each(tabs.begin(), tabs.end(), [&](clTabInfo::Ptr_t t) { panes.Add(t->GetLabel()); });
#endif
    clConfig::Get().SetWorkspaceTabOrder(panes, m_book->GetSelection());
}

void WorkspacePane::DoShowTab(bool show, const wxString& title)
{
    if(!show) {
        for(size_t i = 0; i < m_book->GetPageCount(); i++) {
            if(m_book->GetPageText(i) == title) {
                // we've got a match
                m_book->RemovePage(i);
                wxWindow* win = DoGetControlByName(title);
                if(win) { win->Show(false); }
                break;
            }
        }
    } else {
        for(size_t i = 0; i < m_book->GetPageCount(); i++) {
            if(m_book->GetPageText(i) == title) {
                // requested to add a page which already exists
                return;
            }
        }

        // Fetch the list of detached panes
        // If the mainframe is NULL, read the
        // list from the disk, otherwise use the
        // dockable pane menu

        // Read it from the disk
        DetachedPanesInfo dpi;
        EditorConfigST::Get()->ReadObject(wxT("DetachedPanesList"), &dpi);
        wxArrayString detachedPanes;
        detachedPanes = dpi.GetPanes();

        if(IS_DETACHED(title)) return;

        wxWindow* win = DoGetControlByName(title);
        if(win) {
            win->Show(true);
            m_book->InsertPage(0, win, title, true);
        }
    }
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
    else if(title == _("Tabgroups"))
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
        title = _("Tabgroups");
        win = DoGetControlByName(_("Tabgroups"));
        break;
    }

    if(!win || title.IsEmpty()) return false;

    // if the control exists in the notebook, return true
    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        if(m_book->GetPageText(i) == title) { return true; }
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
    m_book->SetTabDirection(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection());
#if !USE_AUI_NOTEBOOK
    m_book->SetArt(GetNotebookRenderer());
#endif
    if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        m_book->SetStyle((m_book->GetStyle() & ~kNotebook_LightTabs) | kNotebook_DarkTabs);
    } else {
        m_book->SetStyle((m_book->GetStyle() & ~kNotebook_DarkTabs) | kNotebook_LightTabs);
    }
}

void WorkspacePane::OnToggleWorkspaceTab(clCommandEvent& event)
{
    // Handle the core tabs
    if(m_tabs.count(event.GetString()) == 0) {
        event.Skip();
        return;
    }

    const Tab& t = m_tabs.find(event.GetString())->second;
    if(event.IsSelected()) {
        // Insert the page
        int where = clTabTogglerHelper::IsTabInNotebook(GetNotebook(), t.m_label);
        if(where == wxNOT_FOUND) {
            GetNotebook()->AddPage(t.m_window, t.m_label, false, t.m_bmp);
        } else {
            GetNotebook()->SetSelection(where);
        }
    } else {
        // hide the tab
        int where = GetNotebook()->GetPageIndex(t.m_label);
        if(where != wxNOT_FOUND) { GetNotebook()->RemovePage(where); }
    }
}

#if !USE_AUI_NOTEBOOK
clTabRenderer::Ptr_t WorkspacePane::GetNotebookRenderer()
{
    if(m_book->GetStyle() & kNotebook_RightTabs || m_book->GetStyle() & kNotebook_LeftTabs) {
        // Vertical tabs, change the art provider to use the square shape
        return clTabRenderer::Ptr_t(new clTabRendererSquare());
    } else {
        // Else, use the settings
        size_t options = EditorConfigST::Get()->GetOptions()->GetOptions();
        if(options & OptionsConfig::Opt_TabStyleMinimal) {
            return clTabRenderer::Ptr_t(new clTabRendererSquare);
        } else if(options & OptionsConfig::Opt_TabStyleTRAPEZOID) {
            return clTabRenderer::Ptr_t(new clTabRendererCurved());
        } else {
            // the default
            return clTabRenderer::Ptr_t(new clTabRendererClassic);
        }
    }
}
#endif
