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

#include "macros.h"
#include "async_executable_cmd.h"
#include "manager.h"
#include "frame.h"
#include "shelltab.h"


BEGIN_EVENT_TABLE(ShellTab, OutputTabWindow)
    EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_STARTED,    ShellTab::OnProcStarted)
    EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDLINE,    ShellTab::OnProcOutput)
    EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDERRLINE, ShellTab::OnProcError)
    EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ENDED,      ShellTab::OnProcEnded)
    
    EVT_BUTTON(XRCID("show_find"),    ShellTab::OnShowSearch)
    EVT_BUTTON(XRCID("send_input"),   ShellTab::OnSendInput)
    EVT_BUTTON(XRCID("stop_process"), ShellTab::OnStopProc)
    
    EVT_UPDATE_UI(XRCID("send_input"),   ShellTab::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("stop_process"), ShellTab::OnUpdateUI)
END_EVENT_TABLE()


ShellTab::ShellTab(wxWindow* parent, wxWindowID id, const wxString& name)
    : OutputTabWindow(parent, id, name)
    , m_inputSizer(NULL)
    , m_input(NULL)
    , m_findBar(NULL)
    , m_cmd(NULL)
{
    m_inputSizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxButton *btn = new wxBitmapButton(this, XRCID("show_find"), wxXmlResource::Get()->LoadBitmap(wxT("find_and_replace16")));
    m_inputSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    wxStaticText *text = new wxStaticText(this, wxID_ANY, wxT("Send:"));
    m_inputSizer->Add(text, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_input = new wxComboBox(this, wxID_ANY);
    m_input->SetMinSize(wxSize(200,-1));
    ConnectKeyDown(m_input, ShellTab::OnKeyDown);
    m_inputSizer->Add(m_input, 1, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    btn = new wxButton(this, XRCID("send_input"), wxT("Send"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    m_inputSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    btn = new wxButton(this, XRCID("stop_process"), wxT("Stop"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    m_inputSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_findBar = new QuickFindBar(this);
    m_findBar->Connect(XRCID("close_quickfind"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ShellTab::OnShowInput), NULL, this);
    m_findBar->SetEditor(m_sci);

	// grab the base class scintilla and put our sizer in its place
	wxSizer *mainSizer = GetSizer();
	mainSizer->Detach(m_sci);
    
	wxBoxSizer *vertSizer = new wxBoxSizer(wxVERTICAL);
	vertSizer->Add(m_sci,         1, wxEXPAND | wxALL, 1);
	vertSizer->Add(m_inputSizer,  0, wxEXPAND | wxALL, 1);
	vertSizer->Add(m_findBar,     0, wxEXPAND | wxALL, 1);
	mainSizer->Add(vertSizer,     1, wxEXPAND | wxALL, 1);
	mainSizer->Layout();
    
    InitStyle(m_sci);
}

ShellTab::~ShellTab()
{
}
 
void ShellTab::InitStyle(wxScintilla* sci)
{
	int caretSlop = 1;
	int caretStrict = 0;
	int caretEven = 0;
	int caretJumps = 0;
	int caretZone = 20;
	sci->SetXCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

	caretSlop = 1;
	caretStrict = 4;
	caretEven = 8;
	caretJumps = 0;
	caretZone = 1;
	sci->SetYCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);    
}

bool ShellTab::DoSendInput(const wxString &line)
{
    return m_cmd && m_cmd->IsBusy() && m_cmd->GetProcess()->Write(line+wxT('\n'));
}

void ShellTab::OnProcStarted(wxCommandEvent& e)
{
    if (m_cmd && m_cmd->IsBusy()) {
        // TODO: log message: already running a process
        return;
    }
    m_cmd = (AsyncExeCmd*) e.GetEventObject();
    Clear();
    AppendText(e.GetString());
    m_input->Clear();
}

void ShellTab::OnProcOutput(wxCommandEvent& e)
{
    AsyncExeCmd *cmd = (AsyncExeCmd*) e.GetEventObject();
    if (cmd != m_cmd) {
        // TODO: log message
        return;
    }
    AppendText(e.GetString());
}

void ShellTab::OnProcError(wxCommandEvent& e)
{
    AsyncExeCmd *cmd = (AsyncExeCmd*) e.GetEventObject();
    if (cmd != m_cmd) {
        // TODO: log message
        return;
    }
    AppendText(e.GetString());
    // TODO: mark line
}

void ShellTab::OnProcEnded(wxCommandEvent& e)
{
    AsyncExeCmd *cmd = (AsyncExeCmd*) e.GetEventObject();
    if (cmd != m_cmd) {
        // TODO: log message
        return;
    }
    AppendText(e.GetString());
    m_cmd = NULL;
}

void ShellTab::OnShowInput(wxCommandEvent &e)
{
    m_findBar->Show(false);
    m_inputSizer->Show(true);
    GetSizer()->Layout();
    m_input->SetFocus();
}

void ShellTab::OnShowSearch(wxCommandEvent& e)
{
    m_findBar->Show(true);
    m_inputSizer->Show(false);
    GetSizer()->Layout();
}

void ShellTab::OnSendInput(wxCommandEvent& e)
{
    wxUnusedVar(e);
    
    wxString line = m_input->GetValue();
    if (DoSendInput(line)) {
        if (m_input->FindString(line) == wxNOT_FOUND) {
            m_input->Append(line);
        }
        m_input->SetValue(wxEmptyString);
        m_input->SetFocus(); // in case lost by editor changing
    }
 }

void ShellTab::OnStopProc(wxCommandEvent& e)
 {
    wxUnusedVar(e);
    
    if (m_cmd && m_cmd->IsBusy()) {
        m_cmd->Terminate();
    }
}

void ShellTab::OnUpdateUI(wxUpdateUIEvent& e)
{
    e.Enable(m_cmd && m_cmd->IsBusy());
}

void ShellTab::OnKeyDown(wxKeyEvent& e)
{
    wxCommandEvent dummy;
    switch (e.GetKeyCode()) {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            OnSendInput(dummy);
            break;
        case wxT('c'):
        case wxT('C'):
            if (e.GetModifiers() == wxMOD_CONTROL) {
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



DebugTab::DebugTab(wxWindow* parent, wxWindowID id, const wxString& name)
    : ShellTab(parent, id, name)
{
}

DebugTab::~DebugTab()
{
}

bool DebugTab::DoSendInput(const wxString &cmd)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (!dbgr || !dbgr->IsRunning())
        return false;
    bool contIsNeeded = ManagerST::Get()->GetBreakpointsMgr()->PauseDebuggerIfNeeded();
    dbgr->ExecuteCmd(cmd);
    if (contIsNeeded) {
        ManagerST::Get()->DbgStart();
    }
    return true;
}

void DebugTab::OnStopProc(wxCommandEvent& e)
{
    wxUnusedVar(e);
    
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
        ManagerST::Get()->DbgDoSimpleCommand(DBG_PAUSE);
	}
}

void DebugTab::AppendLine(const wxString& line)
{
    if (m_sci->GetLine(m_sci->GetLineCount()-1) != line.BeforeFirst(wxT('\n'))) {
        AppendText(line);
    }
}

void DebugTab::OnUpdateUI(wxUpdateUIEvent& e)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
    e.Enable(dbgr && dbgr->IsRunning());
}

