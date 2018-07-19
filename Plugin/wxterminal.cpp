//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#include "asyncprocess.h"
#include "drawingutils.h"
#include "globals.h"
#include "processreaderthread.h"
#include "wxterminal.h"
#include <wx/app.h>
#include <wx/filename.h>

#define OUTPUT_BUFFER_MAX_SIZE 1024 * 1024 /* 1MB of buffer */

#ifdef __WXMSW__
#include "windows.h"
#define SHELL_PREFIX wxT("cmd /c ")
#define SHELL_WRAPPER wxT("")
#else

#if defined(__WXMAC__) || defined(__FreeBSD__)
#include <sys/wait.h>
#else
#include <wait.h>
#endif

#include <signal.h>
#define SHELL_PREFIX wxT("/bin/sh -c ")
#define SHELL_WRAPPER wxT("'")
#endif

#if defined(__WXGTK__)
#ifdef __FreeBSD__
#include <libutil.h>
#include <sys/ioctl.h>
#include <termios.h>
#else
#include <pty.h>
#endif
#include "unixprocess_impl.h"
#elif defined(__WXMAC__)
#include "unixprocess_impl.h"
#include <util.h>
#endif

wxTerminal::wxTerminal(wxWindow* parent)
    : wxTerminalBase(parent)
    , m_process(NULL)
    , m_exitWhenProcessDies(false)
    , m_exitOnKey(false)
    , m_inferiorEnd(0)
#if defined(__WXMAC__) || defined(__WXGTK__)
    , m_dummyProcess(NULL)
#endif
    , m_interactive(false)
{
    Bind(wxEVT_IDLE, &wxTerminal::OnIdle, this);
    m_defaultStyle.SetFont(m_textCtrl->GetFont());
    m_defaultStyle.SetTextColour(DrawingUtils::GetOutputPaneFgColour());
    m_defaultStyle.SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());

    m_textCtrl->SetForegroundColour(DrawingUtils::GetOutputPaneFgColour());
    m_textCtrl->SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
    m_textCtrl->SetDefaultStyle(m_defaultStyle);

    wxTheApp->Bind(wxEVT_MENU, &wxTerminal::OnCut, this, wxID_COPY);
    wxTheApp->Bind(wxEVT_MENU, &wxTerminal::OnCopy, this, wxID_CUT);
    wxTheApp->Bind(wxEVT_MENU, &wxTerminal::OnSelectAll, this, wxID_SELECTALL);

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminal::OnReadProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminal::OnProcessEnd, this);
}

wxTerminal::~wxTerminal()
{
#if defined(__WXMAC__) || defined(__WXGTK__)
    StopTTY();
#endif

    Unbind(wxEVT_IDLE, &wxTerminal::OnIdle, this);
    wxTheApp->Unbind(wxEVT_MENU, &wxTerminal::OnCut, this, wxID_COPY);
    wxTheApp->Unbind(wxEVT_MENU, &wxTerminal::OnCopy, this, wxID_CUT);
    wxTheApp->Unbind(wxEVT_MENU, &wxTerminal::OnSelectAll, this, wxID_SELECTALL);
}

void wxTerminal::OnEnter(wxKeyEvent& event)
{
    event.Skip();
    if(m_interactive) {
        wxString lineText = GetCommandText();
        if(lineText.IsEmpty()) {
            return;
        }
        DoProcessCommand(lineText);
    }
}

void wxTerminal::OnUp(wxKeyEvent& event)
{
    wxString command = m_history.ArrowUp();
    if(command.IsEmpty()) return;
    InsertCommandText(command);
}

void wxTerminal::OnProcessEnd(clProcessEvent& event)
{
    wxDELETE(m_process);

    // Make sure we flush everything
    DoFlushOutputBuffer();

    if(m_exitWhenProcessDies) {
        m_textCtrl->SetInsertionPointEnd();
        m_textCtrl->AppendText(wxString(wxT("\n")) + _("Press any key to continue..."));
        m_exitOnKey = true;
    }
}

void wxTerminal::OnReadProcessOutput(clProcessEvent& event) { m_outputBuffer << event.GetOutput(); }

void wxTerminal::DoCtrlC()
{
    if(!m_process) return;

#ifdef __WXMSW__
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)m_process->GetPid());
    if(hProcess) {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
#else
    // int status(0); Commented out as 'Unused variable'
    wxKill(m_process->GetPid(), wxSIGKILL, NULL, wxKILL_CHILDREN);
#endif
}

void wxTerminal::Execute(const wxString& command, bool exitWhenDone, const wxString& workingDir)
{
    if(m_process) return;

    m_textCtrl->Clear();

    // restore default style
    m_textCtrl->SetDefaultStyle(m_defaultStyle);
    m_textCtrl->SetFocus();

    m_exitWhenProcessDies = exitWhenDone;
    m_workingDir = workingDir;
    DoProcessCommand(command);
}

void wxTerminal::DoProcessCommand(const wxString& command)
{
    wxString cmd(command);
    cmd.Trim().Trim(false);
    if(cmd.IsEmpty()) return;
    m_history.Add(cmd);

    wxString path;
    // Add the shell prefix
    ::WrapInShell(cmd);
    wxString cmdShell;
    cmdShell.swap(cmd);

    // real command
    IProcess* cmdPrc = ::CreateAsyncProcess(this, cmdShell, IProcessCreateWithHiddenConsole, m_workingDir);
    if(cmdPrc) {
        m_process = cmdPrc;

    } else {
        m_process = NULL;
        m_textCtrl->SetInsertionPointEnd();
        m_textCtrl->AppendText(wxString::Format(_("Failed to execute command: %s\nWorking Directory: %s\n"),
                                                cmdShell.c_str(), m_workingDir.c_str()));

        if(m_exitWhenProcessDies) {
            CaretToEnd();
            m_textCtrl->AppendText(wxString(wxT("\n")) + _("Press any key to continue..."));
            m_exitOnKey = true;
        }
    }
}

void wxTerminal::KillInferior() { DoCtrlC(); }

bool wxTerminal::IsRunning() { return m_process != NULL; }

void wxTerminal::Clear()
{
    m_textCtrl->ClearAll();
    CaretToEnd();
}

#if defined(__WXGTK__) || defined(__WXMAC__)
wxString wxTerminal::StartTTY()
{
    m_process = NULL;
    // Open the master side of a pseudo terminal
    int master = ::posix_openpt(O_RDWR | O_NOCTTY);
    if(master < 0) {
        return "";
    }

    // Grant access to the slave pseudo terminal
    if(::grantpt(master) < 0) {
        ::close(master);
        return "";
    }

    // Clear the lock flag on the slave pseudo terminal
    if(::unlockpt(master) < 0) {
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
    static_cast<UnixProcessImpl*>(m_dummyProcess)->SetReadHandle(master);
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
    if(!m_outputBuffer.IsEmpty()) {
        CaretToEnd();
        if(!m_outputBuffer.EndsWith("\n")) {
            m_outputBuffer << "\n";
        }
        m_textCtrl->AppendText(m_outputBuffer);
        CaretToEnd();
        m_outputBuffer.Clear();
    }
}

void wxTerminal::CaretToEnd() { m_textCtrl->GotoPos(m_textCtrl->GetLength()); }

void wxTerminal::OnCopy(wxCommandEvent& event)
{
    if(wxWindow::FindFocus() != m_textCtrl) {
        event.Skip();
        return;
    }
    if(m_textCtrl->CanCopy()) {
        m_textCtrl->Copy();
    }
}

void wxTerminal::OnCut(wxCommandEvent& event)
{
    if(wxWindow::FindFocus() != m_textCtrl) {
        event.Skip();
        return;
    }
    if(m_textCtrl->CanCut()) {
        m_textCtrl->Cut();
    }
}

void wxTerminal::OnSelectAll(wxCommandEvent& event)
{
    if(wxWindow::FindFocus() != m_textCtrl) {
        event.Skip();
        return;
    }
    m_textCtrl->SelectAll();
}

void wxTerminal::OnDown(wxKeyEvent& event)
{
    wxString command = m_history.ArrowDown();
    if(command.IsEmpty()) return;
    InsertCommandText(command);
}

void wxTerminal::OnLeft(wxKeyEvent& event)
{
    // Right / Left movement is allowed only on the last line
    int curline = m_textCtrl->GetCurrentLine();
    if(curline == (m_textCtrl->GetLineCount() - 1)) {
        event.Skip();
    }
}

void wxTerminal::OnRight(wxKeyEvent& event)
{
    // Right / Left movement is allowed only on the last line
    int curline = m_textCtrl->GetCurrentLine();
    if(curline == (m_textCtrl->GetLineCount() - 1)) {
        event.Skip();
    }
}

void wxTerminal::InsertCommandText(const wxString& command)
{
    int lastLineStartPos = m_textCtrl->PositionFromLine(m_textCtrl->GetLineCount() - 1);
    int lastLineEndPos = m_textCtrl->GetLastPosition();
    m_textCtrl->SetSelection(lastLineStartPos, lastLineEndPos);
    m_textCtrl->ReplaceSelection(command);
    CaretToEnd();
}

wxString wxTerminal::GetCommandText()
{
    int lastLineStartPos = m_textCtrl->PositionFromLine(m_textCtrl->GetLineCount() - 1);
    int lastLineEndPos = m_textCtrl->GetLastPosition();
    wxString cmd = m_textCtrl->GetTextRange(lastLineStartPos, lastLineEndPos);
    cmd.Trim().Trim(false);
    return cmd;
}

// History
wxTerminalHistory::wxTerminalHistory()
    : m_where(wxNOT_FOUND)
{
}

wxTerminalHistory::~wxTerminalHistory() {}

void wxTerminalHistory::Add(const wxString& command)
{
    m_history.insert(m_history.begin(), command);
    m_where = 0;
}

wxString wxTerminalHistory::ArrowUp()
{
    if(m_history.empty()) return "";
    if((m_where == wxNOT_FOUND) || (m_where >= m_history.size())) {
        m_where = 0;
    } else {
        ++m_where;
    }
    if(m_where < m_history.size()) {
        return m_history[m_where];
    }
    return "";
}

wxString wxTerminalHistory::ArrowDown()
{
    if(m_history.empty()) return "";
    if((m_where >= 0) && (m_where < m_history.size()))  {
        m_where--;
        return m_history[m_where];
    } else {
        m_where = wxNOT_FOUND;
        return "";
    }
}
