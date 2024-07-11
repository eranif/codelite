#include "clCommandProcessor.h"

#include "processreaderthread.h"
#include "cl_command_event.h"

wxDEFINE_EVENT(wxEVT_COMMAND_PROCESSOR_ENDED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_PROCESSOR_OUTPUT, clCommandEvent);

clCommandProcessor::clCommandProcessor(const wxString& command, const wxString& wd, size_t processFlags)
    : m_next(NULL)
    , m_prev(NULL)
    , m_process(NULL)
    , m_command(command)
    , m_workingDirectory(wd)
    , m_processFlags(processFlags)
    , m_postExecCallback(NULL)
    , m_obj(NULL)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clCommandProcessor::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clCommandProcessor::OnProcessTerminated, this);
    
}

clCommandProcessor::~clCommandProcessor() { wxDELETE(m_process); }

void clCommandProcessor::ExecuteCommand()
{
    wxString message;
    message << _("Executing: ") << m_command << " [ wd: " << m_workingDirectory << " ]";

    clCommandEvent eventStart(wxEVT_COMMAND_PROCESSOR_OUTPUT);
    eventStart.SetString(message);
    GetFirst()->ProcessEvent(eventStart);
    
    m_output.Clear();
    m_process = ::CreateAsyncProcess(this, m_command, m_processFlags, m_workingDirectory);
    if(!m_process) {
        clCommandEvent eventEnd(wxEVT_COMMAND_PROCESSOR_ENDED);
        eventEnd.SetString(wxString::Format(_("Failed to execute command: %s"), m_command));
        GetFirst()->ProcessEvent(eventEnd);
        DeleteChain();
    }
    m_process->SetHardKill(true);
}

void clCommandProcessor::OnProcessOutput(clProcessEvent& event)
{
    clCommandEvent eventStart(wxEVT_COMMAND_PROCESSOR_OUTPUT);
    m_output << event.GetOutput();
    eventStart.SetString(event.GetOutput());
    GetFirst()->ProcessEvent(eventStart);
    if(eventStart.GetString() != event.GetOutput()) {
        // user provided some input, write it to the running process
        m_process->WriteToConsole(eventStart.GetString());
    }
}

void clCommandProcessor::OnProcessTerminated(clProcessEvent& event)
{
    if(m_obj && m_postExecCallback) {
        // Call the user callback, if the user returns false
        // stop the processor
        if(!(m_obj->*m_postExecCallback)(this)) {
            clCommandEvent eventEnd(wxEVT_COMMAND_PROCESSOR_ENDED);
            GetFirst()->ProcessEvent(eventEnd);
            DeleteChain();
            return;
        }
    }

    if(m_next) {
        wxDELETE(m_process);
        // more commands, don't report an 'END' event
        m_next->ExecuteCommand();

    } else {
        // no more commands to execute, delete the entire chain
        clCommandEvent eventEnd(wxEVT_COMMAND_PROCESSOR_ENDED);
        GetFirst()->ProcessEvent(eventEnd);
        DeleteChain();
    }
}

clCommandProcessor* clCommandProcessor::Link(clCommandProcessor* next)
{
    this->m_next = next;
    if(m_next) {
        m_next->m_prev = this;
    }
    return next;
}

void clCommandProcessor::DeleteChain()
{
    // Move to the first one in the list
    clCommandProcessor* first = GetFirst();

    // delete
    while(first) {
        clCommandProcessor* next = first->m_next;
        wxDELETE(first);
        first = next;
    }
}

clCommandProcessor* clCommandProcessor::GetFirst()
{
    clCommandProcessor* first = this;
    while(first->m_prev) {
        first = first->m_prev;
    }
    return first;
}

void clCommandProcessor::Terminate()
{
    clCommandProcessor* first = GetFirst();
    while(first) {
        if(first->m_process) {
            first->m_process->Terminate();
            break;
        }
        first = first->m_next;
    }
}

clCommandProcessor* clCommandProcessor::GetActiveProcess()
{
    clCommandProcessor* first = GetFirst();
    while(first) {
        if(first->m_process) {
            return first;
        }
        first = first->m_next;
    }
    return NULL;
}
