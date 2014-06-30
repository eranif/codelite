//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : wxterminal.cpp
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

#include "wxterminal.h"
#include "asyncprocess.h"
#include <wx/filename.h>
#include <wx/app.h>
#include "processreaderthread.h"
#include "drawingutils.h"

#define OUTPUT_BUFFER_MAX_SIZE 1024*1024 /* 1MB of buffer */

#ifdef __WXMSW__
#include "windows.h"
#define SHELL_PREFIX  wxT("cmd /c ")
#define SHELL_WRAPPER wxT("\"")
#else

#if defined(__WXMAC__) || defined(__FreeBSD__)
#include <sys/wait.h>
#else
#include <wait.h>
#endif

#include <signal.h>
#define SHELL_PREFIX  wxT("/bin/sh -c ")
#define SHELL_WRAPPER wxT("'")
#endif

#if defined(__WXGTK__)
#ifdef __FreeBSD__
#    include <sys/ioctl.h>
#    include <termios.h>
#    include <libutil.h>
#else
#    include <pty.h>
#endif
#    include "unixprocess_impl.h"
#elif defined(__WXMAC__)
#    include <util.h>
#    include "unixprocess_impl.h"
#endif

static wxString WrapInShell(const wxString &cmd)
{
    wxString shellCommand;
    shellCommand << SHELL_PREFIX << SHELL_WRAPPER << cmd << SHELL_WRAPPER;
    return shellCommand;
}

//-------------------------------------------------------------
BEGIN_EVENT_TABLE(wxTerminal, wxTerminalBase)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  wxTerminal::OnReadProcessOutput)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, wxTerminal::OnProcessEnd       )
    EVT_IDLE(wxTerminal::OnIdle)
END_EVENT_TABLE()

wxTerminal::wxTerminal( wxWindow* parent )
    : wxTerminalBase  ( parent )
    , m_process(NULL)
    , m_exitWhenProcessDies(false)
    , m_exitOnKey(false)
    , m_inferiorEnd(0)
#if defined(__WXMAC__) || defined(__WXGTK__)
    , m_dummyProcess(NULL)
#endif
    , m_interactive(false)
{
    m_defaultStyle.SetFont            ( m_textCtrl->GetFont() );
    m_defaultStyle.SetTextColour      ( DrawingUtils::GetOutputPaneFgColour() );
    m_defaultStyle.SetBackgroundColour( DrawingUtils::GetOutputPaneBgColour());

    m_textCtrl->SetForegroundColour   ( DrawingUtils::GetOutputPaneFgColour() );
    m_textCtrl->SetBackgroundColour   ( DrawingUtils::GetOutputPaneBgColour() );
    m_textCtrl->SetDefaultStyle       ( m_defaultStyle );
    
    wxTheApp->Connect(wxID_CUT,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxTerminal::OnEdit), NULL, this);
    wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxTerminal::OnEdit), NULL, this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxTerminal::OnEdit), NULL, this);
}

wxTerminal::~wxTerminal()
{
#if defined(__WXMAC__) || defined(__WXGTK__)
    StopTTY();
#endif

    wxTheApp->Disconnect(wxID_CUT,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxTerminal::OnEdit), NULL, this);
    wxTheApp->Disconnect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxTerminal::OnEdit), NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxTerminal::OnEdit), NULL, this);
}

void wxTerminal::OnText( wxCommandEvent& event )
{
    event.Skip();
}

void wxTerminal::OnEnter( wxCommandEvent& event )
{
    event.Skip();
    if ( m_interactive ) {
        wxString lineText = m_textCtrl->GetRange(m_inferiorEnd, m_textCtrl->GetInsertionPoint());
        lineText.Trim().Trim(false);
        DoProcessCommand(lineText);
    }
}

void wxTerminal::OnURL( wxTextUrlEvent& event )
{
    event.Skip();
}

void wxTerminal::OnKey(wxKeyEvent& event)
{
    // We don't want to allow much editing in the textctrl, but it's nice to let the DEL key delete the selection
    if ((event.GetKeyCode() == WXK_DELETE) && (m_textCtrl->HasSelection())) {
            m_textCtrl->Cut(); // which is close enough for our purposes
            return;
    }

    long curPos = m_textCtrl->GetInsertionPoint();
    if(curPos < m_inferiorEnd) {
        int keyCode = event.GetKeyCode();
        // Dont allow any key down when
        switch(keyCode) {
        case WXK_UP:
        case WXK_DOWN:
        case WXK_LEFT:
        case WXK_RIGHT:
        case WXK_NUMPAD_UP:
        case WXK_NUMPAD_DOWN:
        case WXK_NUMPAD_LEFT:
        case WXK_NUMPAD_RIGHT:
            event.Skip();
            break;

        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            event.Skip();
            break;

        default:
            break;
        }

        return;
    }


#ifndef __WXMSW__
    if(m_dummyProcess) {
        switch(event.GetKeyCode()) {
        case WXK_NUMPAD_ENTER:
        case WXK_RETURN: {
            // get last line and pass it to the caller
            wxString lineText = m_textCtrl->GetRange(m_inferiorEnd, curPos);
            lineText.Trim().Trim(false);
            m_dummyProcess->Write(lineText);
            break;
        }
        }
    }
#endif
    event.Skip();
}

void wxTerminal::OnEdit(wxCommandEvent& event)
{
    if (wxWindow::FindFocus() != m_textCtrl) {
        event.Skip();
        return;
    }

    switch (event.GetId()) {
        case wxID_COPY:
            m_textCtrl->Copy();
            break;
        case wxID_CUT:
            m_textCtrl->Cut();
            break;
        case wxID_SELECTALL:
            m_textCtrl->SelectAll();
            break;
        default:
            event.Skip();
    }
}

void wxTerminal::OnProcessEnd(wxCommandEvent& event)
{
    ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
    delete ped;
    wxDELETE(m_process);
    
    // Make sure we flush everything
    DoFlushOutputBuffer();

    if(m_exitWhenProcessDies) {
        m_textCtrl->SetInsertionPointEnd();
        m_textCtrl->AppendText(wxString(wxT("\n")) +_("Press any key to continue..."));
        m_exitOnKey = true;
    }
}

void wxTerminal::OnReadProcessOutput(wxCommandEvent& event)
{
    ProcessEventData *ped = (ProcessEventData *)event.GetClientData();
    m_outputBuffer << ped->GetData();
    wxDELETE(ped);
    
    // Incase we hit the limit of the output buffer, flush it now
    // if ( m_outputBuffer.length() > OUTPUT_BUFFER_MAX_SIZE ) {
    //     DoFlushOutputBuffer();
    // }
}

void wxTerminal::DoCtrlC()
{
    if(!m_process)
        return;

#ifdef __WXMSW__
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD) m_process->GetPid());
    if(hProcess) {
        TerminateProcess(hProcess, 0);
        CloseHandle(  hProcess );
    }
#else
    //int status(0); Commented out as 'Unused variable'
    wxKill(m_process->GetPid(), wxSIGKILL, NULL, wxKILL_CHILDREN);
#endif
}

void wxTerminal::Execute(const wxString& command, bool exitWhenDone, const wxString &workingDir)
{
    if(m_process)
        return;

    m_textCtrl->Clear();

    // restore default style
    m_textCtrl->SetDefaultStyle( m_defaultStyle );
    m_textCtrl->SetFocus();

    m_exitWhenProcessDies = exitWhenDone;
    m_workingDir          = workingDir;
    DoProcessCommand(command);
}

void wxTerminal::DoProcessCommand(const wxString& command)
{
    wxString cmd ( command );
    cmd.Trim().Trim(false);
    wxString path;
    // Add the shell prefix
    wxString cmdShell = WrapInShell(cmd);
    // real command
    IProcess *cmdPrc = CreateAsyncProcess(this, cmdShell, IProcessCreateWithHiddenConsole, m_workingDir);
    if( cmdPrc ) {
        m_process = cmdPrc;

    } else {
        m_process = NULL;
        m_textCtrl->SetInsertionPointEnd();
        m_textCtrl->AppendText(wxString::Format(_("Failed to execute command: %s\nWorking Directory: %s\n"), cmdShell.c_str(), m_workingDir.c_str()));

        if(m_exitWhenProcessDies) {
            m_textCtrl->SetInsertionPointEnd();
            m_textCtrl->AppendText(wxString(wxT("\n")) +_("Press any key to continue..."));
            m_exitOnKey = true;
        }
    }
}

void wxTerminal::KillInferior()
{
    DoCtrlC();
}

bool wxTerminal::IsRunning()
{
    return m_process != NULL;
}

void wxTerminal::Clear()
{
    m_textCtrl->Clear();
}

#if defined(__WXGTK__)||defined(__WXMAC__)
wxString wxTerminal::StartTTY()
{
    m_process = NULL;
    // Open the master side of a pseudo terminal
    int master = ::posix_openpt (O_RDWR|O_NOCTTY);
    if (master < 0) {
        return "";
    }

    // Grant access to the slave pseudo terminal
    if (::grantpt (master) < 0) {
        ::close(master);
        return "";
    }

    // Clear the lock flag on the slave pseudo terminal
    if (::unlockpt (master) < 0) {
        ::close(master);
        return "";
    }
    
    m_tty = ::ptsname(master);
    
    // disable ECHO
    struct termios termio;
    tcgetattr(master, &termio);
    termio.c_lflag = ICANON;
    termio.c_oflag = ONOCR | ONLRET;
    tcsetattr(master, TCSANOW, &termio);

    // Start a listener on the tty
    m_dummyProcess = new UnixProcessImpl(this);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetReadHandle  (master);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetWriteHandler(master);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetPid(wxNOT_FOUND);
    static_cast<UnixProcessImpl*>(m_dummyProcess)->StartReaderThread();
    return m_tty;
}

void wxTerminal::StopTTY()
{
    wxDELETE(m_dummyProcess);
    m_tty.Clear();
    // close(m_slave);
    // m_slave = -1;
}

#endif

void wxTerminal::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    DoFlushOutputBuffer();
}

void wxTerminal::DoFlushOutputBuffer()
{
    if ( !m_outputBuffer.IsEmpty() ) {
        m_textCtrl->SetInsertionPointEnd();
        m_textCtrl->AppendText( m_outputBuffer );
        m_textCtrl->SetSelection(m_textCtrl->GetLastPosition(), m_textCtrl->GetLastPosition());
        m_inferiorEnd = m_textCtrl->GetLastPosition();
        m_outputBuffer.Clear();
    }
}
