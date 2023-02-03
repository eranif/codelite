#include "clSSHChannelCommon.hpp"

#include "StringUtils.h"
#include "cl_command_event.h"

#include <wx/regex.h>

#if USE_SFTP

#include "clModuleLogger.hpp"
INITIALISE_SSH_LOG(LOG, "Channel Reader Helper");

#include <libssh/libssh.h>

namespace
{
bool read_from_channel(SSHChannel_t channel, wxEvtHandler* handler, bool isStderr, bool wantStderr)
{
    char buffer[4097];
    int bytes = ssh_channel_read_timeout(channel, buffer, sizeof(buffer) - 1, isStderr ? 1 : 0, 1);
    if(bytes == SSH_ERROR) {
        // an error
        LOG_TRACE(LOG) << "channel read error" << endl;
        int exit_code = ssh_channel_get_exit_status(channel);
        clCommandEvent event(wxEVT_SSH_CHANNEL_READ_ERROR);
        event.SetInt(exit_code);
        handler->QueueEvent(event.Clone());
        return false;
    } else if(bytes == SSH_EOF) {
        // channel closed
        LOG_TRACE(LOG) << "channel read eof" << endl;
        int exit_code = ssh_channel_get_exit_status(channel);

        clCommandEvent event(wxEVT_SSH_CHANNEL_CLOSED);
        event.SetInt(exit_code);
        handler->QueueEvent(event.Clone());
        return false;
    } else if(bytes == 0) {
        // timeout
        if(ssh_channel_is_eof(channel)) {
            LOG_TRACE(LOG) << "channel eof detected" << endl;
            int exit_code = ssh_channel_get_exit_status(channel);

            // send close event
            clCommandEvent event(wxEVT_SSH_CHANNEL_CLOSED);
            event.SetInt(exit_code);
            handler->QueueEvent(event.Clone());
            return false;
        } else {
            return true;
        }
    } else {
        LOG_TRACE(LOG) << "read" << bytes << "bytes" << endl;
        buffer[bytes] = 0;
        clCommandEvent event((isStderr && wantStderr) ? wxEVT_SSH_CHANNEL_READ_STDERR : wxEVT_SSH_CHANNEL_READ_OUTPUT);
        event.SetStringRaw(buffer);
        handler->QueueEvent(event.Clone());
        return true;
    }
}

} // namespace

wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_WRITE_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_OUTPUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_STDERR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_CLOSED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_PTY, clCommandEvent);

bool clSSHCHannelRead(SSHChannel_t channel, wxEvtHandler* handler, bool isStderr, bool wantStderr)
{
    return read_from_channel(channel, handler, isStderr, wantStderr);
}
#endif
