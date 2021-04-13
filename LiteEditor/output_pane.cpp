//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : output_pane.cpp
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
#include "clStrings.h"
#include "clTabTogglerHelper.h"
#include "detachedpanesinfo.h"
#include "dockablepanemenumanager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "findresultstab.h"
#include "findusagetab.h"
#include "frame.h"
#include "new_build_tab.h"
#include "output_pane.h"
#include "pluginmanager.h"
#include "replaceinfilespanel.h"
#include "shelltab.h"
#include "taskpanel.h"
#include <algorithm>
#include <wx/aui/framemanager.h>
#include <wx/dcbuffer.h>
#include <wx/xrc/xmlres.h>

OutputPane::OutputPane(wxWindow* parent, const wxString& caption)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 250))
    , m_caption(caption)
    , m_buildInProgress(false)
{
    CreateGUIControls();
    EventNotifier::Get()->Connect(wxEVT_EDITOR_CLICKED, wxCommandEventHandler(OutputPane::OnEditorFocus), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTED, clBuildEventHandler(OutputPane::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_ENDED, clBuildEventHandler(OutputPane::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &OutputPane::OnSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_SHOW_OUTPUT_TAB, &OutputPane::OnToggleTab, this);
    SetSize(-1, 250);
}

OutputPane::~OutputPane()
{
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLICKED, wxCommandEventHandler(OutputPane::OnEditorFocus), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTED, clBuildEventHandler(OutputPane::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_ENDED, clBuildEventHandler(OutputPane::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &OutputPane::OnSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_SHOW_OUTPUT_TAB, &OutputPane::OnToggleTab, this);
}

void OutputPane::CreateGUIControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);
    SetMinClientSize(wxSize(-1, 250));
    long style = (kNotebook_Default | kNotebook_AllowDnD);
    if(EditorConfigST::Get()->GetOptions()->GetOutputTabsDirection() == wxBOTTOM) {
        style |= kNotebook_BottomTabs;
    } else if(EditorConfigST::Get()->GetOptions()->GetOutputTabsDirection() == wxLEFT) {
#ifdef __WXOSX__
        style &= ~(kNotebook_BottomTabs | kNotebook_LeftTabs | kNotebook_RightTabs);
#else
        style |= kNotebook_LeftTabs;
#endif
    } else if(EditorConfigST::Get()->GetOptions()->GetOutputTabsDirection() == wxRIGHT) {
#ifdef __WXOSX__
        style |= kNotebook_BottomTabs;
#else
        style |= kNotebook_RightTabs;
#endif
    }
    style |= kNotebook_UnderlineActiveTab;
    if(EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs()) {
        style |= kNotebook_MouseScrollSwitchTabs;
    }
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    m_book->Bind(wxEVT_BOOK_FILELIST_BUTTON_CLICKED, &OutputPane::OnOutputBookFileListMenu, this);

    // Calculate the widest tab (the one with the 'Workspace' label) TODO: What happens with translations?
    mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 0);

    // the IManager instance
    IManager* mgr = PluginManager::Get();

    // Build tab
    m_buildWin = new NewBuildTab(m_book);
    auto images = m_book->GetBitmaps();
#if PHP_BUILD
    m_buildWin->Hide();
#else
    m_book->AddPage(m_buildWin, BUILD_WIN, true, images->Add(wxT("build")));
    m_tabs.insert(
        std::make_pair(BUILD_WIN, Tab(BUILD_WIN, m_buildWin, wxNOT_FOUND)));
    mgr->AddOutputTab(BUILD_WIN);
#endif

    // Find in files
    m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, FIND_IN_FILES_WIN);
    m_book->AddPage(m_findResultsTab, FIND_IN_FILES_WIN, false, images->Add(wxT("find")));
    m_tabs.insert(std::make_pair(FIND_IN_FILES_WIN,
                                 Tab(FIND_IN_FILES_WIN, m_findResultsTab, images->Add(wxT("find")))));
    mgr->AddOutputTab(FIND_IN_FILES_WIN);

    // Replace In Files
    m_replaceResultsTab = new ReplaceInFilesPanel(m_book, wxID_ANY, REPLACE_IN_FILES);
    m_book->AddPage(m_replaceResultsTab, REPLACE_IN_FILES, false,
                    images->Add(wxT("find_and_replace")));
    m_tabs.insert(std::make_pair(REPLACE_IN_FILES,
                                 Tab(REPLACE_IN_FILES, m_replaceResultsTab, images->Add(wxT("find_and_replace")))));
    mgr->AddOutputTab(REPLACE_IN_FILES);

    // Show Usage ("References")
    m_showUsageTab = new FindUsageTab(m_book, SHOW_USAGE);
#if PHP_BUILD
    m_showUsageTab->Hide();
#else
    m_book->AddPage(m_showUsageTab, SHOW_USAGE, false, images->Add(wxT("find")));
    m_tabs.insert(std::make_pair(SHOW_USAGE,
                                 Tab(SHOW_USAGE, m_showUsageTab, images->Add(wxT("find")))));
    mgr->AddOutputTab(SHOW_USAGE);
#endif
    // Output tab
    m_outputWind = new OutputTab(m_book, wxID_ANY, OUTPUT_WIN);
    m_book->AddPage(m_outputWind, OUTPUT_WIN, false, images->Add(wxT("console")));
    m_tabs.insert(std::make_pair(OUTPUT_WIN,
                                 Tab(OUTPUT_WIN, m_outputWind, images->Add(wxT("console")))));
    mgr->AddOutputTab(OUTPUT_WIN);

    // Tasks panel
    m_taskPanel = new TaskPanel(m_book, wxID_ANY, TASKS);
    m_book->AddPage(m_taskPanel, TASKS, false, images->Add("tasks"));
    m_tabs.insert(
        std::make_pair(TASKS, Tab(TASKS, m_taskPanel, images->Add("tasks"))));
    mgr->AddOutputTab(TASKS);

    SetMinSize(wxSize(200, 100));
    mainSizer->Layout();
}

void OutputPane::OnEditorFocus(wxCommandEvent& e)
{
    e.Skip();
    if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {

        // Optionally don't hide the various panes (sometimes it's irritating, you click to do something and...)
        int cursel(m_book->GetSelection());
        if(cursel != wxNOT_FOUND && EditorConfigST::Get()->GetPaneStickiness(m_book->GetPageText(cursel))) {
            return;
        }

        if(m_buildInProgress)
            return;

        wxAuiPaneInfo& info = PluginManager::Get()->GetDockingManager()->GetPane(wxT("Output View"));
        DockablePaneMenuManager::HackHidePane(true, info, PluginManager::Get()->GetDockingManager());
    }
}

void OutputPane::OnBuildStarted(clBuildEvent& e)
{
    e.Skip();
    m_buildInProgress = true;
}

void OutputPane::OnBuildEnded(clBuildEvent& e)
{
    e.Skip();
    m_buildInProgress = false;
}

void OutputPane::SaveTabOrder()
{
#if USE_AUI_NOTEBOOK
    wxArrayString panes = m_book->GetAllTabsLabels();
#else
    wxArrayString panes;
    clTabInfo::Vec_t tabs;
    m_book->GetAllTabs(tabs);
    std::for_each(tabs.begin(), tabs.end(), [&](clTabInfo::Ptr_t t) { panes.Add(t->GetLabel()); });
#endif
    clConfig::Get().SetOutputTabOrder(panes, m_book->GetSelection());
}

typedef struct {
    wxString text;
    wxWindow* win = nullptr;
    int bmp = wxNOT_FOUND;
} tagTabInfo;

void OutputPane::ApplySavedTabOrder() const
{

    wxArrayString tabs;
    int index = -1;
    if(!clConfig::Get().GetOutputTabOrder(tabs, index))
        return;

    std::vector<tagTabInfo> vTempstore;
    for(size_t t = 0; t < tabs.GetCount(); ++t) {
        wxString title = tabs.Item(t);
        if(title.empty()) {
            continue;
        }
        for(size_t n = 0; n < m_book->GetPageCount(); ++n) {
            if(title == m_book->GetPageText(n)) {
                tagTabInfo Tab;
                Tab.text = title;
                Tab.win = m_book->GetPage(n);
                Tab.bmp = m_book->GetPageBitmapIndex(n);

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
}

void OutputPane::OnSettingsChanged(wxCommandEvent& event)
{
    event.Skip();
    m_book->SetTabDirection(EditorConfigST::Get()->GetOptions()->GetOutputTabsDirection());
}

void OutputPane::OnToggleTab(clCommandEvent& event)
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
            GetNotebook()->AddPage(t.m_window, t.m_label, true, t.m_bmpIndex);
        } else {
            GetNotebook()->SetSelection(where);
        }
    } else {
        // hide the tab
        int where = GetNotebook()->GetPageIndex(t.m_label);
        if(where != wxNOT_FOUND) {
            GetNotebook()->RemovePage(where);
        }
    }
}

void OutputPane::OnOutputBookFileListMenu(clContextMenuEvent& event)
{
    wxMenu* menu = event.GetMenu();

    DetachedPanesInfo dpi;
    EditorConfigST::Get()->ReadObject("DetachedPanesList", &dpi);

    wxMenu* hiddenTabsMenu = new wxMenu();
    const wxArrayString& tabs = clGetManager()->GetOutputTabs();
    for(size_t i = 0; i < tabs.size(); ++i) {
        const wxString& label = tabs.Item(i);
        if((m_book->GetPageIndex(label) != wxNOT_FOUND)) {
            // Tab is visible, dont show it
            continue;
        }

        if(hiddenTabsMenu->GetMenuItemCount() == 0) {
            // we are adding the first menu item
            menu->AppendSeparator();
        }

        int tabId = wxXmlResource::GetXRCID(wxString() << "output_tab_" << label);
        wxMenuItem* item = new wxMenuItem(hiddenTabsMenu, tabId, label);
        hiddenTabsMenu->Append(item);

        // Output pane does not support "detach"
        if(dpi.GetPanes().Index(label) != wxNOT_FOUND) {
            item->Enable(false);
        }

        hiddenTabsMenu->Bind(
            wxEVT_MENU,
            // Use lambda by value here so we make a copy
            [=](wxCommandEvent& e) {
                clCommandEvent eventShow(wxEVT_SHOW_OUTPUT_TAB);
                eventShow.SetSelected(true).SetString(label);
                EventNotifier::Get()->AddPendingEvent(eventShow);
            },
            tabId);
    }

    if(hiddenTabsMenu->GetMenuItemCount() == 0) {
        wxDELETE(hiddenTabsMenu);
    } else {
        menu->AppendSubMenu(hiddenTabsMenu, _("Hidden Tabs"), _("Hidden Tabs"));
    }
}
