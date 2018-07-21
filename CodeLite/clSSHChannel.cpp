#include "clSSHChannel.h"
#if USE_SFTP
#include "clJoinableThread.h"
#include "cl_exception.h"
#include <libssh/libssh.h>

wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_OUTPUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_CLOSED, clCommandEvent);

class clSSHChannelReader : public clJoinableThread
{
    wxEvtHandler* m_handler;
    SSHChannel_t m_channel;

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
            int bytes = ssh_channel_poll_timeout(m_channel, 500, 0);
            if(bytes == SSH_ERROR) {
                // an error
                clCommandEvent event(wxEVT_SSH_CHANNEL_READ_ERROR);
                m_handler->AddPendingEvent(event);
                break;
            } else if(bytes == SSH_EOF) {
                clCommandEvent event(wxEVT_SSH_CHANNEL_CLOSED);
                m_handler->AddPendingEvent(event);
                break;
            } else if(bytes == 0) {
                continue;
            } else {
                // there is something to read
                char* buffer = new char[bytes + 1];
                if(ssh_channel_read(m_channel, buffer, bytes, 0) != bytes) {
                    clCommandEvent event(wxEVT_SSH_CHANNEL_READ_ERROR);
                    m_handler->AddPendingEvent(event);
                    wxDELETEA(buffer);
                    break;
                } else {
                    buffer[bytes] = 0;
                    clCommandEvent event(wxEVT_SSH_CHANNEL_READ_OUTPUT);
                    event.SetString(wxString(buffer, wxConvUTF8));
                    m_handler->AddPendingEvent(event);
                    wxDELETEA(buffer);
                }
            }
        }
        return NULL;
    }
};

clSSHChannel::clSSHChannel(clSSH::Ptr_t ssh)
    : m_ssh(ssh)
    , m_channel(nullptr)
    , m_readerThread(nullptr)
{
}

clSSHChannel::~clSSHChannel() { Close(); }

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
}

void clSSHChannel::Close()
{
    // Stop the worker thread
    wxDELETE(m_readerThread);
    
    if(IsOpen()) {
        ssh_channel_close(m_channel);
        ssh_channel_free(m_channel);
        m_channel = NULL;
    }
}

void clSSHChannel::Execute(const wxString& command, wxEvtHandler* sink)
{
    // Sanity
    if(m_readerThread) { throw clException("Channel is busy"); }
    if(!IsOpen()) { throw clException("Channel is not opened"); }
    int rc = ssh_channel_request_exec(m_channel, command.mb_str(wxConvUTF8).data());
    if(rc != SSH_OK) {
        Close();
        throw clException(BuildError("Execute failed"));
    }
    m_readerThread = new clSSHChannelReader(sink, m_channel);
    m_readerThread->Start();
}

wxString clSSHChannel::BuildError(const wxString& prefix) const
{
    if(!m_ssh) { return prefix; }
    wxString errmsg = ssh_get_error(m_ssh->GetSession());
    return wxString() << prefix << ". " << errmsg;
}
#endif
