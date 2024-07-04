#ifndef CLREMOTEHOST_HPP
#define CLREMOTEHOST_HPP

#if USE_SFTP
#include "AsyncProcess/asyncprocess.h"
#include "AsyncProcess/processreaderthread.h"
#include "clWorkspaceEvent.hpp"
#include "cl_remote_executor.hpp"
#include "codelite_exports.h"
#include "procutils.h"
#include "ssh/ssh_account_info.h"

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

class WXDLLIMPEXP_SDK clRemoteHost : public wxEvtHandler
{
    clRemoteExecutor m_executor;
    std::vector<std::pair<execute_callback, IProcess::Ptr_t>> m_callbacks;
    std::vector<IProcess::Ptr_t> m_interactiveProcesses;
    wxString m_activeAccount;
    std::vector<clSSH::Ptr_t> m_sessions;

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

    /// create or get a new ssh session
    clSSH::Ptr_t TakeSession();

    /// create a new ssh session
    static clSSH::Ptr_t CreateSession(const wxString& account_name);

    /// put back the ssh_session into the queue
    void AddSshSession(clSSH::Ptr_t ssh_session);

    /// Execute a command with callback. we return the output as raw string (un-converted)
    void run_command_with_callback(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env,
                                   execute_callback&& cb);

    /// An overloaded version
    void run_command_with_callback(const wxString& command, const wxString& wd, const clEnvList_t& env,
                                   execute_callback&& cb);

    /// start an interactive remote process
    IProcess::Ptr_t run_interactive_process(wxEvtHandler* parent, const wxString& command, size_t flags,
                                            const wxString& wd, const clEnvList_t& env = {});

    /// An overloaded version
    IProcess::Ptr_t run_interactive_process(wxEvtHandler* parent, const wxArrayString& command, size_t flags,
                                            const wxString& wd, const clEnvList_t& env = {});

    const wxString& GetActiveAccount() const { return m_activeAccount; }

private:
    clRemoteHost();
    virtual ~clRemoteHost();
};
#endif
#endif // CLREMOTEHOST_HPP
