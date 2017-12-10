//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : shelltab.cpp
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
#include "editor_config.h"

#include "macros.h"
#include "async_executable_cmd.h"
#include "manager.h"
#include "frame.h"
#include "shelltab.h"
#include "pluginmanager.h"
#include "editor_config.h"
#include "lexer_configuration.h"
#include "ColoursAndFontsManager.h"
#include "event_notifier.h"

BEGIN_EVENT_TABLE(ShellTab, OutputTabWindow)
EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_STARTED, ShellTab::OnProcStarted)
EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDLINE, ShellTab::OnProcOutput)
EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDERRLINE, ShellTab::OnProcError)
EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ENDED, ShellTab::OnProcEnded)
EVT_BUTTON(XRCID("send_input"), ShellTab::OnSendInput)
EVT_BUTTON(XRCID("stop_process"), ShellTab::OnStopProc)
EVT_UPDATE_UI(XRCID("send_input"), ShellTab::OnUpdateUI)
EVT_UPDATE_UI(XRCID("stop_process"), ShellTab::OnUpdateUI)
EVT_UPDATE_UI(XRCID("hold_pane_open"), ShellTab::OnHoldOpenUpdateUI)
END_EVENT_TABLE()

ShellTab::ShellTab(wxWindow* parent, wxWindowID id, const wxString& name)
    : OutputTabWindow(parent, id, name)
    , m_inputSizer(NULL)
    , m_input(NULL)
    , m_cmd(NULL)
{
    m_inputSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* text = new wxStaticText(this, wxID_ANY, _("Send:"));
    m_inputSizer->Add(text, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 0);

    m_input =
        new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER);
    m_input->SetMinSize(wxSize(200, -1));
    m_input->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ShellTab::OnEnter), NULL, this);
    m_input->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(ShellTab::OnKeyDown), NULL, this);

    m_inputSizer->Add(m_input, 1, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

    wxButton* btn;
    btn = new wxButton(this, XRCID("send_input"), _("Send"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    m_inputSizer->Add(btn, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

    btn = new wxButton(this, XRCID("stop_process"), _("Stop"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    m_inputSizer->Add(btn, 0, wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 5);

    // grab the base class scintilla and put our sizer in its place
    wxSizer* mainSizer = m_vSizer;
    mainSizer->Detach(m_sci);

    m_vertSizer = new wxBoxSizer(wxVERTICAL);
    m_vertSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);
    m_vertSizer->Add(m_inputSizer, 0, wxEXPAND | wxALL, 1);

#ifdef __WXMAC__
    mainSizer->Insert(0, m_vertSizer, 1, wxEXPAND | wxALL, 1);
#else
    mainSizer->Add(m_vertSizer, 1, wxEXPAND | wxALL, 1);
#endif

    mainSizer->Layout();
    InitStyle(m_sci);
}

ShellTab::~ShellTab() {}

void ShellTab::InitStyle(wxStyledTextCtrl* sci)
{
    LexerConf::Ptr_t text = EditorConfigST::Get()->GetLexer("text");
    text->Apply(sci);
}

bool ShellTab::DoSendInput(const wxString& line)
{
    return m_cmd && m_cmd->IsBusy() && m_cmd->GetProcess()->Write(line + wxT('\n'));
}

void ShellTab::OnProcStarted(wxCommandEvent& e)
{
    if(m_cmd && m_cmd->IsBusy()) {
        return;
    }
    m_cmd = (AsyncExeCmd*)e.GetEventObject();
    AppendText("\n");
    AppendText(e.GetString());
    m_input->Clear();
}

void ShellTab::OnProcOutput(wxCommandEvent& e)
{
    AsyncExeCmd* cmd = (AsyncExeCmd*)e.GetEventObject();
    if(cmd != m_cmd) {
        // TODO: log message
        return;
    }
    AppendText(e.GetString());
}

void ShellTab::OnProcError(wxCommandEvent& e)
{
    AsyncExeCmd* cmd = (AsyncExeCmd*)e.GetEventObject();
    if(cmd != m_cmd) {
        // TODO: log message
        return;
    }
    AppendText(e.GetString());
    // TODO: mark line
}

void ShellTab::OnProcEnded(wxCommandEvent& e)
{
    AsyncExeCmd* cmd = (AsyncExeCmd*)e.GetEventObject();
    if(cmd != m_cmd) {
        // TODO: log message
        return;
    }
    AppendText(e.GetString());
    m_cmd = NULL;
}

void ShellTab::OnSendInput(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxString line = m_input->GetValue();
    if(DoSendInput(line)) {
        if(m_input->FindString(line) == wxNOT_FOUND) {
            m_input->Append(line);
        }
        m_input->SetValue(wxEmptyString);
        m_input->SetFocus(); // in case lost by editor changing
    }
}

void ShellTab::OnStopProc(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(m_cmd && m_cmd->IsBusy()) {
        m_cmd->Terminate();
    }
}

void ShellTab::OnUpdateUI(wxUpdateUIEvent& e) { e.Enable(m_cmd && m_cmd->IsBusy()); }

void ShellTab::OnKeyDown(wxKeyEvent& e)
{
    wxCommandEvent dummy;
    switch(e.GetKeyCode()) {
    case wxT('c'):
    case wxT('C'):
        if(e.GetModifiers() == wxMOD_CONTROL) {
            OnStopProc(dummy);
        } else {
            e.Skip();
        }
        break;
    default:
        e.Skip();
        break;
    }
}

void ShellTab::OnEnter(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxCommandEvent dummy;
    OnSendInput(dummy);
}

DebugTab::DebugTab(wxWindow* parent, wxWindowID id, const wxString& name)
    : ShellTab(parent, id, name)
{
    if(m_sci) {
        m_sci->SetMarginType(0, wxSTC_MARGIN_NUMBER);
        m_sci->SetMarginType(1, wxSTC_MARGIN_FORE);

        // int pixelWidth = 4 + 4 * m_sci->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));

        // Show number margin according to settings.
        m_sci->SetMarginWidth(0, 0);
        m_sci->SetMarginWidth(1, 0);

        m_sci->SetMarginLeft(1);
        m_sci->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        m_sci->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
    }

    m_tb->DeleteTool(XRCID("collapse_all"));
    m_tb->DeleteTool(XRCID("repeat_output"));

    wxCheckBox* cb = new wxCheckBox(m_tb, wxID_ANY, _("Enable debugger full logging"));
    m_tb->AddControl(cb);
    m_tb->Realize();

    cb->Bind(wxEVT_CHECKBOX, &DebugTab::OnEnableDbgLog, this);
    cb->Bind(wxEVT_UPDATE_UI, &DebugTab::OnEnableDbgLogUI, this);

    m_autoAppear = false;
    Connect(XRCID("hold_pane_open"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebugTab::OnHoldOpenUpdateUI), NULL, this);
}

DebugTab::~DebugTab() {}

bool DebugTab::DoSendInput(const wxString& cmd)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(!dbgr || !dbgr->IsRunning()) return false;
    bool contIsNeeded = ManagerST::Get()->GetBreakpointsMgr()->PauseDebuggerIfNeeded();
    dbgr->ExecuteCmd(cmd);
    if(contIsNeeded) {
        ManagerST::Get()->DbgContinue();
    }
    return true;
}

void DebugTab::OnStopProc(wxCommandEvent& e)
{
    wxUnusedVar(e);

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning()) {
        ManagerST::Get()->DbgDoSimpleCommand(DBG_PAUSE);
    }
}

void DebugTab::AppendLine(const wxString& line)
{
    if(m_sci->GetLineCount() > 2) {
        wxString lineBefore = m_sci->GetLine(m_sci->GetLineCount() - 2);
        wxString newLine(line);
        newLine.Trim().Trim(false);
        lineBefore.Trim().Trim(false);
        if((lineBefore == newLine) && (newLine == _("Continuing..."))) {
            // Dont add this line...
        } else {
            AppendText(line);
        }
    } else {
        AppendText(line);
    }
}

void DebugTab::OnUpdateUI(wxUpdateUIEvent& e)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    e.Enable(dbgr && dbgr->IsRunning());
}

void DebugTab::OnEnableDbgLog(wxCommandEvent& event)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr) {
        dbgr->EnableLogging(event.IsChecked());

        DebuggerInformation info = dbgr->GetDebuggerInformation();
        info.enableDebugLog = event.IsChecked();
        DebuggerMgr::Get().SetDebuggerInformation(dbgr->GetName(), info);
    }
}

void DebugTab::OnEnableDbgLogUI(wxUpdateUIEvent& event)
{
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr) {
        DebuggerInformation info = dbgr->GetDebuggerInformation();
        event.Check(info.enableDebugLog);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ShellTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    int sel = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetSelection();
    if(clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) {
        return;
    }

    if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {
        e.Enable(true);
        e.Check(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfOutput());

    } else {
        e.Enable(false);
        e.Check(false);
    }
}

void ShellTab::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    LexerConf::Ptr_t l = ColoursAndFontsManager::Get().GetLexer("text");
    l->Apply(m_sci);
}

void DebugTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    int sel = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetSelection();
    if(clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) {
        return;
    }

    if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {
        e.Enable(true);
        e.Check(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfDebug());

    } else {
        e.Enable(false);
        e.Check(false);
    }
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
// OutputTab class
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

OutputTab::OutputTab(wxWindow* parent, wxWindowID id, const wxString& name)
    : ShellTab(parent, id, name)
    , m_thread(NULL)
    , m_outputDebugStringActive(false)
{
    m_inputSizer->Show(false);
    GetSizer()->Layout();
    EventNotifier::Get()->Bind(wxEVT_OUTPUT_DEBUG_STRING, &OutputTab::OnOutputDebugString, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_STARTED, &OutputTab::OnDebugStarted, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &OutputTab::OnDebugStopped, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &OutputTab::OnWorkspaceClosed, this);
}

OutputTab::~OutputTab()
{
    EventNotifier::Get()->Unbind(wxEVT_OUTPUT_DEBUG_STRING, &OutputTab::OnOutputDebugString, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_STARTED, &OutputTab::OnDebugStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &OutputTab::OnDebugStopped, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &OutputTab::OnWorkspaceClosed, this);
    if(m_thread) {
        m_thread->Stop();
        wxDELETE(m_thread);
    }
}

void OutputTab::OnOutputDebugString(clCommandEvent& event)
{
    event.Skip();
    if(!m_outputDebugStringActive) return;

    wxString msg = event.GetString();
    msg.Trim().Trim(false);
    if(msg.IsEmpty()) return;

    wxString formattedMessage;
    formattedMessage << "[" << event.GetInt() << "] " << msg << "\n";
    AppendText(formattedMessage);
}

void OutputTab::OnDebugStarted(clDebugEvent& event)
{
    event.Skip();
    m_outputDebugStringActive = true;
    m_autoAppear = false;
    DoSetCollecting(true);
}

void OutputTab::OnDebugStopped(clDebugEvent& event)
{
    event.Skip();
    m_outputDebugStringActive = false;
    DoSetCollecting(false);
    m_autoAppear = true;
}

void OutputTab::OnProcStarted(wxCommandEvent& e)
{
    ShellTab::OnProcStarted(e);
    m_outputDebugStringActive = true;
    DoSetCollecting(true);
}

void OutputTab::OnProcEnded(wxCommandEvent& e)
{
    ShellTab::OnProcEnded(e);
    m_outputDebugStringActive = false;
    DoSetCollecting(false);
}

void OutputTab::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();
}

void OutputTab::DoSetCollecting(bool b)
{
#ifdef __WXMSW__
    if(b) {
        if(m_thread) {
            m_thread->Stop();
            wxDELETE(m_thread);
        }
        m_thread = new OutputDebugStringThread();
        m_thread->Start();
        m_thread->SetCollecting(true);
    } else {
        if(m_thread) {
            m_thread->Stop();
            wxDELETE(m_thread);
        }
    }
#endif
}
