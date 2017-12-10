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
#include "dockablepanemenumanager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "findresultstab.h"
#include "findusagetab.h"
#include "frame.h"
#include "new_build_tab.h"
#include "pluginmanager.h"
#include "replaceinfilespanel.h"
#include "shelltab.h"
#include "taskpanel.h"
#include "wxcl_log_text_ctrl.h"
#include "output_pane.h"
#include <algorithm>
#include <wx/dcbuffer.h>
#include <wx/xrc/xmlres.h>
#include <wx/aui/framemanager.h>

#if HAS_LIBCLANG
#include "ClangOutputTab.h"
#endif

OutputPane::OutputPane(wxWindow* parent, const wxString& caption)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 250))
    , m_caption(caption)
    , m_logTargetOld(NULL)
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
    delete wxLog::SetActiveTarget(m_logTargetOld);
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
    if(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection() == wxBOTTOM) {
        style |= kNotebook_BottomTabs;
    } else if(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection() == wxLEFT) {
#ifdef __WXOSX__
        style &= ~(kNotebook_BottomTabs | kNotebook_LeftTabs | kNotebook_RightTabs);
#else
        style |= kNotebook_LeftTabs;
#endif
    } else if(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection() == wxRIGHT) {
#ifdef __WXOSX__
        style |= kNotebook_BottomTabs;
#else
        style |= kNotebook_RightTabs;
#endif
    }
    if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        style &= ~kNotebook_LightTabs;
        style |= kNotebook_DarkTabs;
    }
    style |= kNotebook_UnderlineActiveTab;
    if(EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs()) { style |= kNotebook_MouseScrollSwitchTabs; }
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

    BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();

    // Calculate the widest tab (the one with the 'Workspace' label) TODO: What happens with translations?
    int xx, yy;
    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxWindow::GetTextExtent(wxGetTranslation(REPLACE_IN_FILES), &xx, &yy, NULL, NULL, &fnt);

    mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 0);

    // the IManager instance
    IManager* mgr = PluginManager::Get();

    // Build tab
    m_buildWin = new NewBuildTab(m_book);
    m_book->AddPage(m_buildWin, wxGetTranslation(BUILD_WIN), true, bmpLoader->LoadBitmap(wxT("build")));
    m_tabs.insert(std::make_pair(wxGetTranslation(BUILD_WIN),
                                 Tab(wxGetTranslation(BUILD_WIN), m_buildWin, bmpLoader->LoadBitmap(wxT("build")))));
    mgr->AddOutputTab(wxGetTranslation(BUILD_WIN));

    // Find in files
    m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, wxGetTranslation(FIND_IN_FILES_WIN));
    m_book->AddPage(m_findResultsTab, wxGetTranslation(FIND_IN_FILES_WIN), false, bmpLoader->LoadBitmap(wxT("find")));
    m_tabs.insert(
        std::make_pair(wxGetTranslation(FIND_IN_FILES_WIN),
                       Tab(wxGetTranslation(FIND_IN_FILES_WIN), m_findResultsTab, bmpLoader->LoadBitmap(wxT("find")))));
    mgr->AddOutputTab(wxGetTranslation(FIND_IN_FILES_WIN));

    // Replace In Files
    m_replaceResultsTab = new ReplaceInFilesPanel(m_book, wxID_ANY, wxGetTranslation(REPLACE_IN_FILES));
    m_book->AddPage(m_replaceResultsTab, wxGetTranslation(REPLACE_IN_FILES), false,
                    bmpLoader->LoadBitmap(wxT("find_and_replace")));
    m_tabs.insert(std::make_pair(
        REPLACE_IN_FILES, Tab(REPLACE_IN_FILES, m_replaceResultsTab, bmpLoader->LoadBitmap(wxT("find_and_replace")))));
    mgr->AddOutputTab(REPLACE_IN_FILES);

    // Show Usage ("References")
    m_showUsageTab = new FindUsageTab(m_book, wxGetTranslation(SHOW_USAGE));
    m_book->AddPage(m_showUsageTab, wxGetTranslation(SHOW_USAGE), false, bmpLoader->LoadBitmap(wxT("find")));
    m_tabs.insert(std::make_pair(wxGetTranslation(SHOW_USAGE), Tab(wxGetTranslation(SHOW_USAGE), m_showUsageTab,
                                                                   bmpLoader->LoadBitmap(wxT("find")))));
    mgr->AddOutputTab(wxGetTranslation(SHOW_USAGE));

    // Output tab
    m_outputWind = new OutputTab(m_book, wxID_ANY, wxGetTranslation(OUTPUT_WIN));
    m_book->AddPage(m_outputWind, wxGetTranslation(OUTPUT_WIN), false, bmpLoader->LoadBitmap(wxT("console")));
    m_tabs.insert(std::make_pair(wxGetTranslation(OUTPUT_WIN), Tab(wxGetTranslation(OUTPUT_WIN), m_outputWind,
                                                                   bmpLoader->LoadBitmap(wxT("console")))));
    mgr->AddOutputTab(wxGetTranslation(OUTPUT_WIN));

#if HAS_LIBCLANG
    // Clang tab
    NewProjImgList images;
    m_clangOutputTab = new ClangOutputTab(m_book);
    m_book->AddPage(m_clangOutputTab, wxGetTranslation(CLANG_TAB), false, bmpLoader->LoadBitmap("clang"));
    m_tabs.insert(std::make_pair(wxGetTranslation(CLANG_TAB),
                                 Tab(wxGetTranslation(CLANG_TAB), m_clangOutputTab, bmpLoader->LoadBitmap("clang"))));
    mgr->AddOutputTab(wxGetTranslation(CLANG_TAB));
#endif

    /////////////////////////////////////
    // Set the trace's font & colors
    /////////////////////////////////////

    wxStyledTextCtrl* stcLog = new wxStyledTextCtrl(m_book);
    m_book->AddPage(stcLog, wxGetTranslation(TRACE_TAB), false, bmpLoader->LoadBitmap("log"));
    m_logTargetOld = wxLog::SetActiveTarget(new wxclTextCtrl(stcLog));
    m_tabs.insert(std::make_pair(wxGetTranslation(TRACE_TAB),
                                 Tab(wxGetTranslation(TRACE_TAB), stcLog, bmpLoader->LoadBitmap("log"))));
    mgr->AddOutputTab(wxGetTranslation(TRACE_TAB));

    // Now that we set up our own log target, re-enable the logging
    wxLog::EnableLogging(true);

    // Tasks panel
    m_taskPanel = new TaskPanel(m_book, wxID_ANY, wxGetTranslation(TASKS));
    m_book->AddPage(m_taskPanel, wxGetTranslation(TASKS), false, bmpLoader->LoadBitmap("tasks"));
    m_tabs.insert(std::make_pair(wxGetTranslation(TASKS),
                                 Tab(wxGetTranslation(TASKS), m_taskPanel, bmpLoader->LoadBitmap("tasks"))));
    mgr->AddOutputTab(wxGetTranslation(TASKS));

    SetMinSize(wxSize(200, 100));
    mainSizer->Layout();
}

void OutputPane::OnEditorFocus(wxCommandEvent& e)
{
    e.Skip();
    if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {

        // Optionally don't hide the various panes (sometimes it's irritating, you click to do something and...)
        int cursel(m_book->GetSelection());
        if(cursel != wxNOT_FOUND && EditorConfigST::Get()->GetPaneStickiness(m_book->GetPageText(cursel))) { return; }

        if(m_buildInProgress) return;

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
    wxArrayString panes;
    clTabInfo::Vec_t tabs;
    m_book->GetAllTabs(tabs);
    std::for_each(tabs.begin(), tabs.end(), [&](clTabInfo::Ptr_t t) { panes.Add(t->GetLabel()); });
    clConfig::Get().SetOutputTabOrder(panes, m_book->GetSelection());
}

typedef struct {
    wxString text;
    wxWindow* win;
    wxBitmap bmp;
} tagTabInfo;

void OutputPane::ApplySavedTabOrder() const
{

    wxArrayString tabs;
    int index = -1;
    if(!clConfig::Get().GetOutputTabOrder(tabs, index)) return;

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
}

void OutputPane::OnSettingsChanged(wxCommandEvent& event)
{
    event.Skip();
    m_book->SetTabDirection(EditorConfigST::Get()->GetOptions()->GetOutputTabsDirection());
    if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        m_book->SetStyle((m_book->GetStyle() & ~kNotebook_LightTabs) | kNotebook_DarkTabs);
    } else {
        m_book->SetStyle((m_book->GetStyle() & ~kNotebook_DarkTabs) | kNotebook_LightTabs);
    }
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
