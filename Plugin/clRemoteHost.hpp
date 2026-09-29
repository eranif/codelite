#ifndef CLREMOTEHOST_HPP
#define CLREMOTEHOST_HPP

#if USE_SFTP
#include "AsyncProcess/asyncprocess.h"
#include "AsyncProcess/processreaderthread.h"
#include "clResult.hpp"
#include "clWorkspaceEvent.hpp"
#include "cl_remote_executor.hpp"
#include "codelite_exports.h"

#include <functional>
#include <vector>
#include <wx/event.h>

enum class clRemoteCommandStatus {
    STDOUT,
    STDERR,
    DONE,
    DONE_WITH_ERROR,
};

using execute_callback = std::function<void(const std::string&, clRemoteCommandStatus)>;

class WXDLLIMPEXP_SDK clRemoteHostEvent : public clCommandEvent
{
public:
    clRemoteHostEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : clCommandEvent(commandType, winid)
    {
    }
    clRemoteHostEvent(const clRemoteHostEvent&) = default;
    clRemoteHostEvent& operator=(const clRemoteHostEvent&) = delete;
    ~clRemoteHostEvent() override = default;
    wxEvent* Clone() const override { return new clRemoteHostEvent(*this); }

    void SetSession(clSSH::Ptr_t session) { m_session = session; }
    clSSH::Ptr_t GetSession() const { return m_session; }
    void SetRequestId(uint64_t id) { m_requestId = id; }
    uint64_t GetRequestId() const { return m_requestId; }

private:
    clSSH::Ptr_t m_session{nullptr};
    uint64_t m_requestId{0};
};

using clRemoteHostEventFunction = void (wxEvtHandler::*)(clRemoteHostEvent&);
#define clRemoteHostEventHandler(func) wxEVENT_HANDLER_CAST(clRemoteHostEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_REMOTEHOST_SESSION_CREATED, clRemoteHostEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_REMOTEHOST_SESSION_CREATE_ERROR, clRemoteHostEvent);

class WXDLLIMPEXP_SDK clRemoteHost : public wxEvtHandler
{
public:
    static clRemoteHost* Instance();
    static void Release();

    clRemoteHost(const clRemoteHost&) = delete;
    clRemoteHost& operator=(const clRemoteHost&) = delete;
    clRemoteHost(clRemoteHost&&) = delete;
    clRemoteHost& operator=(clRemoteHost&&) = delete;

    /// create or get a new ssh session
    clSSH::Ptr_t TakeSession();

    /// create a new ssh session
    static clSSH::Ptr_t CreateSession(const wxString& account_name);

    /// put back the ssh_session into the queue
    void AddSshSession(clSSH::Ptr_t ssh_session);

    /// Execute a command with callback. we return the output as raw string (un-converted)
    void run_command_with_callback(const std::vector<wxString>& command,
                                   const wxString& wd,
                                   const clEnvList_t& env,
                                   execute_callback&& cb);

    /// An overloaded version
    void run_command_with_callback(const wxString& command,
                                   const wxString& wd,
                                   const clEnvList_t& env,
                                   execute_callback&& cb);

    /// Create an interactive process using a new ssh session. This call blocks while the session is created.
    clStatusOr<IProcess::Ptr_t> CreateInteractiveProcess(wxEvtHandler* parent,
                                                         const wxArrayString& command,
                                                         size_t flags,
                                                         const wxString& wd,
                                                         const clEnvList_t& env = {});

    /// Create an interactive process using an existing ssh session (e.g. one created by AsyncCreateSession).
    /// This must be called from the main thread.
    clStatusOr<IProcess::Ptr_t> CreateInteractiveProcess(wxEvtHandler* parent,
                                                         clSSH::Ptr_t ssh_session,
                                                         const wxArrayString& command,
                                                         size_t flags,
                                                         const wxString& wd,
                                                         const clEnvList_t& env = {});

    /// Create a new ssh session in a background thread, so the UI does not block while connecting.
    /// The result is sent to the EventNotifier as wxEVT_REMOTEHOST_SESSION_CREATED (the session is
    /// attached to the event) or wxEVT_REMOTEHOST_SESSION_CREATE_ERROR.
    /// The return value is a unique identifier that can be used to match the request with the caller.
    /// The background thread does not use any pointer owned by the caller.
    clStatusOr<uint64_t> AsyncCreateSession();

    const wxString& GetActiveAccount() const { return m_activeAccount; }

private:
    clRemoteHost();
    ~clRemoteHost() override;

    clRemoteExecutor m_executor;
    std::vector<std::pair<execute_callback, IProcess::Ptr_t>> m_callbacks;
    std::vector<IProcess::Ptr_t> m_interactiveProcesses;
    wxString m_activeAccount;
    std::vector<clSSH::Ptr_t> m_sessions;

    void OnWorkspaceOpened(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnCommandCompleted(clProcessEvent& event);
    void OnCommandStdout(clProcessEvent& event);
    void OnCommandStderr(clProcessEvent& event);
    void DrainPendingCommands();
};
#endif
#endif // CLREMOTEHOST_HPP
