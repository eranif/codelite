#include "clSSHChannelCommon.hpp"

#include "StringUtils.h"
#include "cl_command_event.h"

#include <wx/regex.h>

#if USE_SFTP

#include "clModuleLogger.hpp"
INITIALISE_SSH_LOG(LOG, "Channel Reader Helper");

#include "clTempFile.hpp"
#include "cl_sftp.h"

#include <libssh/libssh.h>

wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_WRITE_ERROR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_OUTPUT, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_STDERR, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_CLOSED, clCommandEvent);

namespace ssh
{
thread_local char buffer[4094 + 1];
read_result channel_read(SSHChannel_t channel, wxEvtHandler* handler, bool isStderr, bool wantStderr)
{
    int bytes = ssh_channel_read_timeout(channel, buffer, sizeof(buffer) - 1, isStderr ? 1 : 0, 1);
    if(bytes == SSH_ERROR) {
        // an error
        LOG_DEBUG(LOG) << "channel read error" << endl;
        int exit_code = ssh_channel_get_exit_status(channel);
        clCommandEvent event(wxEVT_SSH_CHANNEL_READ_ERROR);
        event.SetInt(exit_code);
        handler->QueueEvent(event.Clone());
        return read_result::SSH_IO_ERROR;

    } else if(bytes == SSH_EOF) {
        // channel closed
        LOG_DEBUG(LOG) << "channel read eof" << endl;
        int exit_code = ssh_channel_get_exit_status(channel);

        clCommandEvent event(wxEVT_SSH_CHANNEL_CLOSED);
        event.SetInt(exit_code);
        handler->QueueEvent(event.Clone());
        return read_result::SSH_CONN_CLOSED;

    } else if(bytes == 0) {
        // timeout
        if(ssh_channel_is_eof(channel)) {
            LOG_DEBUG(LOG) << "channel eof detected" << endl;
            int exit_code = ssh_channel_get_exit_status(channel);

            // send close event
            clCommandEvent event(wxEVT_SSH_CHANNEL_CLOSED);
            event.SetInt(exit_code);
            handler->QueueEvent(event.Clone());
            return read_result::SSH_CONN_CLOSED;

        } else {
            // timeout
            return read_result::SSH_TIMEOUT;
        }
    } else {
        LOG_DEBUG(LOG) << "read" << bytes << "bytes" << endl;
        buffer[bytes] = 0;
        clCommandEvent event((isStderr && wantStderr) ? wxEVT_SSH_CHANNEL_READ_STDERR : wxEVT_SSH_CHANNEL_READ_OUTPUT);
        event.SetStringRaw(buffer);
        handler->QueueEvent(event.Clone());
        return read_result::SSH_SUCCESS;
    }
}

wxString build_command(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env)
{
    wxString cmd;
    if(!env.empty()) {
        // build each env in its own "export" statement
        for(const auto& e : env) {
            cmd << "export " << e.first << "=" << e.second << ";";
        }
    }

    if(!wd.empty()) {
        cmd << "cd " << StringUtils::WrapWithDoubleQuotes(wd) << " && ";
    }

    for(const wxString& c : command) {
        cmd << StringUtils::WrapWithDoubleQuotes(c) << " ";
    }

    if(cmd.EndsWith(" ")) {
        cmd.RemoveLast();
    }
    return cmd;
}

wxString build_script_content(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env)
{
    wxString content;
    content << "#!/bin/bash\n\n";
    if(!env.empty()) {
        // build each env in its own "export" statement
        for(const auto& e : env) {
            content << "export " << e.first << "=" << e.second << "\n";
        }
    }

    if(!wd.empty()) {
        content << "cd " << StringUtils::WrapWithDoubleQuotes(wd) << "\n";
    }

    for(const wxString& c : command) {
        content << StringUtils::WrapWithDoubleQuotes(c) << " ";
    }

    if(content.EndsWith(" ")) {
        content.RemoveLast();
    }
    content << "\n";
    return content;
}

clResultBool write_remote_file_content(clSSH::Ptr_t ssh, const wxString& remote_path, const wxString& content)
{
    clTempFile tmpfile;
    if(!tmpfile.Write(content, wxConvUTF8)) {
        return clResultBool::make_error("failed to write file");
    }

    try {
        clSFTP::Ptr_t sftp(new clSFTP(ssh));
        sftp->Initialize();

        // write the file content
        sftp->Write(tmpfile.GetFullPath(), remote_path);
        // set execute permissions to the file
        sftp->Chmod(remote_path, 0755);
    } catch(clException& e) {
        wxString errmsg;
        errmsg << ssh_get_error(ssh->GetSession()) << ". " << e.What();
        return clResultBool::make_error(errmsg);
    }
    return clResultBool::make_success(true);
}

} // namespace ssh
#endif
