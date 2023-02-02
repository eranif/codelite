#ifndef CLREMOTEHOST_HPP
#define CLREMOTEHOST_HPP

#if USE_SFTP
#include "clWorkspaceEvent.hpp"
#include "cl_remote_executor.hpp"
#include "codelite_exports.h"
#include "procutils.h"

#include <functional>
#include <vector>
#include <wx/event.h>

typedef std::function<void(const std::string&)> execute_callback;

class WXDLLIMPEXP_CL clRemoteHost : public wxEvtHandler
{
    static clRemoteHost* ms_instance;
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
    void OnCommandCompleted(clShellProcessEvent& event);
    void DrainPendingCommands();

public:
    static clRemoteHost* Instance();
    static void Release();

private:
    clRemoteHost();
    virtual ~clRemoteHost();

    /// Execute a command with callback. we return the output as raw string (un-converted)
    void run_command_with_callback(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env,
                                   execute_callback&& cb);
};
#endif
#endif // CLREMOTEHOST_HPP
