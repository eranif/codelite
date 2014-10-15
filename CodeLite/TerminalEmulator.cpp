#include "TerminalEmulator.h"
#include <wx/filename.h>
#include <wx/log.h>

#ifndef __WXMSW__
#include <signal.h>
#endif 

wxDEFINE_EVENT(wxEVT_TERMINAL_COMMAND_EXIT, clCommandEvent);

TerminalEmulator::TerminalEmulator()
    : m_process(NULL)
    , m_pid(wxNOT_FOUND)
{
}

TerminalEmulator::~TerminalEmulator() {}

bool TerminalEmulator::Execute(const wxString& command,
                               const wxString& workingDirectory,
                               bool waitOnExit,
                               const wxString& title)
{
    if(m_process) {
        // another process is running
        return false;
    }

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
    if(wxFileName::Exists("/usr/bin/gnome-terminal")) {
        consoleCommand << "/usr/bin/gnome-terminal -t " << strTitle << " -x "
                       << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/konsole")) {
        consoleCommand << "/usr/bin/konsole -e " << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/uxterm")) {
        consoleCommand << "/usr/bin/uxterm -T " << strTitle << " -e " << PrepareCommand(command, strTitle, waitOnExit);

    } else if(wxFileName::Exists("/usr/bin/xterm")) {
        consoleCommand << "/usr/bin/xterm -T " << strTitle << " -e " << PrepareCommand(command, strTitle, waitOnExit);
    }

#elif defined(__WXMAC__)

#endif
    if(consoleCommand.IsEmpty())
        return false;
    wxLogMessage(consoleCommand);

    m_process = new wxProcess(wxPROCESS_DEFAULT);
    m_process->Bind(wxEVT_END_PROCESS, &TerminalEmulator::OnProcessTerminated, this);
    m_pid = ::wxExecute(consoleCommand, wxEXEC_ASYNC, m_process);
    return m_pid > 0;
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
    command << "cmd /c call title " << title << " && " << escapedString;
    if(waitOnExit) {
        command << " && echo \"\" && pause";
    }
#else
// OSX

#endif
    return command;
}

void TerminalEmulator::OnProcessTerminated(wxProcessEvent& event)
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
        wxProcess::Kill(m_pid, wxSIGKILL, wxKILL_CHILDREN);
    }
}

bool TerminalEmulator::IsRunning() const { 
#if defined(__WXGTK__) || defined(__WXMAC__)
    return m_pid != wxNOT_FOUND && kill(m_pid, 0) == 0;
#else
    // On Windows we need to use different approach
    return true;
#endif
}
