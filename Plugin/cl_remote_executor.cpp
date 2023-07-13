#if USE_SFTP
#include "cl_remote_executor.hpp"

#include "StringUtils.h"
#include "clModuleLogger.hpp"
#include "clRemoteHost.hpp"
#include "cl_standard_paths.h"
#include "processreaderthread.h"
#include "ssh/ssh_account_info.h"

#include <thread>

INITIALISE_SSH_LOG(LOG, "clRemoteExecutor");

clRemoteExecutor::clRemoteExecutor()
{
    Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &clRemoteExecutor::OnChannelError, this);
    Bind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clRemoteExecutor::OnChannelError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clRemoteExecutor::OnChannelStdout, this);
    Bind(wxEVT_SSH_CHANNEL_READ_STDERR, &clRemoteExecutor::OnChannelStderr, this);
    Bind(wxEVT_SSH_CHANNEL_CLOSED, &clRemoteExecutor::OnChannelClosed, this);
}

clRemoteExecutor::~clRemoteExecutor()
{
    Unbind(wxEVT_SSH_CHANNEL_READ_ERROR, &clRemoteExecutor::OnChannelError, this);
    Unbind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clRemoteExecutor::OnChannelError, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clRemoteExecutor::OnChannelStdout, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_STDERR, &clRemoteExecutor::OnChannelStderr, this);
    Unbind(wxEVT_SSH_CHANNEL_CLOSED, &clRemoteExecutor::OnChannelClosed, this);
}

IProcess::Ptr_t clRemoteExecutor::try_execute(const clRemoteExecutor::Cmd& cmd)
{
    auto ssh_session = clRemoteHost::Instance()->TakeSession();
    if(!ssh_session) {
        LOG_WARNING(LOG) << "SSH session is not opened" << endl;
        return nullptr;
    }

    // open the channel
    wxString command = ssh::build_command(cmd.command, cmd.wd, cmd.env);
    IProcess::Ptr_t proc = clSSHChannel::Execute(
        ssh_session, [](clSSH::Ptr_t ssh) { clRemoteHost::Instance()->AddSshSession(ssh); }, this, command, true);

    if(!proc) {
        LOG_ERROR(LOG) << "failed to start remote command:" << command << endl;
        return nullptr;
    }
    return proc;
}

void clRemoteExecutor::OnChannelStdout(clCommandEvent& event)
{
    clProcessEvent output_event{ wxEVT_ASYNC_PROCESS_OUTPUT };
    output_event.SetStringRaw(event.GetStringRaw());
    LOG_DEBUG(LOG) << "stdout read:" << event.GetStringRaw().size() << "bytes" << endl;
    ProcessEvent(output_event);
}

void clRemoteExecutor::OnChannelStderr(clCommandEvent& event)
{
    clProcessEvent output_event{ wxEVT_ASYNC_PROCESS_STDERR };
    output_event.SetStringRaw(event.GetStringRaw());
    LOG_DEBUG(LOG) << "stderr read:" << event.GetStringRaw().size() << "bytes" << endl;
    ProcessEvent(output_event);
}

void clRemoteExecutor::OnChannelClosed(clCommandEvent& event)
{
    LOG_DEBUG(LOG) << "remote channel closed" << endl;

    clProcessEvent output_event{ wxEVT_ASYNC_PROCESS_TERMINATED };
    output_event.SetInt(event.GetInt());
    ProcessEvent(output_event);
}

void clRemoteExecutor::OnChannelError(clCommandEvent& event)
{
    wxUnusedVar(event);
    clProcessEvent command_ended{ wxEVT_ASYNC_PROCESS_TERMINATED };
    command_ended.SetInt(event.GetInt()); // exit code
    ProcessEvent(command_ended);
}

#endif // USE_SFTP
