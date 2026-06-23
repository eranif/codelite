#pragma once

#include "assistant/common/json.hpp"

#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <wx/string.h>

namespace llm::acp
{

using json = nlohmann::ordered_json;

// ============================================================
// Info / metadata
// ============================================================

struct AgentInfo {
    wxString name;
    wxString title;
    wxString version;
};

struct ClientInfo {
    wxString name;
    wxString title;
    wxString version;
};

// ============================================================
// Capabilities
// ============================================================

struct ClientFsCapabilities {
    bool readTextFile{false};
    bool writeTextFile{false};
};

struct ClientCapabilities {
    ClientFsCapabilities fs;
    bool terminal{false};
};

struct AgentMcpCapabilities {
    bool http{false};
    bool sse{false};
};

struct AgentPromptCapabilities {
    bool image{false};
    bool audio{false};
    bool embeddedContext{false};
};

struct AgentAuthCapabilities {
    bool logout{false};
};

struct AgentCapabilities {
    bool loadSession{false};
    AgentPromptCapabilities promptCapabilities;
    AgentMcpCapabilities mcpCapabilities;
    AgentAuthCapabilities auth;
};

// ============================================================
// Auth
// ============================================================

struct AuthMethod {
    wxString id;
    wxString name;
    std::optional<wxString> description;
    wxString type{"agent"};
};

// ============================================================
// ContentBlock types
// ============================================================

struct TextResource {
    wxString uri;
    wxString text;
    std::optional<wxString> mimeType;
};

struct BlobResource {
    wxString uri;
    std::string blob; // base64-encoded binary — kept as std::string
    std::optional<wxString> mimeType;
};

using EmbeddedResource = std::variant<TextResource, BlobResource>;

struct ContentBlockText {
    wxString text;
};

struct ContentBlockImage {
    std::string data; // base64-encoded binary — kept as std::string
    wxString mimeType;
    std::optional<wxString> uri;
};

struct ContentBlockAudio {
    std::string data; // base64-encoded binary — kept as std::string
    wxString mimeType;
};

struct ContentBlockResource {
    EmbeddedResource resource;
};

struct ContentBlockResourceLink {
    wxString uri;
    wxString name;
    std::optional<wxString> mimeType;
    std::optional<wxString> title;
    std::optional<wxString> description;
    std::optional<int64_t> size;
};

using ContentBlock = std::
    variant<ContentBlockText, ContentBlockImage, ContentBlockAudio, ContentBlockResource, ContentBlockResourceLink>;

// ============================================================
// MCP server configurations
// ============================================================

struct McpServerStdio {
    wxString name;
    wxString command;
    std::vector<wxString> args;
    std::vector<std::pair<wxString, wxString>> env;
};

struct McpServerHttp {
    wxString name;
    wxString url;
    std::vector<std::pair<wxString, wxString>> headers;
};

struct McpServerSse {
    wxString name;
    wxString url;
    std::vector<std::pair<wxString, wxString>> headers;
};

using McpServer = std::variant<McpServerStdio, McpServerHttp, McpServerSse>;

// ============================================================
// Initialisation
// ============================================================

struct InitializeParams {
    int protocolVersion{1};
    ClientCapabilities clientCapabilities;
    std::optional<ClientInfo> clientInfo;
};

struct InitializeResult {
    int protocolVersion{1};
    AgentCapabilities agentCapabilities;
    std::optional<AgentInfo> agentInfo;
    std::vector<AuthMethod> authMethods;
};

// ============================================================
// Session setup
// ============================================================

struct SessionNewParams {
    wxString cwd;
    std::vector<McpServer> mcpServers;
};

struct SessionNewResult {
    wxString sessionId;
};

struct SessionLoadParams {
    wxString sessionId;
    wxString cwd;
    std::vector<McpServer> mcpServers;
    std::vector<wxString> additionalDirectories;
};

// ============================================================
// session/update — update variants
// ============================================================

struct PlanEntry {
    wxString content;
    wxString priority; // "high" | "medium" | "low"
    wxString status;   // "pending" | ...
};

struct UpdatePlan {
    std::vector<PlanEntry> entries;
};

struct UpdateAgentMessageChunk {
    std::optional<wxString> messageId;
    ContentBlockText content;
};

struct UpdateUserMessageChunk {
    std::optional<wxString> messageId;
    ContentBlockText content;
};

/// ToolCallStatus values
enum class ToolCallStatus {
    pending,
    in_progress,
    completed,
    failed,
    cancelled,
};

// Internal helpers (std::string) — used only during JSON serialisation/parsing.
inline std::string ToolCallStatusToString(ToolCallStatus s)
{
    switch (s) {
    case ToolCallStatus::pending:
        return "pending";
    case ToolCallStatus::in_progress:
        return "in_progress";
    case ToolCallStatus::completed:
        return "completed";
    case ToolCallStatus::failed:
        return "failed";
    case ToolCallStatus::cancelled:
        return "cancelled";
    }
    return "pending";
}

inline ToolCallStatus ToolCallStatusFromString(const std::string& s)
{
    if (s == "in_progress")
        return ToolCallStatus::in_progress;
    if (s == "completed")
        return ToolCallStatus::completed;
    if (s == "failed")
        return ToolCallStatus::failed;
    if (s == "cancelled")
        return ToolCallStatus::cancelled;
    return ToolCallStatus::pending;
}

struct ToolCallLocation {
    wxString path;
    std::optional<int> line;
};

struct ToolCallContentContent {
    ContentBlockText content;
};

struct ToolCallContentDiff {
    wxString path;
    std::optional<wxString> oldText;
    wxString newText;
};

struct ToolCallContentTerminal {
    wxString terminalId;
};

using ToolCallContent = std::variant<ToolCallContentContent, ToolCallContentDiff, ToolCallContentTerminal>;

struct UpdateToolCall {
    wxString toolCallId;
    wxString title;
    std::optional<wxString> kind;
    ToolCallStatus status{ToolCallStatus::pending};
    std::vector<ToolCallContent> content;
    std::vector<ToolCallLocation> locations;
    std::optional<json> rawInput;
    std::optional<json> rawOutput;
};

struct UpdateToolCallUpdate {
    wxString toolCallId;
    std::optional<ToolCallStatus> status;
    std::vector<ToolCallContent> content;
};

struct UpdateUsage {
    int64_t used{0};
    int64_t size{0};
    std::optional<double> costAmount;
    std::optional<wxString> costCurrency;
};

using SessionUpdate = std::variant<UpdatePlan,
                                   UpdateAgentMessageChunk,
                                   UpdateUserMessageChunk,
                                   UpdateToolCall,
                                   UpdateToolCallUpdate,
                                   UpdateUsage>;

struct SessionUpdateParams {
    wxString sessionId;
    SessionUpdate update;
};

// ============================================================
// session/prompt
// ============================================================

enum class StopReason {
    end_turn,
    max_tokens,
    max_turn_requests,
    refusal,
    cancelled,
};

// Internal helpers (std::string) — used only during JSON serialisation/parsing.
inline std::string StopReasonToString(StopReason r)
{
    switch (r) {
    case StopReason::end_turn:
        return "end_turn";
    case StopReason::max_tokens:
        return "max_tokens";
    case StopReason::max_turn_requests:
        return "max_turn_requests";
    case StopReason::refusal:
        return "refusal";
    case StopReason::cancelled:
        return "cancelled";
    }
    return "end_turn";
}

inline StopReason StopReasonFromString(const std::string& s)
{
    if (s == "max_tokens")
        return StopReason::max_tokens;
    if (s == "max_turn_requests")
        return StopReason::max_turn_requests;
    if (s == "refusal")
        return StopReason::refusal;
    if (s == "cancelled")
        return StopReason::cancelled;
    return StopReason::end_turn;
}

struct SessionPromptParams {
    wxString sessionId;
    std::vector<ContentBlock> prompt;
};

struct SessionPromptResult {
    StopReason stopReason{StopReason::end_turn};
};

// ============================================================
// session/cancel notification (client → agent)
// ============================================================

struct SessionCancelParams {
    wxString sessionId;
};

// ============================================================
// session/request_permission (agent → client)
// ============================================================

struct PermissionOption {
    wxString optionId;
    wxString name;
    wxString kind; // "allow_once" | "allow_always" | "reject_once" | "reject_always"
};

struct RequestPermissionParams {
    wxString sessionId;
    wxString toolCallId;
    std::vector<PermissionOption> options;
};

struct PermissionOutcomeSelected {
    wxString optionId;
};

struct PermissionOutcomeCancelled {
};

using PermissionOutcome = std::variant<PermissionOutcomeSelected, PermissionOutcomeCancelled>;

struct RequestPermissionResult {
    PermissionOutcome outcome;
};

// ============================================================
// JSON-RPC 2.0 envelope helpers (internal — use std::string)
// ============================================================

inline json MakeRequest(int id, const std::string& method, json params)
{
    return json{{"jsonrpc", "2.0"}, {"id", id}, {"method", method}, {"params", std::move(params)}};
}

inline json MakeNotification(const std::string& method, json params)
{
    return json{{"jsonrpc", "2.0"}, {"method", method}, {"params", std::move(params)}};
}

inline json MakeResponse(int id, json result)
{
    return json{{"jsonrpc", "2.0"}, {"id", id}, {"result", std::move(result)}};
}

inline json MakeErrorResponse(int id, int code, const std::string& message)
{
    return json{{"jsonrpc", "2.0"}, {"id", id}, {"error", {{"code", code}, {"message", message}}}};
}

// ============================================================
// Serialisation / parsing (defined in ACPClient.cpp)
// ============================================================

json SerializeInitializeParams(const InitializeParams& p);
json SerializeSessionNewParams(const SessionNewParams& p);
json SerializeSessionLoadParams(const SessionLoadParams& p);
json SerializeSessionPromptParams(const SessionPromptParams& p);
json SerializeSessionCancelParams(const SessionCancelParams& p);
json SerializeRequestPermissionResult(int id, const RequestPermissionResult& r);

std::optional<InitializeResult> ParseInitializeResult(const json& j);
std::optional<SessionNewResult> ParseSessionNewResult(const json& j);
std::optional<SessionUpdateParams> ParseSessionUpdate(const json& j);
std::optional<SessionPromptResult> ParseSessionPromptResult(const json& j);
std::optional<RequestPermissionParams> ParseRequestPermissionParams(const json& j);

} // namespace llm::acp
