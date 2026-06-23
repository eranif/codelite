#pragma once

#include "AsyncProcess/ChildProcess.h"
#include "ai/ACPEvent.hpp"
#include "ai/ACPTypes.hpp"
#include "assistant/attributes.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <wx/event.h>
#include <wx/string.h>

namespace llm::acp
{

/**
 * @brief Stdio-transport ACP client.
 *
 * Lifecycle:
 *   1. Construct and set an event sink: SetEventSink(handler).
 *   2. Call Start(argv) — spawns the agent process.
 *   3. Call Initialize() — version / capability negotiation.
 *   4. Call NewSession() or LoadSession().
 *   5. Call Prompt() one or more times.
 *   6. Call Stop() to terminate cleanly.
 *
 * ACPClient subclasses wxEvtHandler so it can receive ChildProcess I/O
 * events.  Protocol events (wxEVT_ACP_*) are fired on the sink set via
 * SetEventSink().  If no sink is set the events are fired on *this so the
 * owner can also Bind() directly to the ACPClient instance.
 *
 * Blocking calls (Initialize, NewSession, LoadSession) must NOT be called
 * from within an ACP event handler — doing so will deadlock.
 */
class WXDLLIMPEXP_SDK ACPClient : public wxEvtHandler
{
public:
    ACPClient();
    ~ACPClient() override;

    // Non-copyable, non-movable.
    ACPClient(const ACPClient&) = delete;
    ACPClient& operator=(const ACPClient&) = delete;
    ACPClient(ACPClient&&) = delete;
    ACPClient& operator=(ACPClient&&) = delete;

    // -------------------------------------------------------
    // Event sink
    // -------------------------------------------------------

    /// Set the wxEvtHandler that will receive wxEVT_ACP_* events.
    /// If not set (or set to nullptr) events are fired on *this.
    void SetEventSink(wxEvtHandler* sink) { m_sink = sink; }

    // -------------------------------------------------------
    // Lifecycle
    // -------------------------------------------------------

    /// Spawn the agent process (argv[0] = executable).
    /// Calls Stop() first if already running.
    void Start(const wxArrayString& argv);

    /// Terminate the agent process and fail all pending requests.
    void Stop();

    /// Returns true if the child process is alive.
    bool IsRunning() const;

    // -------------------------------------------------------
    // Protocol methods
    // -------------------------------------------------------

    /// Send `initialize` and wait for the response (blocking, up to 30 s).
    /// Returns std::nullopt on error or timeout.
    std::optional<InitializeResult> Initialize(const InitializeParams& params = MakeDefaultClientInfo());

    /// Send `session/new` and wait for the response (blocking, up to 30 s).
    std::optional<SessionNewResult> NewSession(const SessionNewParams& params);

    /// Send `session/load` and wait for the response (blocking, up to 60 s).
    /// Returns std::nullopt on error or if the agent does not support loadSession.
    std::optional<json> LoadSession(const SessionLoadParams& params);

    /// Send `session/prompt` asynchronously.
    /// Fires wxEVT_ACP_OUTPUT for each streaming chunk, wxEVT_ACP_PROMPT_DONE on completion.
    /// Returns the JSON-RPC id assigned to the request.
    int Prompt(const SessionPromptParams& params);

    /// Send `session/cancel` notification (no response expected).
    void Cancel(const wxString& session_id);

    /// Resolve a pending wxEVT_ACP_PERMISSION request.
    /// Must be called for every permission event, even when cancelling
    /// (pass PermissionOutcomeCancelled in that case).
    void ResolvePermission(int rpc_id, const RequestPermissionResult& result);

    // -------------------------------------------------------
    // Accessors
    // -------------------------------------------------------

    const std::optional<InitializeResult>& GetInitializeResult() const { return m_initResult; }

private:
    static InitializeParams MakeDefaultClientInfo();

    // -------------------------------------------------------
    // ChildProcess event handlers
    // -------------------------------------------------------

    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);

    // -------------------------------------------------------
    // Internal helpers
    // -------------------------------------------------------

    int NextId();
    void Send(const json& msg);
    void DispatchLine(const std::string& line);
    void DispatchMessage(const json& msg);
    void DispatchResponse(int id, const json& result);
    void DispatchNotification(const std::string& method, const json& params);
    void FireEvent(clACPEvent& event);
    void FailAllPending();

    // -------------------------------------------------------
    // Pending RPC book-keeping
    // -------------------------------------------------------

    using ResponseCallback = std::function<void(const json&)>;
    void RegisterPending(int id, ResponseCallback cb);

    // -------------------------------------------------------
    // Members
    // -------------------------------------------------------

    std::atomic<int> m_nextId{1};
    ChildProcess* m_process{nullptr};
    wxEvtHandler* m_sink{nullptr};

    // Line-buffer for incoming stdio data (accessed only on main thread)
    std::string m_readBuffer;

    // Pending synchronous RPC requests
    mutable std::mutex m_pendingMutex;
    std::unordered_map<int, ResponseCallback> m_pending GUARDED_BY(m_pendingMutex);

    // Cached result of Initialize
    std::optional<InitializeResult> m_initResult;
};

} // namespace llm::acp
