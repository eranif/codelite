#include "StringUtils.h"
#include "clSSHChannel.h"
#include <wx/msgqueue.h>
#include <wx/regex.h>
#if USE_SFTP
#include "clJoinableThread.h"
#include "cl_exception.h"
#include <libssh/libssh.h>

wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_WRITE_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_OUTPUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_CLOSED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_PTY, clCommandEvent);

//===-------------------------------------------------------------
// This thread is used when requesting a non interactive command
//===-------------------------------------------------------------
class clSSHChannelReader : public clJoinableThread
{
    wxEvtHandler* m_handler;
    SSHChannel_t m_channel;

protected:
    bool ReadChannel(bool isStderr)
    {
        int nStderr = isStderr ? 1 : 0;
        int bytes = ssh_channel_poll_timeout(m_channel, 50, nStderr);
        if(bytes == SSH_ERROR) {
            // an error
            clCommandEvent event(wxEVT_SSH_CHANNEL_READ_ERROR);
            m_handler->QueueEvent(event.Clone());
            return false;
        } else if(bytes == SSH_EOF) {
            // channel closed
            clCommandEvent event(wxEVT_SSH_CHANNEL_CLOSED);
            m_handler->QueueEvent(event.Clone());
            return false;
        } else if(bytes == 0) {
            // timeout
            return true;
        } else {
            // there is something to read
            char* buffer = new char[bytes + 1];
            if(ssh_channel_read(m_channel, buffer, bytes, nStderr) != bytes) {
                clCommandEvent event(wxEVT_SSH_CHANNEL_READ_ERROR);
                m_handler->QueueEvent(event.Clone());
                wxDELETEA(buffer);
                return false;
            } else {
                buffer[bytes] = 0;
                clCommandEvent event(wxEVT_SSH_CHANNEL_READ_OUTPUT);
                event.SetString(wxString(buffer, wxConvUTF8));
                m_handler->QueueEvent(event.Clone());
                wxDELETEA(buffer);
            }
        }
        return true;
    }

public:
    clSSHChannelReader(wxEvtHandler* handler, SSHChannel_t channel)
        : m_handler(handler)
        , m_channel(channel)
    {
    }
    virtual ~clSSHChannelReader() {}

    void* Entry()
    {
        while(!TestDestroy()) {
            // First, poll the channel
            if(!ReadChannel(false) || !ReadChannel(true)) { break; }
        }
        return NULL;
    }
};

//===-------------------------------------------------------------
// This thread is used when requesting an interactive shell
//===-------------------------------------------------------------
class clSSHChannelInteractiveThread : public clJoinableThread
{
    wxEvtHandler* m_handler;
    SSHChannel_t m_channel;
    wxMessageQueue<wxString>& m_Queue;
    char m_buffer[4096];
    wxRegEx m_reTTY;

public:
    clSSHChannelInteractiveThread(wxEvtHandler* handler, SSHChannel_t channel, wxMessageQueue<wxString>& Q)
        : m_handler(handler)
        , m_channel(channel)
        , m_Queue(Q)
    {
        m_reTTY.Compile("tty=([a-z/0-9]+)");
    }
    virtual ~clSSHChannelInteractiveThread() {}

protected:
    bool ReadChannel(bool isStderr)
    {
        // First, poll the channel
        m_buffer[0] = 0;
        int bytes = ssh_channel_read_nonblocking(m_channel, m_buffer, sizeof(m_buffer) - 1, 0);
        if(bytes == SSH_ERROR) {
            // an error
            clCommandEvent event(wxEVT_SSH_CHANNEL_READ_ERROR);
            m_handler->QueueEvent(event.Clone());
            return false;
        } else if(bytes == 0) {
            // 0 is a valid output
            // but we need to check for EOF
            if(ssh_channel_is_eof(m_channel)) {
                clCommandEvent event(wxEVT_SSH_CHANNEL_CLOSED);
                m_handler->QueueEvent(event.Clone());
                return false;
            } else {
                // timeout occured
                return true;
            }
        } else {
            // we read something...
            m_buffer[bytes] = 0;
            wxString messageRead(m_buffer, wxConvUTF8);

            // strip colours from the output
            StringUtils::StripTerminalColouring(messageRead, messageRead);

            if(m_reTTY.IsValid() && m_reTTY.Matches(messageRead)) {
                wxString wholeMatch = m_reTTY.GetMatch(messageRead, 0);
                clCommandEvent event(wxEVT_SSH_CHANNEL_PTY);
                wxString tty = m_reTTY.GetMatch(messageRead, 1);
                event.SetString(tty);
                m_handler->QueueEvent(event.Clone());
            }

            if(!messageRead.IsEmpty()) {
                clCommandEvent event(wxEVT_SSH_CHANNEL_READ_OUTPUT);
                event.SetString(messageRead);
                m_handler->QueueEvent(event.Clone());
            }
            return true;
        }
    }

public:
    void* Entry()
    {
        while(!TestDestroy()) {
            if(!ReadChannel(false) || !ReadChannel(true)) { break; }
            
            // Check if we got something to write
            wxString message;
            bool write_error = false;
            while(!write_error && (m_Queue.ReceiveTimeout(20, message) == wxMSGQUEUE_NO_ERROR)) {
                message.Trim().Trim(false);
                message << "\n";
                std::string pbuffer = message.mb_str(wxConvISO8859_1).data();
                if(ssh_channel_write(m_channel, pbuffer.c_str(), pbuffer.length()) == SSH_ERROR) {
                    clCommandEvent event(wxEVT_SSH_CHANNEL_WRITE_ERROR);
                    m_handler->AddPendingEvent(event);
                    write_error = true;
                    break;
                }
            }
            if(write_error) { break; }
        }
        return NULL;
    }
};

//===-------------------------------------------------------------
// The SSH channel
//===-------------------------------------------------------------
clSSHChannel::clSSHChannel(clSSH::Ptr_t ssh, clSSHChannel::eChannelType type, wxEvtHandler* owner)
    : m_ssh(ssh)
    , m_owner(owner)
    , m_type(type)
{
    Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &clSSHChannel::OnReadError, this);
    Bind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clSSHChannel::OnWriteError, this);
    Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clSSHChannel::OnReadOutput, this);
    Bind(wxEVT_SSH_CHANNEL_CLOSED, &clSSHChannel::OnChannelClosed, this);
    Bind(wxEVT_SSH_CHANNEL_PTY, &clSSHChannel::OnChannelPty, this);
}

clSSHChannel::~clSSHChannel()
{
    Unbind(wxEVT_SSH_CHANNEL_READ_ERROR, &clSSHChannel::OnReadError, this);
    Unbind(wxEVT_SSH_CHANNEL_WRITE_ERROR, &clSSHChannel::OnWriteError, this);
    Unbind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &clSSHChannel::OnReadOutput, this);
    Unbind(wxEVT_SSH_CHANNEL_CLOSED, &clSSHChannel::OnChannelClosed, this);
    Unbind(wxEVT_SSH_CHANNEL_PTY, &clSSHChannel::OnChannelPty, this);
    Close();
}

void clSSHChannel::Open()
{
    if(IsOpen()) { return; }
    if(!m_ssh) { throw clException("ssh session is not opened"); }
    m_channel = ssh_channel_new(m_ssh->GetSession());
    if(!m_channel) { throw clException(BuildError("Failed to allocte ssh channel")); }

    int rc = ssh_channel_open_session(m_channel);
    if(rc != SSH_OK) {
        ssh_channel_free(m_channel);
        m_channel = NULL;
        throw clException(BuildError("Failed to open ssh channel"));
    }

    // For an interactive channel, we also need pty
    if(IsInteractive()) {
        rc = ssh_channel_request_pty(m_channel);
        if(rc != SSH_OK) {
            ssh_channel_free(m_channel);
            m_channel = NULL;
            throw clException(BuildError("Failed to request pty"));
        }

        rc = ssh_channel_change_pty_size(m_channel, 1024, 24);
        if(rc != SSH_OK) {
            ssh_channel_free(m_channel);
            m_channel = NULL;
            throw clException(BuildError("Failed to change SSH pty size"));
        }

        rc = ssh_channel_request_shell(m_channel);
        if(rc != SSH_OK) {
            ssh_channel_free(m_channel);
            m_channel = NULL;
            throw clException(BuildError("Failed to request SSH shell"));
        }

        // Open the helper thread
        m_thread = new clSSHChannelInteractiveThread(this, m_channel, m_Queue);
        m_thread->Start();

        // Ask for the tty
        DoWrite("echo tty=`tty`");
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
    if(IsInteractive()) {
        DoWrite(command);

    } else {
        // Sanity
        if(m_thread) { throw clException("Channel is busy"); }
        if(!IsOpen()) { throw clException("Channel is not opened"); }
        int rc = ssh_channel_request_exec(m_channel, command.mb_str(wxConvUTF8).data());
        if(rc != SSH_OK) {
            Close();
            throw clException(BuildError("Execute failed"));
        }
        m_thread = new clSSHChannelReader(this, m_channel);
        m_thread->Start();
    }
}

wxString clSSHChannel::BuildError(const wxString& prefix) const
{
    if(!m_ssh) { return prefix; }
    wxString errmsg = ssh_get_error(m_ssh->GetSession());
    return wxString() << prefix << ". " << errmsg;
}

void clSSHChannel::Write(const wxString& message)
{
    if(IsInteractive()) { throw clException("Write is only available for interactive ssh channels"); }
    if(!IsOpen()) { throw clException("Channel is not opened"); }
    DoWrite(message);
}

void clSSHChannel::DoWrite(const wxString& buffer)
{
    if(IsInteractive()) { throw clException("Write is only available for interactive ssh channels"); }
    m_Queue.Post(buffer);
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

void clSSHChannel::OnChannelClosed(clCommandEvent& event) { m_owner->AddPendingEvent(event); }

void clSSHChannel::OnChannelPty(clCommandEvent& event) { m_owner->AddPendingEvent(event); }

void clSSHChannel::SendSignal(wxSignal sig)
{
    if(!m_ssh) { throw clException("ssh session is not opened"); }
    if(!m_channel) { throw clException("ssh channel is not opened"); }

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
    if(!prefix) { throw clException("Requested to send an unknown signal"); }
    int rc = ssh_channel_request_send_signal(m_channel, prefix);
    if(rc != SSH_OK) { throw clException(BuildError("Failed to send signal")); }
}
#endif
