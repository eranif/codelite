#if USE_SFTP
#ifndef CL_REMOTE_EXECUTOR_HPP
#define CL_REMOTE_EXECUTOR_HPP

#include "AsyncProcess/asyncprocess.h"
#include "clSSHChannel.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "procutils.h" // clShellProcessEvent
#include "ssh/ssh_account_info.h"

#include <vector>
#include <wx/msgqueue.h>

/// Manage a pool of workers to execute commands remotely for us
class WXDLLIMPEXP_SDK clRemoteExecutor : public wxEvtHandler
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

protected:
    void OnChannelStdout(clCommandEvent& event);
    void OnChannelStderr(clCommandEvent& event);
    void OnChannelClosed(clCommandEvent& event);
    void OnChannelError(clCommandEvent& event);

public:
    /// Create executor with default worker pool size
    clRemoteExecutor();
    virtual ~clRemoteExecutor();

    /// @param cmd the command details
    ///
    /// try to find a free channel and execute `cmd` on the remote host.
    ///
    /// the command is executed on the active remote host
    /// as defined by the `clRemoteHost` instance
    ///
    /// If succeeded, the output is returned in the form of this event:
    ///
    /// * wxEVT_ASYNC_PROCESS_OUTPUT
    /// * wxEVT_ASYNC_PROCESS_STDERR
    /// * wxEVT_ASYNC_PROCESS_TERMINATED (never contains output)
    ///
    /// Returns: the remote channel. The caller should delete it upon completion
    IProcess::Ptr_t try_execute(const clRemoteExecutor::Cmd& cmd);
};
#endif // CL_REMOTE_EXECUTOR_HPP
#endif
