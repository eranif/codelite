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

const wxString OutputPane::FIND_IN_FILES_WIN = _("Search");
const wxString OutputPane::BUILD_WIN         = _("Build");
const wxString OutputPane::OUTPUT_WIN        = _("Output");
const wxString OutputPane::OUTPUT_DEBUG      = _("Debug");
const wxString OutputPane::REPLACE_IN_FILES  = _("Replace");
const wxString OutputPane::TASKS             = _("Tasks");
const wxString OutputPane::TRACE_TAB         = _("Trace");
const wxString OutputPane::SHOW_USAGE        = _("References");

OutputPane::OutputPane(wxWindow *parent, const wxString &caption)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 250))
    , m_caption(caption)
    , m_logTargetOld(NULL)
    , m_buildInProgress(false)
{
    CreateGUIControls();
    EventNotifier::Get()->Connect ( wxEVT_EDITOR_CLICKED , wxCommandEventHandler ( OutputPane::OnEditorFocus  ), NULL, this );
    EventNotifier::Get()->Connect ( wxEVT_BUILD_STARTED ,  clBuildEventHandler ( OutputPane::OnBuildStarted ), NULL, this );
    EventNotifier::Get()->Connect ( wxEVT_BUILD_ENDED ,    clBuildEventHandler ( OutputPane::OnBuildEnded   ), NULL, this );
    SetSize(-1, 250);
}

OutputPane::~OutputPane()
{
    delete wxLog::SetActiveTarget(m_logTargetOld);
    EventNotifier::Get()->Disconnect( wxEVT_EDITOR_CLICKED , wxCommandEventHandler ( OutputPane::OnEditorFocus  ), NULL, this );
    EventNotifier::Get()->Disconnect( wxEVT_BUILD_STARTED ,  clBuildEventHandler ( OutputPane::OnBuildStarted ), NULL, this );
    EventNotifier::Get()->Disconnect( wxEVT_BUILD_ENDED ,    clBuildEventHandler ( OutputPane::OnBuildEnded   ), NULL, this );
}

void OutputPane::CreateGUIControls()
{
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

    BitmapLoader *bmpLoader = PluginManager::Get()->GetStdIcons();

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
    m_book->AddPage(m_buildWin, wxGetTranslation(BUILD_WIN), true, bmpLoader->LoadBitmap(wxT("toolbars/16/build/build")));

#ifdef __WXMAC__
    m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, wxGetTranslation(FIND_IN_FILES_WIN), false);
#else
    m_findResultsTab = new FindResultsTab(m_book, wxID_ANY, wxGetTranslation(FIND_IN_FILES_WIN), true);
#endif

    m_book->AddPage(m_findResultsTab, wxGetTranslation(FIND_IN_FILES_WIN), false, bmpLoader->LoadBitmap(wxT("toolbars/16/search/find")));

    m_replaceResultsTab = new ReplaceInFilesPanel(m_book, wxID_ANY, wxGetTranslation(REPLACE_IN_FILES));
    m_book->AddPage(m_replaceResultsTab, wxGetTranslation(REPLACE_IN_FILES), false, bmpLoader->LoadBitmap(wxT("toolbars/16/search/find_and_replace")) );

    m_showUsageTab = new FindUsageTab(m_book, wxGetTranslation(SHOW_USAGE));
    m_book->AddPage(m_showUsageTab, wxGetTranslation(SHOW_USAGE), false, bmpLoader->LoadBitmap(wxT("toolbars/16/search/find")) );

    m_outputWind = new ShellTab(m_book, wxID_ANY, wxGetTranslation(OUTPUT_WIN));
    m_book->AddPage(m_outputWind, wxGetTranslation(OUTPUT_WIN), false, bmpLoader->LoadBitmap(wxT("output-pane/16/terminal")));

    wxTextCtrl *text = new wxTextCtrl(m_book, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 | wxTE_MULTILINE | wxTE_READONLY| wxHSCROLL);

    /////////////////////////////////////
    // Set the trace's font & colors
    /////////////////////////////////////

    m_book->AddPage(text, wxGetTranslation(TRACE_TAB), false, wxXmlResource::Get()->LoadBitmap(wxT("debug_window")));
    m_logTargetOld = wxLog::SetActiveTarget( new wxclTextCtrl(text) );

    // Now that we set up our own log target, re-enable the logging
    wxLog::EnableLogging(true);

    m_taskPanel = new TaskPanel(m_book, wxID_ANY, wxGetTranslation(TASKS));
    m_book->AddPage(m_taskPanel, wxGetTranslation(TASKS), false, bmpLoader->LoadBitmap(wxT("output-pane/16/tasks")));
    SetMinSize( wxSize(200, 100) );
    mainSizer->Layout();
}

void OutputPane::OnEditorFocus(wxCommandEvent& e)
{
    e.Skip();
    if (EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {

        // Optionally don't hide the various panes (sometimes it's irritating, you click to do something and...)
        size_t cursel(m_book->GetSelection());
        if (cursel != Notebook::npos
            && EditorConfigST::Get()->GetPaneStickiness(m_book->GetPageText(cursel))) {
            return;
        }

        if(m_buildInProgress)
            return;

        wxAuiPaneInfo &info = PluginManager::Get()->GetDockingManager()->GetPane(wxT("Output View"));
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
