#include "GitCommandProcessor.h"
#include "git.h"
#include "GitConsole.h"

BEGIN_EVENT_TABLE(GitCommandProcessor, wxEvtHandler)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ, GitCommandProcessor::OnProcessOutput)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, GitCommandProcessor::OnProcessTerminated)
END_EVENT_TABLE()

GitCommandProcessor::GitCommandProcessor(GitPlugin* plugin,
                                         const wxString& command,
                                         const wxString& wd,
                                         size_t processFlags)
    : m_plugin(plugin)
    , m_next(NULL)
    , m_prev(NULL)
    , m_process(NULL)
    , m_command(command)
    , m_workingDirectory(wd)
    , m_processFlags(processFlags)
{
}

GitCommandProcessor::~GitCommandProcessor() { wxDELETE(m_process); }

void GitCommandProcessor::ExecuteCommand()
{
    m_output.Clear();
    m_plugin->GetManager()->ShowOutputPane("git");
    m_plugin->GetConsole()->AddText(m_command + " [wd: " + m_workingDirectory + "]");
    m_process = ::CreateAsyncProcess(this, m_command, m_processFlags, m_workingDirectory);
    if(!m_process) {
        m_plugin->GetConsole()->AddText(_("Git command execution failed!"));
        DeleteChain();
    }
}

void GitCommandProcessor::OnProcessOutput(wxCommandEvent& event)
{
    ProcessEventData* ped = (ProcessEventData*)event.GetClientData();
    m_output << ped->GetData();
    wxDELETE(ped);
}

void GitCommandProcessor::OnProcessTerminated(wxCommandEvent& event)
{
    m_plugin->CallAfter(&GitPlugin::OnCommandCompleted, m_output);
    if(m_next) {
        m_next->ExecuteCommand();
    } else {
        // no more commands to execute, delete the entire chain
        DeleteChain();
    }
}

GitCommandProcessor* GitCommandProcessor::Link(GitCommandProcessor* next)
{
    this->m_next = next;
    if(m_next) {
        m_next->m_prev = this;
    }
    return next;
}

void GitCommandProcessor::DeleteChain()
{
    // Move to the first one in the list
    GitCommandProcessor* first = this;
    while(first->m_prev) {
        first = first->m_prev;
    }
    
    // delete
    while(first) {
        GitCommandProcessor* next = first->m_next;
        wxDELETE(first);
        first = next;
    }
}
