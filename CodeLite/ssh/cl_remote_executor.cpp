#if USE_SFTP
#include "ssh/cl_remote_executor.hpp"

#include "StringUtils.h"
#include "clModuleLogger.hpp"
#include "cl_standard_paths.h"
#include "processreaderthread.h"
#include "ssh/ssh_account_info.h"

#include <thread>

namespace
{
clModuleLogger LOG;
bool once = true;
const wxString TERMINATOR = "CODELITE_TERMINATOR";
} // namespace

clRemoteExecutor::clRemoteExecutor()
{
    if(once) {
        wxFileName logfile{ clStandardPaths::Get().GetUserDataDir(), "remote-executor.log" };
        logfile.AppendDir("logs");
        LOG.Open(logfile.GetFullPath());
        once = true;
    }

    Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &clRemoteExecutor::OnChannelError, this);
    Bind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clRemoteExecutor::OnChannelError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clRemoteExecutor::OnChannelStdout, this);
    Bind(wxEVT_SSH_CHANNEL_READ_STDERR, &clRemoteExecutor::OnChannelStderr, this);
    Bind(wxEVT_SSH_CHANNEL_CLOSED, &clRemoteExecutor::OnChannelClosed, this);
}

clRemoteExecutor::~clRemoteExecutor()
{
    shutdown();
    Unbind(wxEVT_SSH_CHANNEL_READ_ERROR, &clRemoteExecutor::OnChannelError, this);
    Unbind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clRemoteExecutor::OnChannelError, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clRemoteExecutor::OnChannelStdout, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_STDERR, &clRemoteExecutor::OnChannelStderr, this);
    Unbind(wxEVT_SSH_CHANNEL_CLOSED, &clRemoteExecutor::OnChannelClosed, this);
}

bool clRemoteExecutor::startup(const wxString& account_name)
{
    if(m_ssh) {
        return true;
    }

    auto account = SSHAccountInfo::LoadAccount(account_name);
    if(account.GetHost().empty()) {
        return false;
    }

    /// open channel
    try {
        m_ssh.reset(new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()));
        wxString message;

        m_ssh->Connect();
        if(!m_ssh->AuthenticateServer(message)) {
            m_ssh->AcceptServerAuthentication();
        }
        m_ssh->Login();
    } catch(clException& e) {
        LOG_ERROR(LOG) << "Failed to open ssh channel to account:" << account.GetAccountName() << "." << e.What()
                       << endl;
        m_channel.reset(nullptr);
        return false;
    }
    return true;
}

void clRemoteExecutor::shutdown()
{
    if(m_channel) {
        m_channel->Close();
        m_channel.reset(nullptr);
        m_ssh.reset();
    }
}

bool clRemoteExecutor::try_execute(const clRemoteExecutor::Cmd& cmd)
{
    wxString command;
    if(!m_ssh) {
        LOG_WARNING(LOG) << "SSH session is not opened" << endl;
        return false;
    }

    // open the channel
    try {
        m_channel.reset(new clSSHChannel(m_ssh, clSSHChannel::kRemoteCommand, this, true));
        m_channel->Open();
    } catch(clException& e) {
        LOG_ERROR(LOG) << "failed to open channel." << e.What() << endl;
        return false;
    }

    if(!cmd.env.empty()) {
        // build each env in its own "export" statement
        for(const auto& e : cmd.env) {
            command << "export " << e.first << "=" << e.second << ";";
        }
    }

    if(!cmd.wd.empty()) {
        command << "cd " << StringUtils::WrapWithDoubleQuotes(cmd.wd) << " && ";
    }

    for(const wxString& c : cmd.command) {
        command << StringUtils::WrapWithDoubleQuotes(c) << " ";
    }

    if(command.EndsWith(" ")) {
        command.RemoveLast();
    }

    LOG_DEBUG(LOG) << "Executing command:" << command << endl;

    // prepare the commands

    try {
        m_channel->Execute(command);
        LOG_DEBUG(LOG) << "Success" << endl;
        return true;
    } catch(clException& e) {
        LOG_TRACE(LOG) << "channel is busy, try again later" << endl;
    }
    return false;
}

void clRemoteExecutor::OnChannelStdout(clCommandEvent& event)
{
    m_stdout.append(event.GetStringRaw());
    LOG_DEBUG(LOG) << m_stdout << endl;
}

void clRemoteExecutor::OnChannelStderr(clCommandEvent& event)
{
    m_stderr.append(event.GetStringRaw());
    LOG_DEBUG(LOG) << m_stderr << endl;
}

void clRemoteExecutor::OnChannelClosed(clCommandEvent& event)
{
    LOG_DEBUG(LOG) << "remote command completed" << endl;

    if(!m_stdout.empty()) {
        clProcessEvent stdout_event{ wxEVT_ASYNC_PROCESS_OUTPUT };
        stdout_event.SetStringRaw(m_stdout);
        LOG_DEBUG(LOG) << "read remote stdout:" << endl;
        LOG_DEBUG(LOG) << m_stdout << endl;
        ProcessEvent(stdout_event);
    }

    if(!m_stderr.empty()) {
        clProcessEvent stderr_event{ wxEVT_ASYNC_PROCESS_STDERR };
        stderr_event.SetStringRaw(m_stderr);
        LOG_DEBUG(LOG) << "read remote stderr:" << endl;
        LOG_DEBUG(LOG) << m_stderr << endl;
        ProcessEvent(stderr_event);
    }
    m_stdout.clear();
    m_stderr.clear();
    m_channel.reset();

    clProcessEvent command_ended{ wxEVT_ASYNC_PROCESS_TERMINATED };
    ProcessEvent(command_ended);
}

void clRemoteExecutor::OnChannelError(clCommandEvent& event)
{
    wxUnusedVar(event);
    clProcessEvent command_ended{ wxEVT_ASYNC_PROCESS_TERMINATED };
    ProcessEvent(command_ended);
    m_channel.reset();
}

#endif // USE_SFTP
