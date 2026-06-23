#pragma once

#include "AsyncProcess/ChildProcess.h"
#include "ai/ACPEvent.hpp"
#include "ai/ACPTypes.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <atomic>
#include <functional>
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
 *   2. Bind to wxEVT_ACP_* events on the sink (or on *this).
 *   3. Call Start(argv) — spawns the agent process.
 *   4. Call Initialize() — fires wxEVT_ACP_INITIALIZED on success.
 *   5. Call NewSession() or LoadSession() — fires wxEVT_ACP_SESSION_READY.
 *   6. Call Prompt() one or more times.
 *   7. Call Stop() to terminate cleanly.
 *
 * All methods are non-blocking. Results arrive as wxEVT_ACP_* events
 * delivered on the main thread via AddPendingEvent.
 *
 * ACPClient subclasses wxEvtHandler so it can receive ChildProcess I/O
 * events and can serve as its own event sink when none is set.
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
    /// If not set (or nullptr) events are fired on *this.
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
    // Protocol methods (all async — results arrive as events)
    // -------------------------------------------------------

    /// Send `initialize`. Fires wxEVT_ACP_INITIALIZED on success,
    /// wxEVT_ACP_ERROR on failure.
    void Initialize(const InitializeParams& params = MakeDefaultClientInfo());

    /// Send `session/new`. Fires wxEVT_ACP_SESSION_READY on success,
    /// wxEVT_ACP_ERROR on failure.
    void NewSession(const SessionNewParams& params);

    /// Send `session/load`. Fires wxEVT_ACP_SESSION_READY on success,
    /// wxEVT_ACP_ERROR on failure.
    /// No-ops if the agent did not advertise loadSession capability.
    void LoadSession(const SessionLoadParams& params);

    /// Send `session/prompt` asynchronously.
    /// Fires wxEVT_ACP_OUTPUT for each streaming chunk,
    /// wxEVT_ACP_PROMPT_DONE on completion, wxEVT_ACP_ERROR on failure.
    /// Returns the JSON-RPC id assigned to the request.
    int Prompt(const SessionPromptParams& params);

    /// Send `session/list` to discover existing sessions.
    /// Fires wxEVT_ACP_SESSION_LIST on success, wxEVT_ACP_ERROR on failure.
    /// No-ops if the agent did not advertise sessionCapabilities.list.
    /// Pass a cursor from a prior result to fetch the next page.
    void ListSessions(const SessionListParams& params = {});

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
    // ChildProcess event handlers (main thread)
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
    // Pending RPC book-keeping (main-thread only — no mutex needed)
    // -------------------------------------------------------

    using ResponseCallback = std::function<void(const json&)>;
    void RegisterPending(int id, ResponseCallback cb);

    // -------------------------------------------------------
    // Members
    // -------------------------------------------------------

    std::atomic<int> m_nextId{1};
    ChildProcess* m_process{nullptr};
    wxEvtHandler* m_sink{nullptr};

    // Line-buffer for incoming stdio data (main thread only)
    std::string m_readBuffer;

    // Pending RPC callbacks (main thread only — no locking required)
    std::unordered_map<int, ResponseCallback> m_pending;

    // Cached result of Initialize
    std::optional<InitializeResult> m_initResult;
};

} // namespace llm::acp
