//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggerpane.cpp
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
#include "DebuggerCallstackView.h"
#include "DebuggerDisassemblyTab.h"
#include "breakpointdlg.h"
#include "codelite_events.h"
#include "debugger.h"
#include "debuggerasciiviewer.h"
#include "debuggermanager.h"
#include "debuggerpane.h"
#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "localstable.h"
#include "manager.h"
#include "memoryview.h"
#include "plugin_general_wxcp.h"
#include "shelltab.h"
#include "simpletable.h"
#include "threadlistpanel.h"
#include "wx/dcbuffer.h"
#include "wx/xrc/xmlres.h"
#include <wx/aui/framemanager.h>

const wxString DebuggerPane::LOCALS = wxTRANSLATE("Locals");
const wxString DebuggerPane::WATCHES = wxTRANSLATE("Watches");
const wxString DebuggerPane::FRAMES = wxTRANSLATE("Call Stack");
const wxString DebuggerPane::BREAKPOINTS = wxTRANSLATE("Breakpoints");
const wxString DebuggerPane::THREADS = wxTRANSLATE("Threads");
const wxString DebuggerPane::MEMORY = wxTRANSLATE("Memory");
const wxString DebuggerPane::ASCII_VIEWER = wxTRANSLATE("Ascii Viewer");
const wxString DebuggerPane::DEBUGGER_OUTPUT = wxTRANSLATE("Output");
const wxString DebuggerPane::DISASSEMBLY = wxTRANSLATE("Disassemble");

#define IS_DETACHED(name) (detachedPanes.Index(name) != wxNOT_FOUND) ? true : false

DebuggerPane::DebuggerPane(wxWindow* parent, const wxString& caption, wxAuiManager* mgr)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
    , m_caption(caption)
    , m_initDone(false)
    , m_mgr(mgr)
{
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &DebuggerPane::OnSettingsChanged, this);
    CreateGUIControls();
}

DebuggerPane::~DebuggerPane()
{
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &DebuggerPane::OnSettingsChanged, this);
}

void DebuggerPane::OnPageChanged(wxBookCtrlEvent& event)
{
    if(m_initDone && DebuggerMgr::Get().GetActiveDebugger() && DebuggerMgr::Get().GetActiveDebugger()->IsRunning()) {
        if(event.GetEventObject() == m_book) {
            ManagerST::Get()->CallAfter(&Manager::UpdateDebuggerPane);
        } else {
            event.Skip();
        }
    } else {
        event.Skip();
    }
}

void DebuggerPane::CreateGUIControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    long style = (kNotebook_Default | kNotebook_AllowDnD);
    if(!EditorConfigST::Get()->GetOptions()->GetOutputTabsDirection()) {
        style |= kNotebook_BottomTabs;
    }
    if(EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs()) {
        style |= kNotebook_MouseScrollSwitchTabs;
    }
    style |= kNotebook_UnderlineActiveTab;

    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    auto images = m_book->GetBitmaps();

    m_book->SetTabDirection(EditorConfigST::Get()->GetOptions()->GetOutputTabsDirection());
    mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 0);

    // Calculate the widthest tab (the one with the 'Call Stack' label)
    int xx, yy;
    wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxWindow::GetTextExtent(_("Breakpoints"), &xx, &yy, NULL, NULL, &fnt);

    // load list of detached panes
    wxArrayString detachedPanes;
    DetachedPanesInfo dpi;
    EditorConfigST::Get()->ReadObject(wxT("DetachedPanesList"), &dpi);
    detachedPanes = dpi.GetPanes();

    wxString name;
    name = wxGetTranslation(LOCALS);
    // Add the 'Locals View'
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_localsTable = new LocalsTable(cp);
        cp->SetChildNoReparent(m_localsTable);

    } else {
        m_localsTable = new LocalsTable(m_book);
        m_book->AddPage(m_localsTable, name, false, wxNOT_FOUND);
    }

    // Add the 'watches View'
    name = wxGetTranslation(WATCHES);
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_watchesTable = new WatchesTable(cp);
        cp->SetChildNoReparent(m_watchesTable);

    } else {
        m_watchesTable = new WatchesTable(m_book);
        m_book->AddPage(m_watchesTable, name, false, wxNOT_FOUND);
    }

    // Add the 'ASCII Viewer'
    name = wxGetTranslation(ASCII_VIEWER);
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_asciiViewer = new DebuggerAsciiViewer(cp);
        cp->SetChildNoReparent(m_asciiViewer);

    } else {
        m_asciiViewer = new DebuggerAsciiViewer(m_book);
        m_book->AddPage(m_asciiViewer, name, false, wxNOT_FOUND);
    }

    // Add the 'Call Stack'
    name = wxGetTranslation(FRAMES);
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_frameList = new DebuggerCallstackView(cp);
        cp->SetChildNoReparent(m_frameList);

    } else {
        m_frameList = new DebuggerCallstackView(m_book);
        m_book->AddPage(m_frameList, name, false, wxNOT_FOUND);
    }

    // Add the 'Breakpoints'
    name = wxGetTranslation(BREAKPOINTS);
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_breakpoints = new BreakpointDlg(cp);
        cp->SetChildNoReparent(m_breakpoints);

    } else {
        m_breakpoints = new BreakpointDlg(m_book);
        m_book->AddPage(m_breakpoints, name, false, wxNOT_FOUND);
    }

    // Add the 'Threads'
    name = wxGetTranslation(THREADS);
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_threads = new ThreadListPanel(cp);
        cp->SetChildNoReparent(m_threads);

    } else {
        m_threads = new ThreadListPanel(m_book);
        m_book->AddPage(m_threads, name, false, wxNOT_FOUND);
    }

    // Add the 'Memory View'
    name = wxGetTranslation(MEMORY);
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_memory = new MemoryView(cp);
        cp->SetChildNoReparent(m_memory);

    } else {
        m_memory = new MemoryView(m_book);
        m_book->AddPage(m_memory, name, false, wxNOT_FOUND);
    }

    // Add the "Output" tab
    name = wxGetTranslation(DEBUGGER_OUTPUT);
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_outputDebug = new DebugTab(cp, wxID_ANY, wxGetTranslation(DEBUGGER_OUTPUT));
        cp->SetChildNoReparent(m_outputDebug);

    } else {
        m_outputDebug = new DebugTab(m_book, wxID_ANY, wxGetTranslation(DEBUGGER_OUTPUT));
        m_book->AddPage(m_outputDebug, name, false, wxNOT_FOUND);
    }

    // Add the "Output" tab
    name = wxGetTranslation(DISASSEMBLY);
    if(IS_DETACHED(name)) {
        DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNOT_FOUND, wxSize(200, 200));
        m_disassemble = new DebuggerDisassemblyTab(cp, wxGetTranslation(DISASSEMBLY));
        cp->SetChildNoReparent(m_disassemble);

    } else {
        m_disassemble = new DebuggerDisassemblyTab(m_book, wxGetTranslation(DISASSEMBLY));
        m_book->AddPage(m_disassemble, name, false, wxNOT_FOUND);
    }
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &DebuggerPane::OnPageChanged, this);
    m_initDone = true;
}

void DebuggerPane::SelectTab(const wxString& tabName)
{
    for(size_t i = 0; i < m_book->GetPageCount(); i++) {
        if(m_book->GetPageText(i) == tabName) {
            m_book->SetSelection(i);
            break;
        }
    }
}

void DebuggerPane::Clear()
{
    GetLocalsTable()->Clear();
    GetWatchesTable()->Clear();
    GetFrameListView()->Clear();
    GetThreadsView()->Clear();
    GetMemoryView()->Clear();
}

void DebuggerPane::OnSettingsChanged(wxCommandEvent& event)
{
    event.Skip();
    m_book->EnableStyle(kNotebook_BottomTabs,
                        EditorConfigST::Get()->GetOptions()->GetOutputTabsDirection() == wxBOTTOM);
}

//----------------------------------------------------------------
// Debugger config
//----------------------------------------------------------------

void DebuggerPaneConfig::FromJSON(const JSONItem& json) { m_windows = json.namedObject("m_windows").toSize_t(All); }

JSONItem DebuggerPaneConfig::ToJSON() const
{
    JSONItem e = JSONItem::createObject(GetName());
    e.addProperty("m_windows", m_windows);
    return e;
}

DebuggerPaneConfig::DebuggerPaneConfig()
    : clConfigItem("debuggerWindows")
    , m_windows(All)
{
}

DebuggerPaneConfig::~DebuggerPaneConfig() {}

wxString DebuggerPaneConfig::WindowName(eDebuggerWindows flag) const
{
    switch(flag) {
    default:
    case All:
        return wxEmptyString;

    case Locals:
        return wxGetTranslation(DebuggerPane::LOCALS);

    case Watches:
        return wxGetTranslation(DebuggerPane::WATCHES);

    case Threads:
        return wxGetTranslation(DebuggerPane::THREADS);

    case Callstack:
        return wxGetTranslation(DebuggerPane::FRAMES);

    case Breakpoints:
        return wxGetTranslation(DebuggerPane::BREAKPOINTS);

    case Memory:
        return wxGetTranslation(DebuggerPane::MEMORY);

    case AsciiViewer:
        return wxGetTranslation(DebuggerPane::ASCII_VIEWER);

    case Output:
        return wxGetTranslation(DebuggerPane::DEBUGGER_OUTPUT);

    case Disassemble:
        return wxGetTranslation(DebuggerPane::DISASSEMBLY);
    }
}
