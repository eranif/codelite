#include "clSSHChannel.hpp"

#include <wx/msgqueue.h>

#if USE_SFTP
#include "clJoinableThread.h"
#include "clModuleLogger.hpp"
#include "clResult.hpp"
#include "clTempFile.hpp"
#include "cl_exception.h"
#include "cl_sftp.h"

#include <libssh/libssh.h>

//===-------------------------------------------------------------
// This thread is used when requesting a non interactive command
//===-------------------------------------------------------------

#include "clModuleLogger.hpp"
INITIALISE_MODULE_LOG(LOG, "clSSHChannel", "ssh.log");

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
        while(!TestDestroy()) {
            // Poll the channel for output
            auto stdout_res = ssh::channel_read(m_channel, m_handler, false, m_wantStderr);
            if(stdout_res == ssh::read_result::SSH_SUCCESS) {
                // got something
                continue;
            }

            // if we got an error, do not attempt to read stderr
            if(!ssh::result_ok(stdout_res)) {
                break;
            }

            auto stderrr_res = ssh::channel_read(m_channel, m_handler, true, m_wantStderr);
            if(stderrr_res == ssh::read_result::SSH_SUCCESS) {
                // got something
                continue;
            }

            if(!ssh::result_ok(stdout_res)) {
                // error occurred
                break;
            }
        }
        return NULL;
    }
};

//===-------------------------------------------------------------
// The SSH channel
//===-------------------------------------------------------------
clSSHChannel::clSSHChannel(clSSH::Ptr_t ssh, clSSHDeleterFunc deleter_cb, wxEvtHandler* owner, bool wantStderrEvents)
    : IProcess(owner)
    , m_ssh(ssh)
    , m_owner(owner)
    , m_wantStderr(wantStderrEvents)
    , m_deleter_cb(std::move(deleter_cb))
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

void clSSHChannel::Open()
{
    if(IsOpen()) {
        return;
    }
    if(!m_ssh) {
        throw clException("ssh session is not opened");
    }
    m_channel = ssh_channel_new(m_ssh->GetSession());
    if(!m_channel) {
        throw clException(BuildError("ssh_channel_new error."));
    }

    int rc = ssh_channel_open_session(m_channel);
    if(rc != SSH_OK) {
        ssh_channel_free(m_channel);
        m_channel = NULL;
        throw clException(BuildError("ssh_channel_open_session error."));
    }
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

    if(m_hadErrors) {
        // log this
        LOG_DEBUG(LOG()) << "ssh session had errors. discarding it" << endl;

    } else {
        // put back the ssh session
        m_deleter_cb(m_ssh);
    }
    // clear the local copy
    m_ssh.reset();
}

IProcess::Ptr_t clSSHChannel::CreateAndExecuteScript(clSSH::Ptr_t ssh, clSSHDeleterFunc deleter_cb, wxEvtHandler* owner,
                                                     const wxString& content, const wxString& script_path,
                                                     bool wantStderr)
{
    if(!ssh::write_remote_file_content(ssh, script_path, content)) {
        LOG_ERROR(LOG()) << "failed to write remote file:" << script_path << endl;
        return nullptr;
    }
    return Execute(ssh, std::move(deleter_cb), owner, script_path, wantStderr);
}

IProcess::Ptr_t clSSHChannel::Execute(clSSH::Ptr_t ssh, clSSHDeleterFunc deleter_cb, wxEvtHandler* owner,
                                      const wxString& command, bool wantStderr)
{
    clSSHChannel* channel = nullptr;
    try {
        channel = new clSSHChannel(ssh, std::move(deleter_cb), owner, wantStderr);
        channel->Open();
    } catch(clException& e) {
        LOG_ERROR(LOG()) << "failed to open channel." << e.What() << endl;
        wxDELETE(channel);
        return nullptr;
    }

    int rc = ssh_channel_request_exec(channel->m_channel, command.mb_str(wxConvUTF8).data());
    if(rc != SSH_OK) {
        // mark the channel + ssh session as "broken"
        channel->m_hadErrors = true;
        wxDELETE(channel);
        return nullptr;
    }

    channel->m_thread = new clSSHChannelReader(channel, channel->m_channel, channel->m_wantStderr);
    channel->m_thread->Start();
    return IProcess::Ptr_t{ channel };
}

wxString clSSHChannel::BuildError(const wxString& prefix)
{
    if(!m_ssh) {
        return prefix;
    }

    // if we reached here, it means we had errors - mark this channel as "errornous"
    m_hadErrors = true;
    wxString errmsg = ssh_get_error(m_ssh->GetSession());
    return wxString() << prefix << ". " << errmsg;
}

void clSSHChannel::OnReadError(clCommandEvent& event)
{
    event.SetString(BuildError("Read error"));
    m_owner->AddPendingEvent(event);
}

void clSSHChannel::OnWriteError(clCommandEvent& event)
{
    event.SetString(BuildError("Write error"));
    m_owner->AddPendingEvent(event);
}

void clSSHChannel::OnReadOutput(clCommandEvent& event) { m_owner->AddPendingEvent(event); }
void clSSHChannel::OnReadStderr(clCommandEvent& event) { m_owner->AddPendingEvent(event); }
void clSSHChannel::OnChannelClosed(clCommandEvent& event) { m_owner->AddPendingEvent(event); }
void clSSHChannel::OnChannelPty(clCommandEvent& event) { m_owner->AddPendingEvent(event); }

void clSSHChannel::Signal(wxSignal sig)
{
    if(!m_ssh) {
        throw clException("ssh session is not opened");
    }
    if(!m_channel) {
        throw clException("ssh channel is not opened");
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
        throw clException("Requested to send an unknown signal");
    }
    int rc = ssh_channel_request_send_signal(m_channel, prefix);
    if(rc != SSH_OK) {
        throw clException(BuildError("Failed to send signal"));
    }
}

void clSSHChannel::Detach() {}

#endif
