#include "clSSHChannel.h"
#if USE_SFTP
#include "cl_exception.h"
#include <libssh/libssh.h>

clSSHChannel::clSSHChannel(clSSH::Ptr_t ssh)
    : m_ssh(ssh)
    , m_channel(nullptr)
{
}

clSSHChannel::~clSSHChannel() { Close(); }

void clSSHChannel::Open()
{
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
    if(m_channel) {
        ssh_channel_close(m_channel);
        ssh_channel_free(m_channel);
        m_channel = NULL;
    }
}

void clSSHChannel::Execute(const wxString& command)
{
    // Sanity
    if(!IsOpen()) { throw clException("Channel is not opened"); }
    int rc = ssh_channel_request_exec(m_channel, command.mb_str(wxConvUTF8).data());
    if(rc != SSH_OK) {
        Close();
        throw clException(BuildError("Execute failed"));
    }
}

wxString clSSHChannel::BuildError(const wxString& prefix) const
{
    if(!m_ssh) { return prefix; }
    wxString errmsg = ssh_get_error(m_ssh->GetSession());
    return wxString() << prefix << ". " << errmsg;
}

bool clSSHChannel::DoRead(wxString& output, int timeout_ms)
{
    if(!IsOpen()) { throw clException("Channel is not opened"); }
    char buffer[256];
    int nbytes;
    nbytes = ssh_channel_read_timeout(m_channel, buffer, sizeof(buffer), 0, timeout_ms);
    if(nbytes > 0) {
        output << wxString(buffer, wxConvUTF8, nbytes);
        return true;
    } else if(nbytes == 0) {
        // timeout
        return false;
    } else {
        Close();
        throw clException(BuildError("Read failed"));
    }
}

bool clSSHChannel::ReadAll(wxString& output, int timeout_ms)
{
    output.Clear();
    // Read everything from the channel
    while(DoRead(output, timeout_ms)) {}

    // Did we read anything?
    return !output.IsEmpty();
}

void clSSHChannel::SendEOF()
{
    if(!IsOpen()) { throw clException("Channel is not opened"); }
    ssh_channel_send_eof(m_channel);
}
#endif
