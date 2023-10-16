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
#include "output_pane.h"

#include "BuildTab.hpp"
#include "clPropertiesPage.hpp"
#include "clStrings.h"
#include "clTabTogglerHelper.h"
#include "detachedpanesinfo.h"
#include "dockablepanemenumanager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "findresultstab.h"
#include "findusagetab.h"
#include "frame.h"
#include "pluginmanager.h"
#include "replaceinfilespanel.h"
#include "shelltab.h"

#include <algorithm>
#include <wx/aui/framemanager.h>
#include <wx/dcbuffer.h>
#include <wx/xrc/xmlres.h>

OutputPane::OutputPane(wxWindow* parent, const wxString& caption, long style)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 250), style)
    , m_caption(caption)
    , m_buildInProgress(false)
{
    Hide();
    CreateGUIControls();
    EventNotifier::Get()->Connect(wxEVT_EDITOR_CLICKED, wxCommandEventHandler(OutputPane::OnEditorFocus), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTED, clBuildEventHandler(OutputPane::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_ENDED, clBuildEventHandler(OutputPane::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &OutputPane::OnSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_SHOW_OUTPUT_TAB, &OutputPane::OnToggleTab, this);
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &OutputPane::OnPageChanged, this);
    SetSize(-1, 250);
}

OutputPane::~OutputPane()
{
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &OutputPane::OnPageChanged, this);
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
    style |= kNotebook_UnderlineActiveTab | kNotebook_FixedWidth;
    if(EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs()) {
        style |= kNotebook_MouseScrollSwitchTabs;
    }
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    m_book->Bind(wxEVT_BOOK_FILELIST_BUTTON_CLICKED, &OutputPane::OnOutputBookFileListMenu, this);

    // Calculate the widest tab (the one with the 'Workspace' label) TODO: What happens with translations?
    mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 0);

    // the IManager instance
    IManager* mgr = PluginManager::Get();
    m_build_tab = new BuildTab(m_book);
    m_book->AddPage(m_build_tab, BUILD_WIN, true);
    m_tabs.insert(std::make_pair(BUILD_WIN, Tab(BUILD_WIN, m_build_tab, wxNOT_FOUND)));
    mgr->AddOutputTab(BUILD_WIN);

    // Find in files
    m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, FIND_IN_FILES_WIN);
    m_book->AddPage(m_findResultsTab, FIND_IN_FILES_WIN, false);
    m_tabs.insert(std::make_pair(FIND_IN_FILES_WIN, Tab(FIND_IN_FILES_WIN, m_findResultsTab)));
    mgr->AddOutputTab(FIND_IN_FILES_WIN);

    // Replace In Files
    m_replaceResultsTab = new ReplaceInFilesPanel(m_book, wxID_ANY, REPLACE_IN_FILES);
    m_book->AddPage(m_replaceResultsTab, REPLACE_IN_FILES, false);
    m_tabs.insert(std::make_pair(REPLACE_IN_FILES, Tab(REPLACE_IN_FILES, m_replaceResultsTab)));
    mgr->AddOutputTab(REPLACE_IN_FILES);

    // Show Usage ("References")
    m_showUsageTab = new FindUsageTab(m_book);
    m_book->AddPage(m_showUsageTab, SHOW_USAGE, false);
    m_tabs.insert({ SHOW_USAGE, Tab(SHOW_USAGE, m_showUsageTab) });
    mgr->AddOutputTab(SHOW_USAGE);

    // Output tab
    m_outputWind = new OutputTab(m_book, wxID_ANY, OUTPUT_WIN);
    m_book->AddPage(m_outputWind, OUTPUT_WIN, false);
    m_tabs.insert(std::make_pair(OUTPUT_WIN, Tab(OUTPUT_WIN, m_outputWind)));
    mgr->AddOutputTab(OUTPUT_WIN);

    // Output tab
    m_terminal = new clBuiltinTerminalPane(m_book, wxID_ANY);
    m_book->AddPage(m_terminal, TERMINAL_TAB, false);
    m_tabs.insert(std::make_pair(TERMINAL_TAB, Tab(TERMINAL_TAB, m_terminal)));
    mgr->AddOutputTab(TERMINAL_TAB);

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

typedef struct _tagTabInfo {
    wxString text;
    wxWindow* win = nullptr;
    int bmp = wxNOT_FOUND;
} tagTabInfo;

void OutputPane::ApplySavedTabOrder(bool update_ui) const
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

    // Restore any saved last selection
    // NB: this doesn't actually work atm: the selection is set correctly, but presumably something else changes is
    // later
    // I've left the code in case anyone ever has time/inclination to fix it
    if((index >= 0) && (index < (int)m_book->GetPageCount())) {
        m_book->SetSelection(index);
    } else if(m_book->GetPageCount()) {
        m_book->SetSelection(0);
    }

    if(update_ui) {
        clGetManager()->GetDockingManager()->Update();
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
        if(!clTabTogglerHelper::IsTabInNotebook(PaneId::BOTTOM_BAR, t.m_label)) {
            clGetManager()->BookAddPage(PaneId::BOTTOM_BAR, t.m_window, t.m_label);
        } else {
            clGetManager()->BookSelectPage(PaneId::BOTTOM_BAR, t.m_label);
        }
    } else {
        // hide the tab
        clGetManager()->BookRemovePage(PaneId::BOTTOM_BAR, t.m_label);
    }
}

void OutputPane::OnOutputBookFileListMenu(clContextMenuEvent& event)
{
    if(event.GetEventObject() != m_book) {
        event.Skip();
        return;
    }
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

        if(menu->GetMenuItemCount() > 0 && hiddenTabsMenu->GetMenuItemCount() == 0) {
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

void OutputPane::ShowTab(const wxString& name, bool show)
{
    clCommandEvent show_event(wxEVT_SHOW_OUTPUT_TAB);
    show_event.SetString(name);
    show_event.SetSelected(show);
    EventNotifier::Get()->ProcessEvent(show_event);
}

void OutputPane::OnPageChanged(wxBookCtrlEvent& event)
{
    event.Skip();
    //::SetBestFocus(m_book->GetCurrentPage());
}
