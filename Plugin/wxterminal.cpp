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

#include "ColoursAndFontsManager.h"
#include "asyncprocess.h"
#include "clTerminalHistory.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "processreaderthread.h"
#include "wxterminal.h"
#include <algorithm>
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

wxDEFINE_EVENT(wxEVT_TERMINAL_EXECUTE_COMMAND, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_C, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_KILL_INFERIOR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_EXIT_WHEN_DONE, clCommandEvent);

#define MARKER_ID 1

wxTerminal::wxTerminal(wxWindow* parent)
    : wxTerminalBase(parent)
    , m_process(NULL)
    , m_exitWhenProcessDies(false)
    , m_exitOnKey(false)
#if defined(__WXMAC__) || defined(__WXGTK__)
    , m_dummyProcess(NULL)
#endif
    , m_interactive(false)
    , m_history(new clTerminalHistory())
{
    Bind(wxEVT_IDLE, &wxTerminal::OnIdle, this);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_textCtrl);
        m_textCtrl->MarkerSetForeground(MARKER_ID, lexer->GetProperty(0).GetFgColour());
        m_textCtrl->MarkerSetBackground(MARKER_ID, lexer->GetProperty(0).GetBgColour());
    }

    wxTheApp->Bind(wxEVT_MENU, &wxTerminal::OnCut, this, wxID_COPY);
    wxTheApp->Bind(wxEVT_MENU, &wxTerminal::OnCopy, this, wxID_CUT);
    wxTheApp->Bind(wxEVT_MENU, &wxTerminal::OnSelectAll, this, wxID_SELECTALL);

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminal::OnReadProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminal::OnProcessEnd, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &wxTerminal::OnThemeChanged, this);
    m_textCtrl->SetSize(wxSize(400, 300));
}

wxTerminal::~wxTerminal()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &wxTerminal::OnThemeChanged, this);
    wxDELETE(m_history);
#if defined(__WXMAC__) || defined(__WXGTK__)
    StopTTY();
#endif
    Unbind(wxEVT_IDLE, &wxTerminal::OnIdle, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminal::OnReadProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminal::OnProcessEnd, this);
    wxTheApp->Unbind(wxEVT_MENU, &wxTerminal::OnCut, this, wxID_COPY);
    wxTheApp->Unbind(wxEVT_MENU, &wxTerminal::OnCopy, this, wxID_CUT);
    wxTheApp->Unbind(wxEVT_MENU, &wxTerminal::OnSelectAll, this, wxID_SELECTALL);
}

void wxTerminal::OnEnter()
{
    if(m_interactive) {
        wxString lineText = GetCommandText();
        if(lineText.IsEmpty()) { return; }
        clCommandEvent event(wxEVT_TERMINAL_EXECUTE_COMMAND);
        event.SetEventObject(this);
        event.SetString(lineText);
        // We first try the event way
        if(!GetEventHandler()->ProcessEvent(event)) {
            // Do the default action
            DoProcessCommand(lineText);
        } else {
            m_history->Add(lineText);
        }
    }
}

void wxTerminal::OnUp(wxKeyEvent& event)
{
    const wxString& command = m_history->ArrowUp();
    if(command.IsEmpty()) return;
    InsertCommandText(command);
}

void wxTerminal::OnProcessEnd(clProcessEvent& event)
{
    wxDELETE(m_process);

    // Make sure we flush everything
    DoFlushOutputBuffer();

    if(m_exitWhenProcessDies) {
        AddTextWithEOL(_("\nPress any key to continue..."));
        m_exitOnKey = true;
    }
}

void wxTerminal::OnReadProcessOutput(clProcessEvent& event) { m_outputBuffer << event.GetOutput(); }

void wxTerminal::DoCtrlC()
{
    clCommandEvent event(wxEVT_TERMINAL_CTRL_C);
    event.SetEventObject(this);
    if(!GetEventHandler()->ProcessEvent(event) && m_process) {
        wxKill(m_process->GetPid(), wxSIGINT, nullptr, wxKILL_CHILDREN);
    }
}

void wxTerminal::Execute(const wxString& command, bool exitWhenDone, const wxString& workingDir)
{
    if(m_process) return;
    m_textCtrl->ClearAll();
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
    m_history->Add(cmd);

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
        AddTextWithEOL(wxString() << _("\nFailed to execute command: ") << cmdShell << _("\nWorking directory")
                                  << m_workingDir);

        if(m_exitWhenProcessDies) {
            AddTextWithEOL(_("\nPress any key to continue..."));
            m_exitOnKey = true;
        }
    }
}

void wxTerminal::KillInferior()
{
    clCommandEvent event(wxEVT_TERMINAL_KILL_INFERIOR);
    event.SetEventObject(this);
    if(!GetEventHandler()->ProcessEvent(event) && m_process) {
        wxKill(m_process->GetPid(), wxSIGKILL, NULL, wxKILL_CHILDREN);
    }
}

bool wxTerminal::IsRunning() { return m_process != NULL; }

void wxTerminal::Clear()
{
    m_textCtrl->SetReadOnly(false);
    m_textCtrl->ClearAll();
    m_textCtrl->SetReadOnly(true);
    CaretToEnd();
}

#if defined(__WXGTK__) || defined(__WXMAC__)
wxString wxTerminal::StartTTY()
{
    m_process = NULL;
    // Open the master side of a pseudo terminal
    int master = ::posix_openpt(O_RDWR | O_NOCTTY);
    if(master < 0) { return ""; }

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
        if(!m_outputBuffer.EndsWith("\n")) { m_outputBuffer << "\n"; }
        AddTextRaw(m_outputBuffer);
        m_outputBuffer.Clear();
    }
}

void wxTerminal::OnCopy(wxCommandEvent& event)
{
    if(wxWindow::FindFocus() != m_textCtrl) {
        event.Skip();
        return;
    }
    if(m_textCtrl->CanCopy()) { m_textCtrl->Copy(); }
}

void wxTerminal::OnCut(wxCommandEvent& event)
{
    if(wxWindow::FindFocus() != m_textCtrl) {
        event.Skip();
        return;
    }
    if(m_textCtrl->CanCut()) { m_textCtrl->Cut(); }
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
    const wxString& command = m_history->ArrowDown();
    if(command.IsEmpty()) return;
    InsertCommandText(command);
}

void wxTerminal::OnLeft(wxKeyEvent& event)
{
    // Don't allow moving LEFT when at the begin of a line
    if(m_textCtrl->GetColumn(m_textCtrl->GetCurrentPos()) == 0) { return; }

    // Right / Left movement is allowed only on the last line
    int curline = m_textCtrl->GetCurrentLine();
    if(curline == (m_textCtrl->GetLineCount() - 1)) { event.Skip(); }
}

void wxTerminal::OnRight(wxKeyEvent& event)
{
    // Right / Left movement is allowed only on the last line
    int curline = m_textCtrl->GetCurrentLine();
    if(curline == (m_textCtrl->GetLineCount() - 1)) { event.Skip(); }
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
    int line_before_last = m_textCtrl->GetLineCount() - 2;
    if(line_before_last < 0) return "";
    int lineStartPos = m_textCtrl->PositionFromLine(line_before_last);
    int lineLen = m_textCtrl->LineLength(line_before_last);
    wxString cmd = m_textCtrl->GetTextRange(lineStartPos, lineStartPos + lineLen);
    cmd.Trim().Trim(false);
    return cmd;
}

void wxTerminal::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_textCtrl);
        m_textCtrl->MarkerSetForeground(MARKER_ID, lexer->GetProperty(0).GetFgColour());
        m_textCtrl->MarkerSetBackground(MARKER_ID, lexer->GetProperty(0).GetBgColour());
    }
}

void wxTerminal::OnCtrlC(wxKeyEvent& event)
{
    wxUnusedVar(event);
    DoCtrlC();
}

void wxTerminal::AddTextWithEOL(const wxString& text)
{
    wxString textToAdd = text;
    textToAdd.Trim().Trim(false);
    if(textToAdd.IsEmpty()) { return; }
    if(!textToAdd.EndsWith("\n")) { textToAdd << "\n"; }
    m_textCtrl->SetReadOnly(false);
    m_textCtrl->AppendText(textToAdd);
    m_textCtrl->GotoPos(m_textCtrl->GetLastPosition());
    m_textCtrl->SetReadOnly(true);
}

void wxTerminal::AddTextRaw(const wxString& text)
{
    m_textCtrl->SetReadOnly(false);
    m_textCtrl->AppendText(text);
    m_textCtrl->ScrollToEnd();
    m_textCtrl->ClearSelections();
    m_textCtrl->GotoPos(m_textCtrl->GetLength());
    m_textCtrl->SetReadOnly(true);
}

void wxTerminal::OnKey(wxKeyEvent& event)
{
    if(m_exitOnKey) {
        clCommandEvent event(wxEVT_TERMINAL_EXIT_WHEN_DONE);
        GetEventHandler()->AddPendingEvent(event);
    } else {
        wxTerminalBase::OnKey(event);
    }
}
