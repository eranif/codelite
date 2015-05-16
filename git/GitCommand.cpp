#include "GitCommand.h"
#include "processreaderthread.h"
#include "asyncprocess.h"

GitCommand::GitCommand()
    : m_process(NULL)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitCommand::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitCommand::OnProcessTerminated, this);
}

GitCommand::~GitCommand()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitCommand::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitCommand::OnProcessTerminated, this);
}

void GitCommand::OnProcessOutput(clProcessEvent& event) { ProcessOutput(event.GetOutput()); }

void GitCommand::OnProcessTerminated(clProcessEvent& event) { ProcessTerminated(); }

bool GitCommand::Execute()
{
    if(IsRunning()) return false;
    m_process = ::CreateAsyncProcess(this, m_command, IProcessCreateConsole, m_workingDirectory);
    return (m_process != NULL);
}
