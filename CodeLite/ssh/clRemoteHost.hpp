#ifndef CLREMOTEHOST_HPP
#define CLREMOTEHOST_HPP

#if USE_SFTP
#include "asyncprocess.h"
#include "clWorkspaceEvent.hpp"
#include "cl_remote_executor.hpp"
#include "codelite_exports.h"
#include "processreaderthread.h"
#include "procutils.h"

#include <functional>
#include <vector>
#include <wx/event.h>

enum class clRemoteCommandStatus {
    STDOUT,
    STDERR,
    DONE,
    DONE_WITH_ERROR,
};

typedef std::function<void(const std::string&, clRemoteCommandStatus)> execute_callback;

class WXDLLIMPEXP_CL clRemoteHost : public wxEvtHandler
{
    clRemoteExecutor m_executor;
    std::vector<std::pair<execute_callback, clSSHChannel*>> m_callbacks;

private:
    clRemoteHost(const clRemoteHost&) = delete;
    clRemoteHost& operator=(const clRemoteHost&) = delete;
    clRemoteHost(clRemoteHost&&) = delete;
    clRemoteHost& operator=(clRemoteHost&&) = delete;

protected:
    void OnWorkspaceOpened(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnCommandCompleted(clProcessEvent& event);
    void OnCommandStdout(clProcessEvent& event);
    void OnCommandStderr(clProcessEvent& event);
    void DrainPendingCommands();

public:
    static clRemoteHost* Instance();
    static void Release();

    /// Execute a command with callback. we return the output as raw string (un-converted)
    void run_command_with_callback(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env,
                                   execute_callback&& cb);

    /// An overloaded version
    void run_command_with_callback(const wxString& command, const wxString& wd, const clEnvList_t& env,
                                   execute_callback&& cb);

private:
    clRemoteHost();
    virtual ~clRemoteHost();
};
#endif
#endif // CLREMOTEHOST_HPP
