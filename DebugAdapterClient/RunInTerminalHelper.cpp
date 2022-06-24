#include "RunInTerminalHelper.hpp"

#include "event_notifier.h"

RunInTerminalHelper::RunInTerminalHelper(clModuleLogger& log)
    : LOG(log)
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &RunInTerminalHelper::OnProcessTerminated, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &RunInTerminalHelper::OnDebugEnded, this);
}

RunInTerminalHelper::~RunInTerminalHelper()
{
    // dont accept events anymore
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &RunInTerminalHelper::OnProcessTerminated, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &RunInTerminalHelper::OnDebugEnded, this);
    if(m_process) {
        // dont send us events
        m_process->Detach();
        wxDELETE(m_process);
    }
}

void RunInTerminalHelper::OnProcessTerminated(clProcessEvent& event)
{
    LOG_DEBUG(LOG) << "Helper process terminated!" << endl;
    wxDELETE(m_process);
}

int RunInTerminalHelper::RunProcess(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env)
{
    if(m_process) {
        return wxNOT_FOUND;
    }

    m_processId = wxNOT_FOUND;
    LOG_DEBUG(LOG) << "Starting run-in-terminal process:" << command << endl;
    LOG_DEBUG(LOG) << "wd:" << wd << endl;

    m_process = ::CreateAsyncProcess(this, command, IProcessCreateConsole | IProcessNoRedirect, wd, &env);
    if(m_process) {
        m_processId = m_process->GetPid();
        LOG_WARNING(LOG) << "Helper process launched successfully P:" << m_processId << endl;
    } else {
        LOG_WARNING(LOG) << "Failed to launch helper process:" << command << endl;
    }
    return m_processId;
}

void RunInTerminalHelper::Terminate()
{
    if(m_process) {
        m_process->Terminate();
    }
}

void RunInTerminalHelper::OnDebugEnded(clDebugEvent& event)
{
    event.Skip();
    Terminate();
}
