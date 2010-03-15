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
	btn->SetToolTip(wxT("Show QuickFind Bar"));

	wxStaticText *text = new wxStaticText(this, wxID_ANY, wxT("Send:"));
	m_inputSizer->Add(text, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	m_input = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxTE_PROCESS_ENTER);
	m_input->SetMinSize(wxSize(200,-1));
	m_input->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ShellTab::OnEnter), NULL, this);
	m_input->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(ShellTab::OnKeyDown), NULL, this);

	m_inputSizer->Add(m_input, 1, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	btn = new wxButton(this, XRCID("send_input"), wxT("Send"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_inputSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	btn = new wxButton(this, XRCID("stop_process"), wxT("Stop"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_inputSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

	m_findBar = new QuickFindBar(this);
	m_findBar->Connect(m_findBar->GetCloseButtonId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ShellTab::OnShowInput), NULL, this);
	m_findBar->SetEditor(m_sci);

	// grab the base class scintilla and put our sizer in its place
	wxSizer *mainSizer = GetSizer();
	mainSizer->Detach(m_sci);

	m_vertSizer = new wxBoxSizer(wxVERTICAL);
	m_vertSizer->Add(m_sci,         1, wxEXPAND | wxALL, 1);
	m_vertSizer->Add(m_inputSizer,  0, wxEXPAND | wxALL, 1);
	m_vertSizer->Add(m_findBar,     0, wxEXPAND | wxALL, 1);
	
#ifdef __WXMAC__
	mainSizer->Insert(0, m_vertSizer,     1, wxEXPAND | wxALL, 1);
#else
	mainSizer->Add(m_vertSizer,     1, wxEXPAND | wxALL, 1);
#endif

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
	Freeze();

	m_findBar->Show(false);
	m_inputSizer->Show(true);
	GetSizer()->Layout();
	m_input->SetFocus();

	Thaw();
}

void ShellTab::OnShowSearch(wxCommandEvent& e)
{
	Freeze();

	m_findBar->Show(true);
	m_inputSizer->Show(false);
	GetSizer()->Layout();

	Thaw();
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


void ShellTab::OnEnter(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxCommandEvent dummy;
	OnSendInput(dummy);
}


DebugTab::DebugTab(wxWindow* parent, wxWindowID id, const wxString& name)
		: ShellTab(parent, id, name)
{
	if (m_sci) {
		m_sci->SetMarginType(0, wxSCI_MARGIN_NUMBER);
		m_sci->SetMarginType(1, wxSCI_MARGIN_FORE);

		//int pixelWidth = 4 + 4 * m_sci->TextWidth(wxSCI_STYLE_LINENUMBER, wxT("9"));

		// Show number margin according to settings.
		m_sci->SetMarginWidth(0, 0);
		m_sci->SetMarginWidth(1, 0);

		m_sci->SetMarginLeft(1);
		m_sci->StyleSetBackground(wxSCI_STYLE_LINENUMBER, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		m_sci->StyleSetForeground(wxSCI_STYLE_LINENUMBER, wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
	}

	m_vertSizer->Prepend(new DebugTabPanel(this), 0, wxEXPAND);
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
	if ( m_sci->GetLineCount() > 2 ) {
		wxString lineBefore = m_sci->GetLine(m_sci->GetLineCount()-2);
		wxString newLine (line);
		newLine.Trim().Trim(false);
		lineBefore.Trim().Trim(false);
		if ( (lineBefore == newLine) && (newLine == wxT("Continuing...")) ) {
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
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	e.Enable(dbgr && dbgr->IsRunning());
}

///////////////////////////////////////////////////////////////////////////////////////////////

DebugTabPanel::DebugTabPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_checkBoxEnableLog = new wxCheckBox( this, wxID_ANY, wxT("Enable debugger full logging"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_checkBoxEnableLog, 0, wxALL|wxEXPAND, 0 );

	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	m_checkBoxEnableLog->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DebugTabPanel::OnEnableDbgLog ), NULL, this );
	m_checkBoxEnableLog->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebugTabPanel::OnEnableDbgLogUI ), NULL, this );
}

DebugTabPanel::~DebugTabPanel()
{
	// Disconnect Events
	m_checkBoxEnableLog->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DebugTabPanel::OnEnableDbgLog ), NULL, this );
	m_checkBoxEnableLog->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebugTabPanel::OnEnableDbgLogUI ), NULL, this );
}

void DebugTabPanel::OnEnableDbgLog(wxCommandEvent& event)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if(dbgr) {
		dbgr->EnableLogging(event.IsChecked());

		DebuggerInformation info = dbgr->GetDebuggerInformation();
		info.enableDebugLog = event.IsChecked();
		DebuggerMgr::Get().SetDebuggerInformation(dbgr->GetName(), info);
	}
}

void DebugTabPanel::OnEnableDbgLogUI(wxUpdateUIEvent& event)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if(dbgr) {
		DebuggerInformation info = dbgr->GetDebuggerInformation();
		event.Check( info.enableDebugLog );
	}
}
