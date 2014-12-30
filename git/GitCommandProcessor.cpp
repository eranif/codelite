#include "GitCommandProcessor.h"
#include "git.h"

BEGIN_EVENT_TABLE(GitCommandProcessor, wxEvtHandler)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ, GitCommandProcessor::OnProcessOutput)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, GitCommandProcessor::OnProcessTerminated)
END_EVENT_TABLE()

GitCommandProcessor::GitCommandProcessor(GitPlugin* plugin)
    : m_plugin(plugin)
    , m_process(NULL)
{
}

GitCommandProcessor::~GitCommandProcessor() { wxDELETE(m_process); }

void GitCommandProcessor::ExecuteCommand(const wxString& command, const wxString& workingDirectory)
{
    m_output.Clear();
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, workingDirectory);
    if(!m_process) {
        delete this;
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
    delete this;
}
