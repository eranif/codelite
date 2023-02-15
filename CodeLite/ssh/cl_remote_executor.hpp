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
    std::unordered_set<clSSHChannel*> m_runningCommands;
    friend class clSSHChannel;

    /// remove the channel from the tracking queue and delete it
    /// this method is called by the clSSHChannel after it completes its execution
    void Delete(clSSHChannel* channel);

public:
    /// Create executor with default worker pool size
    clRemoteExecutor();
    virtual ~clRemoteExecutor();

    /// cancel the current command being executed and return
    void shutdown();

    /// Returns: the remote channel. The caller should delete it upon completion
    // clSSHChannel* execute(const clRemoteExecutor::Cmd& cmd, const wxString& account);
    void execute_with_callback(const clRemoteExecutor::Cmd& cmd, const wxString& account, execute_callback&& cb);

    /// Execute a command and waits for its output
    bool execute_command(const clRemoteExecutor::Cmd& cmd, const wxString& account, std::string* output);
};
#endif // CL_REMOTE_EXECUTOR_HPP
#endif
