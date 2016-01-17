#include "TerminalEmulator.h"
#include <wx/filename.h>
#include <wx/log.h>
#include "processreaderthread.h"
#include "macros.h"
#include <algorithm>
#include "dirsaver.h"

#ifndef __WXMSW__
#include <signal.h>
#endif

wxDEFINE_EVENT(wxEVT_TERMINAL_COMMAND_EXIT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_COMMAND_OUTPUT, clCommandEvent);

class MyProcess : public wxProcess
{
public:
    TerminalEmulator* m_parent;

public:
    MyProcess(TerminalEmulator* parent)
        : wxProcess(parent)
        , m_parent(parent)
    {
        if(m_parent) {
            m_parent->m_myProcesses.push_back(this);
        }
    }

    virtual ~MyProcess() { m_parent = NULL; }
    void OnTerminate(int pid, int status)
    {
        if(m_parent) {
            clCommandEvent terminateEvent(wxEVT_TERMINAL_COMMAND_EXIT);
            m_parent->AddPendingEvent(terminateEvent);
            m_parent->m_pid = wxNOT_FOUND;

            std::list<wxProcess*>::iterator iter = std::find_if(m_parent->m_myProcesses.begin(),
                                                                m_parent->m_myProcesses.end(),
                                                                [&](wxProcess* proc) { return proc == this; });
            if(iter != m_parent->m_myProcesses.end()) {
                m_parent->m_myProcesses.erase(iter);
            }
            delete this;
        }
    }
};

TerminalEmulator::TerminalEmulator()
    : m_process(NULL)
    , m_pid(wxNOT_FOUND)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &TerminalEmulator::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &TerminalEmulator::OnProcessTerminated, this);
}

TerminalEmulator::~TerminalEmulator()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &TerminalEmulator::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &TerminalEmulator::OnProcessTerminated, this);
    std::for_each(m_myProcesses.begin(), m_myProcesses.end(), [&](wxProcess* proc) {
        MyProcess* myproc = dynamic_cast<MyProcess*>(proc);
        myproc->m_parent = NULL;
    });
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
#ifdef __WXMSW__
    consoleCommand = PrepareCommand(command, strTitle, waitOnExit);

#elif defined(__WXGTK__)
    // Test for the common terminals on Linux
    // gnome-terminal, konsole and lxterminal are all starting asychronously
    // this means that "waitOnExit" has no effect here
    if(wxFileName::Exists("/usr/bin/gnome-terminal") && !waitOnExit) {
        consoleCommand << "/usr/bin/gnome-terminal -t " << strTitle << " -x "
                       << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/konsole") && !waitOnExit) {
        consoleCommand << "/usr/bin/konsole -e " << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/lxterminal") && !waitOnExit) {
        consoleCommand << "/usr/bin/lxterminal -T " << strTitle << " -e "
                       << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/uxterm")) {
        consoleCommand << "/usr/bin/uxterm -T " << strTitle << " -e " << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/xterm")) {
        consoleCommand << "/usr/bin/xterm -T " << strTitle << " -e " << PrepareCommand(command, strTitle, waitOnExit);
    }

#elif defined(__WXMAC__)

    consoleCommand = TERMINAL_CMD;
    consoleCommand.Replace("$(CMD)", command);
    wxUnusedVar(strTitle);
    wxUnusedVar(waitOnExit);

#endif
    if(consoleCommand.IsEmpty()) return false;
    wxLogMessage(consoleCommand);

    // Create the process as group leader, this way we make sure that killing it
    // will also kill all the children processes
    DirSaver ds;
    if(!workingDirectory.IsEmpty()) {
        wxSetWorkingDirectory(workingDirectory);
        wxLogMessage("Working directory is now: " + ::wxGetCwd());
    }
    
    wxLogMessage(consoleCommand);
    m_pid = ::wxExecute(consoleCommand, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER, new MyProcess(this));
    return (m_pid != 0);
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
        command << " ; echo 'Hit ENTER to continue' ; read";
    }
    command << "\"";

#elif defined(__WXMSW__)
    // Windows
    wxString escapedString = str;
    command << "cmd /C call title \"" << title << "\" && " << escapedString;
    if(waitOnExit) {
        command << " && echo \"\" & pause";
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
