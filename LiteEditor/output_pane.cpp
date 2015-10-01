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
#include <wx/xrc/xmlres.h>
#include "frame.h"
#include "editor_config.h"
#include "new_build_tab.h"
#include <wx/dcbuffer.h>
#include "event_notifier.h"
#include "pluginmanager.h"
#include "output_pane.h"
#include "findresultstab.h"
#include "findusagetab.h"
#include "dockablepanemenumanager.h"
#include "replaceinfilespanel.h"
#include "new_build_tab.h"
#include "shelltab.h"
#include "taskpanel.h"
#include "wxcl_log_text_ctrl.h"
#include <algorithm>

#if HAS_LIBCLANG
#include "ClangOutputTab.h"
#endif

const wxString OutputPane::FIND_IN_FILES_WIN = _("Search");
const wxString OutputPane::BUILD_WIN = _("Build");
const wxString OutputPane::OUTPUT_WIN = _("Output");
const wxString OutputPane::OUTPUT_DEBUG = _("Debug");
const wxString OutputPane::REPLACE_IN_FILES = _("Replace");
const wxString OutputPane::TASKS = _("Tasks");
const wxString OutputPane::TRACE_TAB = _("Trace");
const wxString OutputPane::SHOW_USAGE = _("References");
const wxString OutputPane::CLANG_TAB = _("Clang");

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
    SetSize(-1, 250);
}

OutputPane::~OutputPane()
{
    delete wxLog::SetActiveTarget(m_logTargetOld);
    EventNotifier::Get()->Disconnect(
        wxEVT_EDITOR_CLICKED, wxCommandEventHandler(OutputPane::OnEditorFocus), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTED, clBuildEventHandler(OutputPane::OnBuildStarted), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_ENDED, clBuildEventHandler(OutputPane::OnBuildEnded), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &OutputPane::OnSettingsChanged, this);
}

void OutputPane::CreateGUIControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    long style = (kNotebook_Default | kNotebook_AllowDnD);
    if(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection() == wxBOTTOM) {
        style |= kNotebook_BottomTabs;
    } else if(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection() == wxLEFT) {
        style |= kNotebook_LeftTabs;
    } else if(EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection() == wxRIGHT) {
        style |= kNotebook_RightTabs;
    }

    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

    BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();

    // Calculate the widest tab (the one with the 'Workspace' label) TODO: What happens with translations?
    int xx, yy;
    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxWindow::GetTextExtent(wxGetTranslation(REPLACE_IN_FILES), &xx, &yy, NULL, NULL, &fnt);

    mainSizer->Add(m_book, 1, wxEXPAND | wxALL | wxGROW, 0);

#if CL_USE_NEW_BUILD_TAB
    m_buildWin = new NewBuildTab(m_book);
#else
    m_buildWin = new BuildTab(m_book, wxID_ANY, wxGetTranslation(BUILD_WIN));
#endif
    m_book->AddPage(
        m_buildWin, wxGetTranslation(BUILD_WIN), true, bmpLoader->LoadBitmap(wxT("toolbars/16/build/build")));

    m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, wxGetTranslation(FIND_IN_FILES_WIN));
    m_book->AddPage(m_findResultsTab,
                    wxGetTranslation(FIND_IN_FILES_WIN),
                    false,
                    bmpLoader->LoadBitmap(wxT("toolbars/16/search/find")));

    m_replaceResultsTab = new ReplaceInFilesPanel(m_book, wxID_ANY, wxGetTranslation(REPLACE_IN_FILES));
    m_book->AddPage(m_replaceResultsTab,
                    wxGetTranslation(REPLACE_IN_FILES),
                    false,
                    bmpLoader->LoadBitmap(wxT("toolbars/16/search/find_and_replace")));

    m_showUsageTab = new FindUsageTab(m_book, wxGetTranslation(SHOW_USAGE));
    m_book->AddPage(
        m_showUsageTab, wxGetTranslation(SHOW_USAGE), false, bmpLoader->LoadBitmap(wxT("toolbars/16/search/find")));

    m_outputWind = new OutputTab(m_book, wxID_ANY, wxGetTranslation(OUTPUT_WIN));
    m_book->AddPage(
        m_outputWind, wxGetTranslation(OUTPUT_WIN), false, bmpLoader->LoadBitmap(wxT("output-pane/16/terminal")));

#if HAS_LIBCLANG
    NewProjImgList images;
    m_clangOutputTab = new ClangOutputTab(m_book);
    m_book->AddPage(m_clangOutputTab, wxGetTranslation(CLANG_TAB), false, images.Bitmap("clang16"));
#endif
    wxTextCtrl* text = new wxTextCtrl(m_book,
                                      wxID_ANY,
                                      wxEmptyString,
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      wxTE_RICH2 | wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

    /////////////////////////////////////
    // Set the trace's font & colors
    /////////////////////////////////////

    m_book->AddPage(text, wxGetTranslation(TRACE_TAB), false, wxXmlResource::Get()->LoadBitmap(wxT("debug_window")));
    m_logTargetOld = wxLog::SetActiveTarget(new wxclTextCtrl(text));

    // Now that we set up our own log target, re-enable the logging
    wxLog::EnableLogging(true);

    m_taskPanel = new TaskPanel(m_book, wxID_ANY, wxGetTranslation(TASKS));
    m_book->AddPage(m_taskPanel, wxGetTranslation(TASKS), false, bmpLoader->LoadBitmap(wxT("output-pane/16/tasks")));
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
        if(title.empty()) {
            continue;
        }
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
}
