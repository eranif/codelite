#include "DapProcess.hpp"

DapProcess::DapProcess(wxEvtHandler* notifier)
    : m_notifier(notifier)
{
}

DapProcess::~DapProcess() {}

void DapProcess::OnTerminate(int pid, int status)
{
    if(m_notifier) {
        wxProcessEvent event_terminated{ wxEVT_END_PROCESS };
        event_terminated.m_pid = pid;
        event_terminated.m_exitcode = status;
        m_notifier->AddPendingEvent(event_terminated);
    }
    delete this;
}
