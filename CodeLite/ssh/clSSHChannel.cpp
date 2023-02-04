#include "clSSHChannel.hpp"

#include <wx/msgqueue.h>

#if USE_SFTP
#include "clJoinableThread.h"
#include "clModuleLogger.hpp"
#include "cl_exception.h"

#include <libssh/libssh.h>

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
        while(!TestDestroy()) {
            // Poll the channel for output
            auto stdout_res = ssh::channel_read(m_channel, m_handler, false, m_wantStderr);
            if(stdout_res == ssh::read_result::SSH_SUCCESS) {
                // got something
                continue;
            }

            auto stderrr_res = ssh::channel_read(m_channel, m_handler, true, m_wantStderr);
            if(stderrr_res == ssh::read_result::SSH_SUCCESS) {
                // got something
                continue;
            }

            if(!ssh::result_ok(stdout_res) || !ssh::result_ok(stdout_res)) {
                // error occured (but not timeout)
                break;
            }
        }
        return NULL;
    }
};

//===-------------------------------------------------------------
// The SSH channel
//===-------------------------------------------------------------
clSSHChannel::clSSHChannel(clSSH::Ptr_t ssh, clSSHChannel::eChannelType type, wxEvtHandler* owner,
                           bool wantStderrEvents)
    : m_ssh(ssh)
    , m_owner(owner)
    , m_type(type)
    , m_wantStderr(wantStderrEvents)
{
    Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &clSSHChannel::OnReadError, this);
    Bind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clSSHChannel::OnWriteError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clSSHChannel::OnReadOutput, this);
    Bind(wxEVT_SSH_CHANNEL_READ_STDERR, &clSSHChannel::OnReadStderr, this);
    Bind(wxEVT_SSH_CHANNEL_CLOSED, &clSSHChannel::OnChannelClosed, this);
    Bind(wxEVT_SSH_CHANNEL_PTY, &clSSHChannel::OnChannelPty, this);
}

clSSHChannel::~clSSHChannel()
{
    Unbind(wxEVT_SSH_CHANNEL_READ_ERROR, &clSSHChannel::OnReadError, this);
    Unbind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clSSHChannel::OnWriteError, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clSSHChannel::OnReadOutput, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_STDERR, &clSSHChannel::OnReadStderr, this);
    Unbind(wxEVT_SSH_CHANNEL_CLOSED, &clSSHChannel::OnChannelClosed, this);
    Unbind(wxEVT_SSH_CHANNEL_PTY, &clSSHChannel::OnChannelPty, this);
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
        throw clException(BuildError("Failed to allocte ssh channel"));
    }

    int rc = ssh_channel_open_session(m_channel);
    if(rc != SSH_OK) {
        ssh_channel_free(m_channel);
        m_channel = NULL;
        throw clException(BuildError("Failed to open ssh channel"));
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
}

void clSSHChannel::Execute(const wxString& command)
{
    // Sanity
    if(m_thread) {
        throw clException("Channel is busy");
    }
    if(!IsOpen()) {
        throw clException("Channel is not opened");
    }
    int rc = ssh_channel_request_exec(m_channel, command.mb_str(wxConvUTF8).data());
    if(rc != SSH_OK) {
        Close();
        throw clException(BuildError("Execute failed"));
    }
    m_thread = new clSSHChannelReader(this, m_channel, m_wantStderr);
    m_thread->Start();
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

void clSSHChannel::SendSignal(wxSignal sig)
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
