#include "TerminalEmulator.h"
#include <wx/filename.h>
#include <wx/log.h>
#include "processreaderthread.h"
#include "macros.h"

#ifndef __WXMSW__
#include <signal.h>
#endif

wxDEFINE_EVENT(wxEVT_TERMINAL_COMMAND_EXIT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_COMMAND_OUTPUT, clCommandEvent);

TerminalEmulator::TerminalEmulator()
    : m_process(NULL)
    , m_pid(wxNOT_FOUND)
    , m_timer(NULL)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &TerminalEmulator::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &TerminalEmulator::OnProcessTerminated, this);
    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &TerminalEmulator::OnCheckProcessAlive, this, m_timer->GetId());
}

TerminalEmulator::~TerminalEmulator()
{
    Unbind(wxEVT_TIMER, &TerminalEmulator::OnCheckProcessAlive, this, m_timer->GetId());
    m_timer->Stop();
    wxDELETE(m_timer);
}

bool TerminalEmulator::ExecuteConsole(const wxString& command,
                                      const wxString& workingDirectory,
                                      bool waitOnExit,
                                      const wxString& title)
{
    wxString consoleCommand;
    wxString strTitle = title;
    if(strTitle.IsEmpty()) {
        strTitle << "'" << command << "'";
    } else {
        strTitle.Prepend("'").Append("'");
    }
    m_timer->Stop();
#ifdef __WXMSW__
    consoleCommand = PrepareCommand(command, strTitle, waitOnExit);

#elif defined(__WXGTK__)
    // Test for the common terminals on Linux
    if(wxFileName::Exists("/usr/bin/gnome-terminal")) {
        consoleCommand << "/usr/bin/gnome-terminal -t " << strTitle << " -x "
                       << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/konsole")) {
        consoleCommand << "/usr/bin/konsole -e " << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/lxterminal")) {
        consoleCommand << "/usr/bin/lxterminal -T " << strTitle << " -e "
                       << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/uxterm")) {
        consoleCommand << "/usr/bin/uxterm -T " << strTitle << " -e " << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/xterm")) {
        consoleCommand << "/usr/bin/xterm -T " << strTitle << " -e " << PrepareCommand(command, strTitle, waitOnExit);
    }

#elif defined(__WXMAC__)

    wxString consoleCommand = TERMINAL_CMD;
    consoleCommand.Replace("$(CMD)", command);
    wxUnusedVar(strTitle);
    wxUnusedVar(waitOnExit);

#endif
    if(consoleCommand.IsEmpty()) return false;
    wxLogMessage(consoleCommand);

    // Create the process as group leader, this way we make sure that killing it
    // will also kill all the children processes
    m_pid = ::wxExecute(consoleCommand, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
    bool res = (m_pid != 0);
    if(res) {
        m_timer->Start(150);
    }
    return res;
}

wxString TerminalEmulator::PrepareCommand(const wxString& str, const wxString& title, bool waitOnExit)
{
    wxString command;
#ifdef __WXGTK__
    // First escape any double quotes
    // so "Some Command" -> \"Some Command\"
    wxString escapedString = str;
    escapedString.Replace("\"", "\\\"");
    command << "/bin/bash -c \"" << escapedString;
    if(waitOnExit) {
        command << " && echo 'Hit ENTER to continue' && read";
    }
    command << "\"";

#elif defined(__WXMSW__)
    // Windows
    wxString escapedString = str;
    command << "cmd /C call title \"Node.js\" && " << escapedString;
    if(waitOnExit) {
        command << " && echo \"\" && pause";
    }
#else
// OSX

#endif
    return command;
}

void TerminalEmulator::OnProcessTerminated(clProcessEvent& event)
{
    // Process terminated
    wxDELETE(m_process);
    m_pid = wxNOT_FOUND;
    m_timer->Stop();
    // Notify that the terminal has terminated
    clCommandEvent terminateEvent(wxEVT_TERMINAL_COMMAND_EXIT);
    AddPendingEvent(terminateEvent);
}

void TerminalEmulator::Terminate()
{
    if(IsRunning()) {
        if(m_process) {
            m_process->Terminate();
        }
        if(m_pid != wxNOT_FOUND) {
            wxKill(m_pid, wxSIGKILL, NULL, wxKILL_CHILDREN);
            m_pid = wxNOT_FOUND;
            m_timer->Stop();
        }
    }
}

bool TerminalEmulator::IsRunning() const { return (m_process != NULL) || (m_pid != wxNOT_FOUND); }

void TerminalEmulator::OnProcessOutput(clProcessEvent& event)
{
    clCommandEvent evtOutput(wxEVT_TERMINAL_COMMAND_OUTPUT);
    evtOutput.SetString(event.GetOutput());
    AddPendingEvent(evtOutput);
}

bool TerminalEmulator::ExecuteNoConsole(const wxString& commandToRun, const wxString& workingDirectory)
{
    if(m_process) {
        // another process is running
        return false;
    }

    wxString command;
#ifdef __WXMSW__
    wxString shell = wxGetenv("COMSPEC");
    if(shell.IsEmpty()) {
        shell = "CMD";
    }

    command << shell << wxT(" /c \"");
    command << commandToRun << wxT("\"");

#else
    wxString tmpCmd = commandToRun;
    command << "/bin/sh -c '";
    // escape any single quoutes
    tmpCmd.Replace("'", "\\'");
    command << tmpCmd << "'";
#endif
    wxLogMessage("TerminalEmulator::ExecuteNoConsole: %s", command);
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateWithHiddenConsole, workingDirectory);
    return m_process != NULL;
}

void TerminalEmulator::OnCheckProcessAlive(wxTimerEvent& event)
{
    if(m_pid != wxNOT_FOUND) {
        bool isAlive;
#ifdef __WXMSW__
        HANDLE hProcess = ::OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION,
                                        FALSE, // not inheritable
                                        (DWORD)m_pid);
        if(hProcess != NULL) {
            DWORD exitCode;
            GetExitCodeProcess(hProcess, &exitCode);
            CloseHandle(hProcess);
            isAlive = (exitCode == STILL_ACTIVE);
        } else {
            isAlive = false;
        }
#else
        isAlive = (::kill(m_pid, 0) == 0);
#endif
        if(!isAlive) {
            clCommandEvent terminateEvent(wxEVT_TERMINAL_COMMAND_EXIT);
            AddPendingEvent(terminateEvent);
            m_pid = wxNOT_FOUND;
            m_timer->Stop();
        }
    }
}
