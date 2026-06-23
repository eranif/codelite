#pragma once

#include "ai/ACPTypes.hpp"
#include "codelite_exports.h"

#include <optional>
#include <wx/event.h>
#include <wx/string.h>

// ============================================================
// clACPEvent — event class for all ACP protocol events
// ============================================================

class WXDLLIMPEXP_SDK clACPEvent : public wxEvent
{
public:
    clACPEvent(wxEventType type = wxEVT_NULL, int winid = 0);
    clACPEvent(const clACPEvent&) = default;
    clACPEvent& operator=(const clACPEvent&) = delete;
    ~clACPEvent() override = default;
    wxEvent* Clone() const override { return new clACPEvent(*this); }

    // -------------------------------------------------------
    // wxEVT_ACP_OUTPUT — streaming text chunk from the agent
    // -------------------------------------------------------

    /// Optional message id (chunks with the same id belong to one message).
    clACPEvent& SetMessageId(const std::optional<wxString>& id)
    {
        m_messageId = id;
        return *this;
    }
    const std::optional<wxString>& GetMessageId() const { return m_messageId; }

    /// The text chunk content.
    clACPEvent& SetText(const wxString& text)
    {
        m_text = text;
        return *this;
    }
    const wxString& GetText() const { return m_text; }

    // -------------------------------------------------------
    // wxEVT_ACP_PROMPT_DONE — turn finished
    // -------------------------------------------------------

    clACPEvent& SetStopReason(llm::acp::StopReason reason)
    {
        m_stopReason = reason;
        return *this;
    }
    llm::acp::StopReason GetStopReason() const { return m_stopReason; }

    // -------------------------------------------------------
    // wxEVT_ACP_TOOL_CALL — new tool call reported by agent
    // -------------------------------------------------------

    clACPEvent& SetToolCall(const llm::acp::UpdateToolCall& tc)
    {
        m_toolCall = tc;
        return *this;
    }
    const std::optional<llm::acp::UpdateToolCall>& GetToolCall() const { return m_toolCall; }

    // -------------------------------------------------------
    // wxEVT_ACP_TOOL_CALL_UPDATE — tool call status change
    // -------------------------------------------------------

    clACPEvent& SetToolCallUpdate(const llm::acp::UpdateToolCallUpdate& tcu)
    {
        m_toolCallUpdate = tcu;
        return *this;
    }
    const std::optional<llm::acp::UpdateToolCallUpdate>& GetToolCallUpdate() const { return m_toolCallUpdate; }

    // -------------------------------------------------------
    // wxEVT_ACP_USAGE — token / cost update
    // -------------------------------------------------------

    clACPEvent& SetUsage(const llm::acp::UpdateUsage& u)
    {
        m_usage = u;
        return *this;
    }
    const std::optional<llm::acp::UpdateUsage>& GetUsage() const { return m_usage; }

    // -------------------------------------------------------
    // wxEVT_ACP_PLAN — agent execution plan
    // -------------------------------------------------------

    clACPEvent& SetPlan(const llm::acp::UpdatePlan& p)
    {
        m_plan = p;
        return *this;
    }
    const std::optional<llm::acp::UpdatePlan>& GetPlan() const { return m_plan; }

    // -------------------------------------------------------
    // wxEVT_ACP_PERMISSION — agent requests tool permission
    //
    // Handler must call ACPClient::ResolvePermission(GetRpcId(), result)
    // before returning, including when the turn is being cancelled.
    // -------------------------------------------------------

    clACPEvent& SetRpcId(int id)
    {
        m_rpcId = id;
        return *this;
    }
    int GetRpcId() const { return m_rpcId; }

    clACPEvent& SetPermissionRequest(const llm::acp::RequestPermissionParams& p)
    {
        m_permissionRequest = p;
        return *this;
    }
    const std::optional<llm::acp::RequestPermissionParams>& GetPermissionRequest() const
    {
        return m_permissionRequest;
    }

    // -------------------------------------------------------
    // wxEVT_ACP_SESSION_LIST — session/list round-trip complete
    // -------------------------------------------------------

    clACPEvent& SetSessionListResult(const llm::acp::SessionListResult& r)
    {
        m_sessionListResult = r;
        return *this;
    }
    const std::optional<llm::acp::SessionListResult>& GetSessionListResult() const { return m_sessionListResult; }

    // -------------------------------------------------------
    // wxEVT_ACP_INITIALIZED — initialize round-trip complete
    // -------------------------------------------------------

    clACPEvent& SetInitializeResult(const llm::acp::InitializeResult& r)
    {
        m_initResult = r;
        return *this;
    }
    const std::optional<llm::acp::InitializeResult>& GetInitializeResult() const { return m_initResult; }

    // -------------------------------------------------------
    // wxEVT_ACP_SESSION_READY — session/new or session/load complete
    // -------------------------------------------------------

    clACPEvent& SetSessionId(const wxString& id)
    {
        m_sessionId = id;
        return *this;
    }
    const wxString& GetSessionId() const { return m_sessionId; }

    // -------------------------------------------------------
    // wxEVT_ACP_ERROR — transport or parse error
    // -------------------------------------------------------

    clACPEvent& SetErrorMessage(const wxString& msg)
    {
        m_errorMessage = msg;
        return *this;
    }
    const wxString& GetErrorMessage() const { return m_errorMessage; }

private:
    // wxEVT_ACP_OUTPUT / wxEVT_ACP_PLAN (user message replayed via session/load)
    std::optional<wxString> m_messageId;
    wxString m_text;

    // wxEVT_ACP_PROMPT_DONE
    llm::acp::StopReason m_stopReason{llm::acp::StopReason::end_turn};

    // wxEVT_ACP_TOOL_CALL
    std::optional<llm::acp::UpdateToolCall> m_toolCall;

    // wxEVT_ACP_TOOL_CALL_UPDATE
    std::optional<llm::acp::UpdateToolCallUpdate> m_toolCallUpdate;

    // wxEVT_ACP_USAGE
    std::optional<llm::acp::UpdateUsage> m_usage;

    // wxEVT_ACP_PLAN
    std::optional<llm::acp::UpdatePlan> m_plan;

    // wxEVT_ACP_SESSION_LIST
    std::optional<llm::acp::SessionListResult> m_sessionListResult;

    // wxEVT_ACP_INITIALIZED
    std::optional<llm::acp::InitializeResult> m_initResult;

    // wxEVT_ACP_SESSION_READY
    wxString m_sessionId;

    // wxEVT_ACP_PERMISSION
    int m_rpcId{-1};
    std::optional<llm::acp::RequestPermissionParams> m_permissionRequest;

    // wxEVT_ACP_ERROR
    wxString m_errorMessage;
};

using clACPEventFunction = void (wxEvtHandler::*)(clACPEvent&);
#define clACPEventHandler(func) wxEVENT_HANDLER_CAST(clACPEventFunction, func)

// ============================================================
// Event type declarations
// ============================================================

/// Streaming text chunk arriving from the agent.
/// Use GetMessageId() and GetText().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_OUTPUT, clACPEvent);

/// A prompt turn has finished.
/// Use GetStopReason().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_PROMPT_DONE, clACPEvent);

/// The agent reported a new tool call.
/// Use GetToolCall().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_TOOL_CALL, clACPEvent);

/// The agent updated the status of an existing tool call.
/// Use GetToolCallUpdate().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_TOOL_CALL_UPDATE, clACPEvent);

/// Token and cost usage update from the agent.
/// Use GetUsage().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_USAGE, clACPEvent);

/// The agent published an execution plan.
/// Use GetPlan().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_PLAN, clACPEvent);

/// The agent is requesting permission to invoke a tool.
/// Handler MUST call ACPClient::ResolvePermission(event.GetRpcId(), result).
/// Use GetRpcId() and GetPermissionRequest().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_PERMISSION, clACPEvent);

/// A session/list round-trip completed successfully.
/// Use GetSessionListResult() to get the page of sessions and the next cursor.
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_SESSION_LIST, clACPEvent);

/// The `initialize` round-trip completed successfully.
/// Use GetInitializeResult().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_INITIALIZED, clACPEvent);

/// A session/new or session/load round-trip completed successfully.
/// Use GetSessionId().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_SESSION_READY, clACPEvent);

/// A transport or JSON-RPC parse error occurred.
/// Use GetErrorMessage().
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_ERROR, clACPEvent);

/// The agent process terminated unexpectedly.
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_ACP_TERMINATED, clACPEvent);
