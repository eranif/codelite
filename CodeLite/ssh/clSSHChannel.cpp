#include "clSSHChannel.hpp"

#include <wx/msgqueue.h>

#if USE_SFTP
#include "clJoinableThread.h"
#include "clModuleLogger.hpp"
#include "clRemoteHost.hpp"
#include "cl_exception.h"
#include "cl_remote_executor.hpp"

#include <chrono>
#include <libssh/libssh.h>
#include <thread>

//===-------------------------------------------------------------
// This thread is used when requesting a non interactive command
//===-------------------------------------------------------------
class clSSHChannelReader : public clJoinableThread
{
    wxEvtHandler* m_handler;
    SSHChannel_t m_channel;
    bool m_wantStderr = false;

public:
    clSSHChannelReader(wxEvtHandler* handler, SSHChannel_t channel, bool wantStderr)
        : m_handler(handler)
        , m_channel(channel)
        , m_wantStderr(false)
    {
    }
    virtual ~clSSHChannelReader() {}

    void* Entry()
    {
        size_t successfull_consecutive_stdout_calls = 0;
        while(!TestDestroy()) {
            // Poll the channel for output
            auto stdout_res = ssh::channel_read(m_channel, m_handler, false, m_wantStderr, 1);
            if(stdout_res == ssh::read_result::SSH_SUCCESS) {
                ++successfull_consecutive_stdout_calls;
                // got something, sleep a bit and let other process events as well
                if(successfull_consecutive_stdout_calls == 2) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    successfull_consecutive_stdout_calls = 0;
                }
                continue;
            }

            // reset the consecutive calls counter
            successfull_consecutive_stdout_calls = 0;

            // if we got an error, do not attempt to read stderr
            if(!ssh::result_ok(stdout_res)) {
                break;
            }

            auto stderrr_res = ssh::channel_read(m_channel, m_handler, true, m_wantStderr, 1);
            if(stderrr_res == ssh::read_result::SSH_SUCCESS) {
                // got something
                continue;
            }

            if(!ssh::result_ok(stdout_res)) {
                // error occured
                break;
            }
        }
        return NULL;
    }
};

#include "clModuleLogger.hpp"
INITIALISE_SSH_LOG(EXECLOG, "clSSHChannel");

//===-------------------------------------------------------------
// The SSH channel
//===-------------------------------------------------------------
clSSHChannel::clSSHChannel(clRemoteExecutor* parent, clSSH::Ptr_t ssh, bool wantStderrEvents)
    : m_parent(parent)
    , m_ssh(ssh)
    , m_wantStderr(wantStderrEvents)
{
    Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &clSSHChannel::OnReadError, this);
    Bind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clSSHChannel::OnWriteError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clSSHChannel::OnReadOutput, this);
    Bind(wxEVT_SSH_CHANNEL_READ_STDERR, &clSSHChannel::OnReadStderr, this);
    Bind(wxEVT_SSH_CHANNEL_CLOSED, &clSSHChannel::OnChannelClosed, this);
}

clSSHChannel::~clSSHChannel()
{
    Unbind(wxEVT_SSH_CHANNEL_READ_ERROR, &clSSHChannel::OnReadError, this);
    Unbind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clSSHChannel::OnWriteError, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clSSHChannel::OnReadOutput, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_STDERR, &clSSHChannel::OnReadStderr, this);
    Unbind(wxEVT_SSH_CHANNEL_CLOSED, &clSSHChannel::OnChannelClosed, this);
    Close();
}

bool clSSHChannel::Open()
{
    if(IsOpen()) {
        return false;
    }
    if(!m_ssh) {
        LOG_WARNING(EXECLOG) << "SSH is not opened" << endl;
        return false;
    }
    m_channel = ssh_channel_new(m_ssh->GetSession());
    if(!m_channel) {
        LOG_WARNING(EXECLOG) << "Failed to allocte ssh channel" << endl;
        return false;
    }

    int rc = ssh_channel_open_session(m_channel);
    if(rc != SSH_OK) {
        ssh_channel_free(m_channel);
        m_channel = NULL;
        LOG_WARNING(EXECLOG) << BuildError("Failed to open ssh channel") << endl;
        return false;
    }
    return true;
}

void clSSHChannel::Close()
{
    // Stop the worker thread
    wxDELETE(m_thread);

    if(IsOpen()) {
        ssh_channel_close(m_channel);
        ssh_channel_free(m_channel);
        m_channel = NULL;
    }

    // put back the ssh session
    clRemoteHost::Instance()->AddSshSession(m_ssh);

    // clear the local copy
    m_ssh.reset();
}

bool clSSHChannel::Execute(const wxString& command, execute_callback&& cb)
{
    // Sanity
    if(m_thread) {
        LOG_WARNING(EXECLOG) << "Channel is busy" << endl;
        return false;
    }

    if(!IsOpen()) {
        LOG_WARNING(EXECLOG) << "Execute error: channel is not opened" << endl;
        return false;
    }

    int rc = ssh_channel_request_exec(m_channel, command.mb_str(wxConvUTF8).data());
    if(rc != SSH_OK) {
        Close();
        LOG_WARNING(EXECLOG) << BuildError("Execute failed") << endl;
        return false;
    }

    m_callback = std::move(cb);
    m_thread = new clSSHChannelReader(this, m_channel, m_wantStderr);
    m_thread->Start();
    return true;
}

wxString clSSHChannel::BuildError(const wxString& prefix) const
{
    if(!m_ssh) {
        return prefix;
    }
    wxString errmsg = ssh_get_error(m_ssh->GetSession());
    return wxString() << prefix << ". " << errmsg;
}

void clSSHChannel::OnReadError(clCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_callback) {
        m_callback("", clRemoteCommandStatus::DONE_WITH_ERROR);
        CallAfter(&clSSHChannel::Destroy);
    } else {
        clProcessEvent event_terminated{ wxEVT_ASYNC_PROCESS_TERMINATED };
        event_terminated.SetProcess(nullptr);
        AddPendingEvent(event_terminated);
    }
}

void clSSHChannel::OnWriteError(clCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_callback) {
        m_callback("", clRemoteCommandStatus::DONE_WITH_ERROR);
        CallAfter(&clSSHChannel::Destroy);
    } else {
        clProcessEvent event_terminated{ wxEVT_ASYNC_PROCESS_TERMINATED };
        event_terminated.SetProcess(nullptr);
        AddPendingEvent(event_terminated);
    }
}

void clSSHChannel::OnReadOutput(clCommandEvent& event)
{
    if(m_callback) {
        m_callback(event.GetStringRaw(), clRemoteCommandStatus::STDOUT);
    } else {
        clProcessEvent event_stdout{ wxEVT_ASYNC_PROCESS_OUTPUT };
        event_stdout.SetProcess(nullptr);
        event_stdout.SetOutputRaw(event.GetStringRaw());
        event_stdout.SetOutput(event.GetStringRaw());
        AddPendingEvent(event_stdout);
    }
}

void clSSHChannel::OnReadStderr(clCommandEvent& event)
{
    if(m_callback) {
        m_callback(event.GetStringRaw(), clRemoteCommandStatus::STDERR);
    } else {
        clProcessEvent event_stdout{ wxEVT_ASYNC_PROCESS_STDERR };
        event_stdout.SetProcess(nullptr);
        event_stdout.SetOutputRaw(event.GetStringRaw());
        event_stdout.SetOutput(event.GetStringRaw());
        AddPendingEvent(event_stdout);
    }
}

void clSSHChannel::OnChannelClosed(clCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_callback) {
        m_callback("", clRemoteCommandStatus::DONE);
        CallAfter(&clSSHChannel::Destroy);
    } else {
        clProcessEvent event_terminated{ wxEVT_ASYNC_PROCESS_TERMINATED };
        event_terminated.SetProcess(nullptr);
        AddPendingEvent(event_terminated);
    }
}

void clSSHChannel::SendSignal(wxSignal sig)
{
    if(!m_ssh) {
        LOG_WARNING(EXECLOG) << "SendSignal(): SSH is not opened" << endl;
        return;
    }

    if(!m_channel) {
        LOG_WARNING(EXECLOG) << "SendSignal(): Channel is not opened" << endl;
        return;
    }

    const char* prefix = nullptr;
    switch(sig) {
    case wxSIGABRT:
        prefix = "ABRT";
        break;
    case wxSIGALRM:
        prefix = "ALRM";
        break;
    case wxSIGFPE:
        prefix = "FPE";
        break;
    case wxSIGHUP:
        prefix = "HUP";
        break;
    case wxSIGILL:
        prefix = "ILL";
        break;
    case wxSIGINT:
        prefix = "INT";
        break;
    case wxSIGKILL:
        prefix = "KILL";
        break;
    case wxSIGPIPE:
        prefix = "PIPE";
        break;
    case wxSIGQUIT:
        prefix = "QUIT";
        break;
    case wxSIGSEGV:
        prefix = "SEGV";
        break;
    case wxSIGTERM:
        prefix = "TERM";
        break;
    default:
        break;
    }

    if(!prefix) {
        LOG_WARNING(EXECLOG) << "Requested to send an unknown signal:" << (int)sig << endl;
        return;
    }

    int rc = ssh_channel_request_send_signal(m_channel, prefix);
    if(rc != SSH_OK) {
        LOG_WARNING(EXECLOG) << "failed to send signal:" << prefix << BuildError("reason:") << endl;
    }
}

void clSSHChannel::Detach() {}

void clSSHChannel::Destroy()
{
    if(m_parent) {
        m_parent->Delete(this);
    }
}

#endif
