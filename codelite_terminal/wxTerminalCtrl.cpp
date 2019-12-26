#include "wxTerminalCtrl.h"
#include <wx/sizer.h>
#include <processreaderthread.h>
#include <wx/regex.h>
#include "procutils.h"
#include <drawingutils.h>
#include <wx/wupdlock.h>
#include <wx/log.h>
#include "wxTerminalOptions.h"
#include "TextView.h"
#include <fileutils.h>
#ifndef __WXMSW__
#include "unixprocess_impl.h"
#include <termios.h>
#include <unistd.h>
#endif

wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_READY, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_OUTPUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_STDERR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_DONE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_SET_TITLE, clCommandEvent);

///---------------------------------------------------------------
/// Helper methods
///---------------------------------------------------------------
static wxString ConvertString(const std::string& str, const wxMBConv& conv = wxConvISO8859_1)
{
    if(str.empty()) { return wxEmptyString; }
    wxString wx_str = wxString(str.c_str(), conv);
    if(wx_str.IsEmpty()) {
        // conversion failed
        wx_str = wxString::From8BitData(str.c_str());
    }
    return wx_str;
}
///---------------------------------------------------------------
///
///---------------------------------------------------------------

wxTerminalCtrl::wxTerminalCtrl() {}

wxTerminalCtrl::wxTerminalCtrl(wxWindow* parent, wxWindowID winid, const wxExecuteEnv& env, const wxPoint& pos,
                               const wxSize& size, long style, const wxString& name)
{
    if(!Create(parent, winid, env, pos, size, style)) { return; }
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_textCtrl = new TextView(this);
    m_textCtrl->SetSink(this);
    GetSizer()->Add(m_textCtrl, 1, wxEXPAND);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminalCtrl::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_STDERR, &wxTerminalCtrl::OnProcessStderr, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminalCtrl::OnProcessTerminated, this);

    Bind(wxEVT_CHAR_HOOK, &wxTerminalCtrl::OnKeyDown, this);
    GetSizer()->Fit(this);
    m_textCtrl->GetCtrl()->Bind(wxEVT_LEFT_DOWN, &wxTerminalCtrl::OnLeftDown, this);

    // load the commands from the configurationk file
    m_history.SetCommands(wxTerminalOptions::Get().GetHistory());
    Bind(wxEVT_IDLE, &wxTerminalCtrl::OnIdle, this);
}

wxTerminalCtrl::~wxTerminalCtrl()
{
    Unbind(wxEVT_IDLE, &wxTerminalCtrl::OnIdle, this);
    if(m_shell) {
        m_shell->Detach();
        wxDELETE(m_shell);
    }

    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &wxTerminalCtrl::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_STDERR, &wxTerminalCtrl::OnProcessStderr, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &wxTerminalCtrl::OnProcessTerminated, this);
    Unbind(wxEVT_CHAR_HOOK, &wxTerminalCtrl::OnKeyDown, this);
}

bool wxTerminalCtrl::Create(wxWindow* parent, wxWindowID winid, const wxExecuteEnv& env, const wxPoint& pos,
                            const wxSize& size, long style, const wxString& name)
{
    m_env = env;
    m_style = style & ~wxWINDOW_STYLE_MASK; // Remove all wxWindow style masking (Hi Word)
    return wxWindow::Create(parent, winid, pos, size,
                            style & wxWINDOW_STYLE_MASK); // Pass only the Windows related styles
}

void wxTerminalCtrl::PostCreate()
{
    wxString shell;
#ifdef __WXMSW__
    shell = "C:\\Windows\\System32\\cmd.exe /Q"; // echo off
    if(!m_startupCommand.IsEmpty()) {
        // /C means: terminate when the command has completed
        shell << " /C " << m_startupCommand;
    }
#else
    shell = "/bin/bash";
    if(!m_startupCommand.IsEmpty()) { shell << " -c '" << m_startupCommand << "'"; }
#endif
    m_shell = ::CreateAsyncProcess(this, shell, IProcessCreateDefault | IProcessRawOutput, m_workingDirectory);

    if(m_shell) {
        clCommandEvent readyEvent(wxEVT_TERMINAL_CTRL_READY);
        readyEvent.SetEventObject(this);
#ifndef __WXMSW__
        UnixProcessImpl* unixProcess = dynamic_cast<UnixProcessImpl*>(m_shell);
        if(unixProcess) {
            m_pts = unixProcess->GetTty();
            readyEvent.SetString(GetPTS());
        }
#endif
        if(m_style & wxTERMINAL_CTRL_USE_EVENTS) { GetEventHandler()->AddPendingEvent(readyEvent); }
        if(IsPrintTTY()) {
            //  Write the tty into a file
            FileUtils::WriteFileContent(wxFileName(m_ttyfile), GetPTS());
        }
    }

    // Keep a list of initial processes that we DONT want to kill
    std::vector<long> children;
    ProcUtils::GetChildren(m_shell->GetPid(), children);
    for(long pid : children) {
        m_initialProcesses.insert(pid);
    }
}

void wxTerminalCtrl::Run(const wxString& command)
{
    if(m_shell) {
        if(!command.IsEmpty()) {
            int a = 0;
            ++a;
        }
        m_shell->WriteRaw(command + "\n");
        AppendText("\n");
        if(!m_echoOff && !command.empty() && (command != "exit")) { m_history.Add(command); }
    }
}

void wxTerminalCtrl::OnProcessOutput(clProcessEvent& event)
{
    if(m_style & wxTERMINAL_CTRL_USE_EVENTS) {
        clCommandEvent outputEvent(wxEVT_TERMINAL_CTRL_OUTPUT);
        outputEvent.SetString(event.GetOutput());
        outputEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(outputEvent);
    }
    AppendText(event.GetOutput());
}

void wxTerminalCtrl::OnProcessStderr(clProcessEvent& event)
{
    if(m_style & wxTERMINAL_CTRL_USE_EVENTS) {
        clCommandEvent outputEvent(wxEVT_TERMINAL_CTRL_STDERR);
        outputEvent.SetString(event.GetOutput());
        outputEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(outputEvent);
    }
    AppendText(event.GetOutput());
}

void wxTerminalCtrl::OnProcessTerminated(clProcessEvent& event)
{
    if(IsPauseOnExit()) {
        m_waitingForKey = true;
        AppendText("\nHit any key to continue...\n");
    } else {
        DoProcessTerminated();
    }
}

void wxTerminalCtrl::AppendText(const wxString& text)
{
    if(m_log.IsOpened()) {
        m_log.Write(text);
        m_log.Flush();
    }
    m_textCtrl->StyleAndAppend(text);
    m_commandOffset = m_textCtrl->GetLastPosition();
    CallAfter(&wxTerminalCtrl::SetFocus);
}

void wxTerminalCtrl::OnKeyDown(wxKeyEvent& event)
{
    if(m_waitingForKey) {
        DoProcessTerminated();
        m_waitingForKey = false;
    } else {
        if(!m_textCtrl->IsEditable()) { return; }
        if(event.GetKeyCode() == WXK_NUMPAD_ENTER || event.GetKeyCode() == WXK_RETURN) {
            // Execute command
            Run(GetShellCommand());
            if(m_echoOff) {
                m_textCtrl->SetDefaultStyle(m_preEchoOffAttr);
                m_preEchoOffAttr = wxTextAttr();
                m_echoOff = false;
            }
        } else if(event.GetKeyCode() == WXK_HOME || event.GetKeyCode() == WXK_NUMPAD_HOME) {
            m_textCtrl->SetInsertionPoint(m_commandOffset);

        } else if(event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_NUMPAD_UP) {
            m_history.Up();
            SetShellCommand(m_history.Get());
        } else if(event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_NUMPAD_DOWN) {
            m_history.Down();
            SetShellCommand(m_history.Get());
        } else if((event.GetKeyCode() == 'C') && event.ControlDown()) {
            // Generate Ctrl-C
            GenerateCtrlC();
        } else if((event.GetKeyCode() == 'L') && event.ControlDown()) {
            ClearScreen();
        } else if((event.GetKeyCode() == 'U') && event.ControlDown()) {
            ClearLine();
        } else if((event.GetKeyCode() == 'D') && event.ControlDown()) {
            Logout();
        } else {
            SetEchoOff();
            int pos = m_textCtrl->GetInsertionPoint();
            if(event.GetKeyCode() == WXK_BACK || event.GetKeyCode() == WXK_LEFT) {
                // going backward
                event.Skip(pos > m_commandOffset);
            } else {
                if(pos < m_commandOffset) { m_textCtrl->SetInsertionPointEnd(); }
                event.Skip(true);
            }
        }
    }
}

wxString wxTerminalCtrl::GetShellCommand() const
{
    return m_textCtrl->GetRange(m_commandOffset, m_textCtrl->GetLastPosition());
}

void wxTerminalCtrl::SetShellCommand(const wxString& command)
{
    if(command.IsEmpty()) { return; }
    m_textCtrl->SetCommand(m_commandOffset, command);
    CallAfter(&wxTerminalCtrl::SetCaretAtEnd);
}

void wxTerminalCtrl::SetCaretAtEnd() { m_textCtrl->SetCaretEnd(); }

#ifdef __WXMSW__
static void KillPorcessByPID(DWORD pid)
{
    HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
    if(hProcess != INVALID_HANDLE_VALUE) { TerminateProcess(hProcess, 0); }
}
#endif

void wxTerminalCtrl::GenerateCtrlC()
{
    // Get list of the process children
    // and send them all SIGINT
    std::vector<long> children;
    ProcUtils::GetChildren(m_shell->GetPid(), children);
    for(long pid : children) {
        // Don't kill any initial process
        if(m_initialProcesses.count(pid)) { continue; }
#ifdef __WXMSW__
        KillPorcessByPID(pid);
#else
        wxKillError rc;
        ::wxKill(pid, wxSIGTERM, &rc, wxKILL_CHILDREN);
#endif
    }
}

void wxTerminalCtrl::ClearScreen()
{
    wxWindowUpdateLocker locker(m_textCtrl);
#if USE_STC
    m_textCtrl->Clear();
    m_commandOffset = 0;
    Run("");
#else
    // Delete the entire content excluding the last list
    if(m_textCtrl->GetNumberOfLines() < 1) { return; }
    long x, y;
    if(!m_textCtrl->PositionToXY(m_textCtrl->GetLastPosition(), &x, &y)) { return; }
    long insertPos = m_textCtrl->GetInsertionPoint();
    long lineStartPos = m_textCtrl->XYToPosition(0, y);
    m_textCtrl->Remove(0, lineStartPos);
    m_commandOffset -= lineStartPos;
    insertPos -= lineStartPos;
    m_textCtrl->SetInsertionPoint(insertPos);
#endif
}

void wxTerminalCtrl::ClearLine() { m_textCtrl->Remove(m_commandOffset, m_textCtrl->GetLastPosition()); }

void wxTerminalCtrl::Logout()
{
    wxString command;
#ifdef __WXMSW__
    command = "exit";
#else
    command = "exit";
#endif
    // Loguot
    Run(command);
}

void wxTerminalCtrl::SetEchoOff()
{
    if(!m_echoOff) {
        wxString line = m_textCtrl->GetLineText(m_textCtrl->GetNumberOfLines() - 1);
        line = line.Lower();
        if(line.Contains("password:") || line.Contains("password for")) {
            m_echoOff = true;
            m_preEchoOffAttr = m_textCtrl->GetDefaultStyle();
            wxTextAttr echoOffAttr = m_preEchoOffAttr;
            echoOffAttr.SetFontSize(0);
            echoOffAttr.SetTextColour(echoOffAttr.GetBackgroundColour());
            m_textCtrl->SetDefaultStyle(echoOffAttr);
        }
    }
}

void wxTerminalCtrl::DoProcessTerminated()
{
    if(m_style & wxTERMINAL_CTRL_USE_EVENTS) {
        clCommandEvent outputEvent(wxEVT_TERMINAL_CTRL_DONE);
        outputEvent.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(outputEvent);
    }
    m_textCtrl->SetEditable(false);
}

wxString wxTerminalCtrl::GetPTS() const { return ConvertString(m_pts); }

void wxTerminalCtrl::Start(const wxString& startupCommand)
{
    m_startupCommand = startupCommand;
    PostCreate();
    if(!GetLogfile().IsEmpty()) {
        wxFileName logfile(GetLogfile());
        wxLogNull NOLOG;
        m_log.Open(logfile.GetFullPath(), "a+b");
    }
}

void wxTerminalCtrl::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    CallAfter(&wxTerminalCtrl::CheckInsertionPoint);
}

void wxTerminalCtrl::CheckInsertionPoint()
{
    int pos = m_textCtrl->GetInsertionPoint();
    m_textCtrl->SetEditable(pos >= m_commandOffset);
}

void wxTerminalCtrl::ReloadSettings() { m_textCtrl->ReloadSettings(); }

void wxTerminalCtrl::Focus() { m_textCtrl->Focus(); }

void wxTerminalCtrl::OnIdle(wxIdleEvent& event)
{
#if 0
    int count = m_textCtrl->Truncate();
    if(count) {
        m_commandOffset -= count;
        m_textCtrl->SetInsertionPointEnd();
    }
#endif
}

