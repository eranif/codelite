//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : console_frame.cpp
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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "console_frame.h"

#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "plugin.h"
#include "windowattrmanager.h"

#include <wx/app.h>

///////////////////////////////////////////////////////////////////////////

ConsoleFrame::ConsoleFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, _("Console"))
{
    CreateGUIControls();
}

#if USE_SFTP
ConsoleFrame::ConsoleFrame(wxWindow* parent, clSSH::Ptr_t ssh)
    : wxFrame(parent, wxID_ANY, _("Console"))
    , m_ssh(ssh)
{
    CreateGUIControls();
    m_terminal->Bind(wxEVT_TERMINAL_EXECUTE_COMMAND, &ConsoleFrame::OnExecuteRemoteCommand, this);
    m_channel.reset(new clSSHChannel(m_ssh, clSSHChannel::kRemoteCommand, this));
    Bind(wxEVT_SSH_CHANNEL_CLOSED, &ConsoleFrame::OnChannelClosed, this);
    Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &ConsoleFrame::OnChannelReadError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &ConsoleFrame::OnChannelRead, this);
    Bind(wxEVT_SSH_CHANNEL_PTY, &ConsoleFrame::OnChannelPty, this);
}
#endif

ConsoleFrame::~ConsoleFrame()
{
    m_terminal->Unbind(wxEVT_TERMINAL_EXIT_WHEN_DONE, &ConsoleFrame::OnExitWhenDone, this);
    m_terminal->Unbind(wxEVT_TERMINAL_CTRL_C, &ConsoleFrame::OnTerminalCtrlC, this);
}

void ConsoleFrame::CreateGUIControls()
{
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);
    m_terminal = new wxTerminal(this);
    m_terminal->SetInteractive(true);
    bSizer1->Add(m_terminal, 1, wxEXPAND);

    this->SetSizer(bSizer1);
    this->Layout();
    m_terminal->Focus();
    SetSize(wxDLG_UNIT(this, wxSize(500, 300)));
    SetName("ConsoleFrame");
    CenterOnScreen();
    WindowAttrManager::Load(this);
    m_terminal->Bind(wxEVT_TERMINAL_EXIT_WHEN_DONE, &ConsoleFrame::OnExitWhenDone, this);
    m_terminal->Bind(wxEVT_TERMINAL_CTRL_C, &ConsoleFrame::OnTerminalCtrlC, this);
}

#if USE_SFTP
void ConsoleFrame::OnExecuteRemoteCommand(clCommandEvent& event)
{
    try {
        if(m_channel->IsOpen()) {
            return;
        }
        if(!m_channel->IsOpen()) {
            m_channel->Open();
        }
        m_channel->Execute(event.GetString());

    } catch(clException& e) {
        m_terminal->AddTextWithEOL(e.What());
    }
}

void ConsoleFrame::OnChannelReadError(clCommandEvent& event)
{
    wxUnusedVar(event);
    m_terminal->AddTextRaw("\n");
    m_terminal->CaretToEnd();
    m_channel->Close();
}

void ConsoleFrame::OnChannelRead(clCommandEvent& event)
{
    m_terminal->AddTextRaw(event.GetString());
    m_terminal->CaretToEnd();
}

void ConsoleFrame::OnChannelClosed(clCommandEvent& event)
{
    wxUnusedVar(event);
    m_terminal->AddTextRaw("\n");
    m_terminal->CaretToEnd();
    m_channel->Close();
}

void ConsoleFrame::OnChannelPty(clCommandEvent& event)
{
    m_terminal->AddTextRaw("TTY=" + event.GetString() + "\n");
    m_terminal->CaretToEnd();
}
#endif

void ConsoleFrame::Execute(const wxString& command, const wxString& wd) { m_terminal->Execute(command, true, wd); }

void ConsoleFrame::OnExitWhenDone(clCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void ConsoleFrame::OnTerminalCtrlC(clCommandEvent& event)
{
    event.Skip();
#if USE_SFTP
    if(m_channel->IsOpen()) {
        m_channel->Close();
        m_terminal->AddTextWithEOL(_("\nInterrupted"));
        event.Skip(false); // No need for the default action
    }
#endif
}
