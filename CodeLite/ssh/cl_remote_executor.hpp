#if USE_SFTP
#ifndef CL_REMOTE_EXECUTOR_HPP
#define CL_REMOTE_EXECUTOR_HPP

#include "asyncprocess.h"
#include "clSSHChannel.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "procutils.h" // clShellProcessEvent
#include "ssh/ssh_account_info.h"

#include <vector>
#include <wx/msgqueue.h>

/// Manage a pool of workers to execute commands remotely for us
class WXDLLIMPEXP_CL clRemoteExecutor : public wxEvtHandler
{
public:
    struct Cmd {
        std::vector<wxString> command;
        wxString wd;
        clEnvList_t env;

        static Cmd from(const std::vector<wxString>& cmd, const wxString& working_dir, const clEnvList_t& envlist = {})
        {
            Cmd c{ cmd, working_dir, envlist };
            return c;
        }
    };

private:
    wxString m_remoteAccount;

protected:
    void OnChannelStdout(clCommandEvent& event);
    void OnChannelStderr(clCommandEvent& event);
    void OnChannelClosed(clCommandEvent& event);
    void OnChannelError(clCommandEvent& event);

public:
    /// Create executor with default worker pool size
    clRemoteExecutor();
    virtual ~clRemoteExecutor();

    /// establish the channels to the remote host
    bool startup(const wxString& account_name);

    /// cancel the current command being executed and return
    void shutdown();

    /// try to find a free channel and execute our command
    /// If succeeded, the output is returned in the form of this event:
    ///
    /// - wxEVT_ASYNC_PROCESS_OUTPUT
    /// - wxEVT_ASYNC_PROCESS_STDERR
    /// - wxEVT_ASYNC_PROCESS_TERMINATED (never contains output)
    ///
    /// Returns: the remote channel. The caller should delete it upon completion
    clSSHChannel* try_execute(const clRemoteExecutor::Cmd& cmd);
};
#endif // CL_REMOTE_EXECUTOR_HPP
#endif
