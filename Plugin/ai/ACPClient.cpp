#include "ai/ACPClient.hpp"

#include "AsyncProcess/processreaderthread.h"

namespace llm::acp
{

// ============================================================
// Conversion helpers
// ============================================================

namespace
{

/// Extract a UTF-8 string field from JSON and return it as wxString.
inline wxString JStr(const json& j, const char* key)
{
    if (j.contains(key) && j[key].is_string()) {
        return wxString::FromUTF8(j[key].get<std::string>());
    }
    return wxString{};
}

/// Same as JStr but returns a fallback when the field is missing.
inline wxString JStrOr(const json& j, const char* key, const wxString& fallback)
{
    if (j.contains(key) && j[key].is_string()) {
        return wxString::FromUTF8(j[key].get<std::string>());
    }
    return fallback;
}

/// Convert wxString to a UTF-8 std::string for JSON serialisation.
inline std::string WX(const wxString& s) { return s.ToStdString(wxConvUTF8); }

} // anonymous namespace

// ============================================================
// JSON serialisation helpers (internal)
// ============================================================

namespace
{

json SerializeContentBlock(const ContentBlock& cb)
{
    return std::visit(
        [](const auto& v) -> json {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, ContentBlockText>) {
                return json{{"type", "text"}, {"text", WX(v.text)}};
            } else if constexpr (std::is_same_v<T, ContentBlockImage>) {
                json j{{"type", "image"}, {"data", v.data}, {"mimeType", WX(v.mimeType)}};
                if (v.uri)
                    j["uri"] = WX(*v.uri);
                return j;
            } else if constexpr (std::is_same_v<T, ContentBlockAudio>) {
                return json{{"type", "audio"}, {"data", v.data}, {"mimeType", WX(v.mimeType)}};
            } else if constexpr (std::is_same_v<T, ContentBlockResource>) {
                json res = std::visit(
                    [](const auto& r) -> json {
                        using R = std::decay_t<decltype(r)>;
                        json j{{"uri", WX(r.uri)}};
                        if constexpr (std::is_same_v<R, TextResource>) {
                            j["text"] = WX(r.text);
                        } else {
                            j["blob"] = r.blob;
                        }
                        if (r.mimeType)
                            j["mimeType"] = WX(*r.mimeType);
                        return j;
                    },
                    v.resource);
                return json{{"type", "resource"}, {"resource", std::move(res)}};
            } else if constexpr (std::is_same_v<T, ContentBlockResourceLink>) {
                json j{{"type", "resource_link"}, {"uri", WX(v.uri)}, {"name", WX(v.name)}};
                if (v.mimeType)
                    j["mimeType"] = WX(*v.mimeType);
                if (v.title)
                    j["title"] = WX(*v.title);
                if (v.description)
                    j["description"] = WX(*v.description);
                if (v.size)
                    j["size"] = *v.size;
                return j;
            }
        },
        cb);
}

json SerializeMcpServer(const McpServer& server)
{
    return std::visit(
        [](const auto& s) -> json {
            using T = std::decay_t<decltype(s)>;
            if constexpr (std::is_same_v<T, McpServerStdio>) {
                json args = json::array();
                for (const auto& a : s.args) {
                    args.push_back(WX(a));
                }
                json env_arr = json::array();
                for (const auto& [k, v] : s.env) {
                    env_arr.push_back({{"name", WX(k)}, {"value", WX(v)}});
                }
                return json{{"name", WX(s.name)}, {"command", WX(s.command)}, {"args", args}, {"env", env_arr}};
            } else if constexpr (std::is_same_v<T, McpServerHttp>) {
                json hdrs = json::array();
                for (const auto& [k, v] : s.headers) {
                    hdrs.push_back({{"name", WX(k)}, {"value", WX(v)}});
                }
                return json{{"type", "http"}, {"name", WX(s.name)}, {"url", WX(s.url)}, {"headers", hdrs}};
            } else {
                json hdrs = json::array();
                for (const auto& [k, v] : s.headers) {
                    hdrs.push_back({{"name", WX(k)}, {"value", WX(v)}});
                }
                return json{{"type", "sse"}, {"name", WX(s.name)}, {"url", WX(s.url)}, {"headers", hdrs}};
            }
        },
        server);
}

} // anonymous namespace

// ============================================================
// ACPTypes serialisation — definitions for header declarations
// ============================================================

json SerializeInitializeParams(const InitializeParams& p)
{
    json caps;
    caps["fs"]["readTextFile"] = p.clientCapabilities.fs.readTextFile;
    caps["fs"]["writeTextFile"] = p.clientCapabilities.fs.writeTextFile;
    if (p.clientCapabilities.terminal) {
        caps["terminal"] = true;
    }

    json params{{"protocolVersion", p.protocolVersion}, {"clientCapabilities", caps}};
    if (p.clientInfo) {
        params["clientInfo"] = {
            {"name", WX(p.clientInfo->name)},
            {"title", WX(p.clientInfo->title)},
            {"version", WX(p.clientInfo->version)},
        };
    }
    return params;
}

json SerializeSessionNewParams(const SessionNewParams& p)
{
    json servers = json::array();
    for (const auto& s : p.mcpServers) {
        servers.push_back(SerializeMcpServer(s));
    }
    return json{{"cwd", WX(p.cwd)}, {"mcpServers", servers}};
}

json SerializeSessionLoadParams(const SessionLoadParams& p)
{
    json servers = json::array();
    for (const auto& s : p.mcpServers) {
        servers.push_back(SerializeMcpServer(s));
    }
    json params{{"sessionId", WX(p.sessionId)}, {"cwd", WX(p.cwd)}, {"mcpServers", servers}};
    if (!p.additionalDirectories.empty()) {
        json dirs = json::array();
        for (const auto& d : p.additionalDirectories) {
            dirs.push_back(WX(d));
        }
        params["additionalDirectories"] = dirs;
    }
    return params;
}

json SerializeSessionPromptParams(const SessionPromptParams& p)
{
    json blocks = json::array();
    for (const auto& cb : p.prompt) {
        blocks.push_back(SerializeContentBlock(cb));
    }
    return json{{"sessionId", WX(p.sessionId)}, {"prompt", blocks}};
}

json SerializeSessionCancelParams(const SessionCancelParams& p) { return json{{"sessionId", WX(p.sessionId)}}; }

json SerializeSessionListParams(const SessionListParams& p)
{
    json params = json::object();
    if (p.cwd)
        params["cwd"] = WX(*p.cwd);
    if (p.cursor)
        params["cursor"] = WX(*p.cursor);
    return params;
}

json SerializeRequestPermissionResult(int id, const RequestPermissionResult& r)
{
    json outcome = std::visit(
        [](const auto& o) -> json {
            using T = std::decay_t<decltype(o)>;
            if constexpr (std::is_same_v<T, PermissionOutcomeSelected>) {
                return json{{"outcome", "selected"}, {"optionId", WX(o.optionId)}};
            } else {
                return json{{"outcome", "cancelled"}};
            }
        },
        r.outcome);
    return MakeResponse(id, {{"outcome", std::move(outcome)}});
}

// ============================================================
// ACPTypes parse helpers — definitions for header declarations
// ============================================================

std::optional<InitializeResult> ParseInitializeResult(const json& j)
{
    if (!j.contains("protocolVersion"))
        return std::nullopt;

    InitializeResult r;
    r.protocolVersion = j.value("protocolVersion", 1);

    if (j.contains("agentCapabilities")) {
        const auto& ac = j["agentCapabilities"];
        r.agentCapabilities.loadSession = ac.value("loadSession", false);
        if (ac.contains("promptCapabilities")) {
            const auto& pc = ac["promptCapabilities"];
            r.agentCapabilities.promptCapabilities.image = pc.value("image", false);
            r.agentCapabilities.promptCapabilities.audio = pc.value("audio", false);
            r.agentCapabilities.promptCapabilities.embeddedContext = pc.value("embeddedContext", false);
        }
        if (ac.contains("mcpCapabilities")) {
            const auto& mc = ac["mcpCapabilities"];
            r.agentCapabilities.mcpCapabilities.http = mc.value("http", false);
            r.agentCapabilities.mcpCapabilities.sse = mc.value("sse", false);
        }
        if (ac.contains("auth") && ac["auth"].contains("logout")) {
            r.agentCapabilities.auth.logout = true;
        }
        if (ac.contains("sessionCapabilities")) {
            const auto& sc = ac["sessionCapabilities"];
            r.agentCapabilities.sessionCapabilities.list = sc.value("list", false);
        }
    }

    if (j.contains("agentInfo")) {
        const auto& ai = j["agentInfo"];
        r.agentInfo = AgentInfo{
            .name = JStr(ai, "name"),
            .title = JStr(ai, "title"),
            .version = JStr(ai, "version"),
        };
    }

    if (j.contains("authMethods") && j["authMethods"].is_array()) {
        for (const auto& am : j["authMethods"]) {
            AuthMethod m;
            m.id = JStr(am, "id");
            m.name = JStr(am, "name");
            m.type = JStrOr(am, "type", "agent");
            if (am.contains("description") && am["description"].is_string()) {
                m.description = JStr(am, "description");
            }
            r.authMethods.push_back(std::move(m));
        }
    }

    return r;
}

std::optional<SessionNewResult> ParseSessionNewResult(const json& j)
{
    if (!j.contains("sessionId"))
        return std::nullopt;
    return SessionNewResult{.sessionId = JStr(j, "sessionId")};
}

std::optional<SessionListResult> ParseSessionListResult(const json& j)
{
    if (!j.contains("sessions") || !j["sessions"].is_array())
        return std::nullopt;

    SessionListResult r;
    for (const auto& s : j["sessions"]) {
        SessionInfo info;
        info.sessionId = JStr(s, "sessionId");
        info.cwd = JStr(s, "cwd");
        if (s.contains("title") && s["title"].is_string())
            info.title = JStr(s, "title");
        if (s.contains("updatedAt") && s["updatedAt"].is_string())
            info.updatedAt = JStr(s, "updatedAt");
        if (s.contains("additionalDirectories") && s["additionalDirectories"].is_array()) {
            for (const auto& d : s["additionalDirectories"]) {
                if (d.is_string())
                    info.additionalDirectories.push_back(wxString::FromUTF8(d.get<std::string>()));
            }
        }
        r.sessions.push_back(std::move(info));
    }
    if (j.contains("nextCursor") && j["nextCursor"].is_string())
        r.nextCursor = JStr(j, "nextCursor");

    return r;
}

namespace
{

std::optional<SessionUpdate> ParseSessionUpdateBody(const json& update)
{
    if (!update.contains("sessionUpdate"))
        return std::nullopt;

    const std::string kind = update["sessionUpdate"];

    if (kind == "plan") {
        UpdatePlan p;
        if (update.contains("entries") && update["entries"].is_array()) {
            for (const auto& e : update["entries"]) {
                p.entries.push_back(PlanEntry{
                    .content = JStr(e, "content"),
                    .priority = JStrOr(e, "priority", "medium"),
                    .status = JStrOr(e, "status", "pending"),
                });
            }
        }
        return p;
    }

    if (kind == "agent_message_chunk") {
        UpdateAgentMessageChunk chunk;
        if (update.contains("messageId") && update["messageId"].is_string()) {
            chunk.messageId = JStr(update, "messageId");
        }
        if (update.contains("content")) {
            chunk.content.text = JStr(update["content"], "text");
        }
        return chunk;
    }

    if (kind == "user_message_chunk") {
        UpdateUserMessageChunk chunk;
        if (update.contains("messageId") && update["messageId"].is_string()) {
            chunk.messageId = JStr(update, "messageId");
        }
        if (update.contains("content")) {
            chunk.content.text = JStr(update["content"], "text");
        }
        return chunk;
    }

    if (kind == "tool_call") {
        UpdateToolCall tc;
        tc.toolCallId = JStr(update, "toolCallId");
        tc.title = JStr(update, "title");
        if (update.contains("kind") && update["kind"].is_string()) {
            tc.kind = JStr(update, "kind");
        }
        if (update.contains("status") && update["status"].is_string()) {
            tc.status = ToolCallStatusFromString(update["status"]);
        }
        if (update.contains("rawInput")) {
            tc.rawInput = update["rawInput"];
        }
        if (update.contains("rawOutput")) {
            tc.rawOutput = update["rawOutput"];
        }
        if (update.contains("locations") && update["locations"].is_array()) {
            for (const auto& loc : update["locations"]) {
                ToolCallLocation l;
                l.path = JStr(loc, "path");
                if (loc.contains("line") && loc["line"].is_number()) {
                    l.line = loc["line"].get<int>();
                }
                tc.locations.push_back(std::move(l));
            }
        }
        return tc;
    }

    if (kind == "tool_call_update") {
        UpdateToolCallUpdate tcu;
        tcu.toolCallId = JStr(update, "toolCallId");
        if (update.contains("status") && update["status"].is_string()) {
            tcu.status = ToolCallStatusFromString(update["status"]);
        }
        return tcu;
    }

    if (kind == "usage_update") {
        UpdateUsage u;
        u.used = update.value("used", int64_t{0});
        u.size = update.value("size", int64_t{0});
        if (update.contains("cost") && update["cost"].is_object()) {
            const auto& cost = update["cost"];
            if (cost.contains("amount") && cost["amount"].is_number()) {
                u.costAmount = cost["amount"].get<double>();
            }
            if (cost.contains("currency") && cost["currency"].is_string()) {
                u.costCurrency = JStr(cost, "currency");
            }
        }
        return u;
    }

    return std::nullopt;
}

} // anonymous namespace

std::optional<SessionUpdateParams> ParseSessionUpdate(const json& j)
{
    if (!j.contains("sessionId") || !j.contains("update"))
        return std::nullopt;

    auto update = ParseSessionUpdateBody(j["update"]);
    if (!update)
        return std::nullopt;

    return SessionUpdateParams{.sessionId = JStr(j, "sessionId"), .update = std::move(*update)};
}

std::optional<SessionPromptResult> ParseSessionPromptResult(const json& j)
{
    if (!j.contains("stopReason"))
        return std::nullopt;
    return SessionPromptResult{.stopReason = StopReasonFromString(j["stopReason"])};
}

std::optional<RequestPermissionParams> ParseRequestPermissionParams(const json& j)
{
    if (!j.contains("sessionId") || !j.contains("toolCall"))
        return std::nullopt;

    RequestPermissionParams p;
    p.sessionId = JStr(j, "sessionId");
    p.toolCallId = JStr(j["toolCall"], "toolCallId");

    if (j.contains("options") && j["options"].is_array()) {
        for (const auto& opt : j["options"]) {
            p.options.push_back(PermissionOption{
                .optionId = JStr(opt, "optionId"),
                .name = JStr(opt, "name"),
                .kind = JStr(opt, "kind"),
            });
        }
    }
    return p;
}

// ============================================================
// ACPClient implementation
// ============================================================

ACPClient::ACPClient() = default;

ACPClient::~ACPClient() { Stop(); }

// -------------------------------------------------------
// Event dispatch helper
// -------------------------------------------------------

void ACPClient::FireEvent(clACPEvent& event)
{
    wxEvtHandler* sink = m_sink ? m_sink : this;
    sink->AddPendingEvent(event);
}

// -------------------------------------------------------
// Lifecycle
// -------------------------------------------------------

void ACPClient::Start(const wxArrayString& argv)
{
    Stop();

    m_process = new ChildProcess();
    m_process->Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ACPClient::OnProcessOutput, this);
    m_process->Bind(wxEVT_ASYNC_PROCESS_STDERR, &ACPClient::OnProcessStderr, this);
    m_process->Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ACPClient::OnProcessTerminated, this);
    m_process->Start(argv);
}

void ACPClient::Stop()
{
    if (m_process) {
        m_process->Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &ACPClient::OnProcessOutput, this);
        m_process->Unbind(wxEVT_ASYNC_PROCESS_STDERR, &ACPClient::OnProcessStderr, this);
        m_process->Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &ACPClient::OnProcessTerminated, this);
        wxDELETE(m_process);
    }
    m_readBuffer.clear();
    FailAllPending();
}

bool ACPClient::IsRunning() const { return m_process && m_process->IsOk(); }

// -------------------------------------------------------
// ChildProcess event handlers (main thread)
// -------------------------------------------------------

void ACPClient::OnProcessOutput(clProcessEvent& event)
{
    const std::string& data = event.GetOutputRaw();
    if (data.empty())
        return;

    m_readBuffer += data;

    size_t pos;
    while ((pos = m_readBuffer.find('\n')) != std::string::npos) {
        std::string line = m_readBuffer.substr(0, pos);
        m_readBuffer.erase(0, pos + 1);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            DispatchLine(line);
        }
    }
}

void ACPClient::OnProcessStderr(clProcessEvent& event)
{
    // stderr carries agent log output; nothing to forward as an ACP event
    wxUnusedVar(event);
}

void ACPClient::OnProcessTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    wxDELETE(m_process);
    m_readBuffer.clear();
    FailAllPending();

    clACPEvent e(wxEVT_ACP_TERMINATED);
    FireEvent(e);
}

// -------------------------------------------------------
// Send
// -------------------------------------------------------

int ACPClient::NextId() { return m_nextId.fetch_add(1, std::memory_order_relaxed); }

void ACPClient::Send(const json& msg)
{
    if (!IsRunning())
        return;
    m_process->Write(msg.dump() + "\n");
}

// -------------------------------------------------------
// Pending RPC book-keeping
// -------------------------------------------------------

void ACPClient::RegisterPending(int id, ResponseCallback cb)
{
    m_pending.emplace(id, std::move(cb));
}

void ACPClient::FailAllPending()
{
    // Swap out first so callbacks cannot re-enter and modify m_pending.
    std::unordered_map<int, ResponseCallback> pending;
    pending.swap(m_pending);
    for (auto& [id, cb] : pending) {
        cb(json{{"error", {{"code", -32000}, {"message", "ACPClient stopped"}}}});
    }
}

// -------------------------------------------------------
// Protocol methods
// -------------------------------------------------------

void ACPClient::Initialize(const InitializeParams& params)
{
    const int id = NextId();
    RegisterPending(id, [this](const json& response) {
        if (response.contains("error")) {
            clACPEvent e(wxEVT_ACP_ERROR);
            e.SetErrorMessage(wxString::FromUTF8(response["error"].value("message", "initialize failed")));
            FireEvent(e);
            return;
        }
        auto parsed = ParseInitializeResult(response);
        if (!parsed) {
            clACPEvent e(wxEVT_ACP_ERROR);
            e.SetErrorMessage("initialize: failed to parse agent response");
            FireEvent(e);
            return;
        }
        m_initResult = parsed;
        clACPEvent e(wxEVT_ACP_INITIALIZED);
        e.SetInitializeResult(*parsed);
        FireEvent(e);
    });
    Send(MakeRequest(id, "initialize", SerializeInitializeParams(params)));
}

void ACPClient::NewSession(const SessionNewParams& params)
{
    const int id = NextId();
    RegisterPending(id, [this](const json& response) {
        if (response.contains("error")) {
            clACPEvent e(wxEVT_ACP_ERROR);
            e.SetErrorMessage(wxString::FromUTF8(response["error"].value("message", "session/new failed")));
            FireEvent(e);
            return;
        }
        auto parsed = ParseSessionNewResult(response);
        if (!parsed) {
            clACPEvent e(wxEVT_ACP_ERROR);
            e.SetErrorMessage("session/new: failed to parse agent response");
            FireEvent(e);
            return;
        }
        clACPEvent e(wxEVT_ACP_SESSION_READY);
        e.SetSessionId(parsed->sessionId);
        FireEvent(e);
    });
    Send(MakeRequest(id, "session/new", SerializeSessionNewParams(params)));
}

void ACPClient::LoadSession(const SessionLoadParams& params)
{
    if (m_initResult && !m_initResult->agentCapabilities.loadSession) {
        clACPEvent e(wxEVT_ACP_ERROR);
        e.SetErrorMessage("session/load: agent does not support loadSession");
        FireEvent(e);
        return;
    }
    const int id = NextId();
    RegisterPending(id, [this, sessionId = params.sessionId](const json& response) {
        if (response.contains("error")) {
            clACPEvent e(wxEVT_ACP_ERROR);
            e.SetErrorMessage(wxString::FromUTF8(response["error"].value("message", "session/load failed")));
            FireEvent(e);
            return;
        }
        // session/load returns null on success; history was replayed via notifications.
        clACPEvent e(wxEVT_ACP_SESSION_READY);
        e.SetSessionId(sessionId);
        FireEvent(e);
    });
    Send(MakeRequest(id, "session/load", SerializeSessionLoadParams(params)));
}

int ACPClient::Prompt(const SessionPromptParams& params)
{
    const int id = NextId();

    RegisterPending(id, [this](const json& response) {
        if (response.contains("error")) {
            clACPEvent e(wxEVT_ACP_ERROR);
            e.SetErrorMessage(wxString::FromUTF8(response["error"].value("message", "Unknown error")));
            FireEvent(e);
            return;
        }
        auto result = ParseSessionPromptResult(response);
        if (result) {
            clACPEvent e(wxEVT_ACP_PROMPT_DONE);
            e.SetStopReason(result->stopReason);
            FireEvent(e);
        }
    });

    Send(MakeRequest(id, "session/prompt", SerializeSessionPromptParams(params)));
    return id;
}

void ACPClient::ListSessions(const SessionListParams& params)
{
    if (m_initResult && !m_initResult->agentCapabilities.sessionCapabilities.list) {
        clACPEvent e(wxEVT_ACP_ERROR);
        e.SetErrorMessage("session/list: agent does not support session listing");
        FireEvent(e);
        return;
    }
    const int id = NextId();
    RegisterPending(id, [this](const json& response) {
        if (response.contains("error")) {
            clACPEvent e(wxEVT_ACP_ERROR);
            e.SetErrorMessage(wxString::FromUTF8(response["error"].value("message", "session/list failed")));
            FireEvent(e);
            return;
        }
        auto parsed = ParseSessionListResult(response);
        if (!parsed) {
            clACPEvent e(wxEVT_ACP_ERROR);
            e.SetErrorMessage("session/list: failed to parse agent response");
            FireEvent(e);
            return;
        }
        clACPEvent e(wxEVT_ACP_SESSION_LIST);
        e.SetSessionListResult(*parsed);
        FireEvent(e);
    });
    Send(MakeRequest(id, "session/list", SerializeSessionListParams(params)));
}

void ACPClient::Cancel(const wxString& session_id)
{
    Send(MakeNotification("session/cancel", SerializeSessionCancelParams({session_id})));
}

void ACPClient::ResolvePermission(int rpc_id, const RequestPermissionResult& result)
{
    Send(SerializeRequestPermissionResult(rpc_id, result));
}

// -------------------------------------------------------
// Message dispatch (main thread)
// -------------------------------------------------------

void ACPClient::DispatchLine(const std::string& line)
{
    json msg;
    try {
        msg = json::parse(line);
    } catch (const std::exception& e) {
        clACPEvent evt(wxEVT_ACP_ERROR);
        evt.SetErrorMessage(wxString::FromUTF8("Failed to parse JSON-RPC message: ") + wxString::FromUTF8(e.what()));
        FireEvent(evt);
        return;
    }
    DispatchMessage(msg);
}

void ACPClient::DispatchMessage(const json& msg)
{
    if (!msg.contains("jsonrpc"))
        return;

    const bool has_method = msg.contains("method");
    const bool has_id = msg.contains("id") && msg["id"].is_number();

    if (has_method && !has_id) {
        // Notification from agent (no id)
        DispatchNotification(msg["method"].get<std::string>(), msg.value("params", json::object()));
    } else if (has_id && !has_method) {
        // Response to one of our requests
        const int id = msg["id"].get<int>();
        if (msg.contains("result")) {
            DispatchResponse(id, msg["result"]);
        } else if (msg.contains("error")) {
            DispatchResponse(id, msg);
        }
    } else if (has_method && has_id) {
        // Agent-initiated request (e.g. session/request_permission)
        const int id = msg["id"].get<int>();
        const std::string method = msg["method"].get<std::string>();
        const json params = msg.value("params", json::object());
        if (method == "session/request_permission") {
            auto parsed = ParseRequestPermissionParams(params);
            if (parsed) {
                clACPEvent e(wxEVT_ACP_PERMISSION);
                e.SetRpcId(id);
                e.SetPermissionRequest(*parsed);
                FireEvent(e);
            }
        }
    }
}

void ACPClient::DispatchResponse(int id, const json& result)
{
    auto it = m_pending.find(id);
    if (it == m_pending.end())
        return;
    // Move out before calling so the map is consistent if the callback
    // indirectly triggers another dispatch.
    ResponseCallback cb = std::move(it->second);
    m_pending.erase(it);
    cb(result);
}

void ACPClient::DispatchNotification(const std::string& method, const json& params)
{
    if (method != "session/update")
        return;

    auto parsed = ParseSessionUpdate(params);
    if (!parsed)
        return;

    std::visit(
        [this](const auto& update) {
            using T = std::decay_t<decltype(update)>;

            if constexpr (std::is_same_v<T, UpdateAgentMessageChunk> || std::is_same_v<T, UpdateUserMessageChunk>) {
                clACPEvent e(wxEVT_ACP_OUTPUT);
                e.SetMessageId(update.messageId);
                e.SetText(update.content.text);
                FireEvent(e);

            } else if constexpr (std::is_same_v<T, UpdatePlan>) {
                clACPEvent e(wxEVT_ACP_PLAN);
                e.SetPlan(update);
                FireEvent(e);

            } else if constexpr (std::is_same_v<T, UpdateToolCall>) {
                clACPEvent e(wxEVT_ACP_TOOL_CALL);
                e.SetToolCall(update);
                FireEvent(e);

            } else if constexpr (std::is_same_v<T, UpdateToolCallUpdate>) {
                clACPEvent e(wxEVT_ACP_TOOL_CALL_UPDATE);
                e.SetToolCallUpdate(update);
                FireEvent(e);

            } else if constexpr (std::is_same_v<T, UpdateUsage>) {
                clACPEvent e(wxEVT_ACP_USAGE);
                e.SetUsage(update);
                FireEvent(e);
            }
        },
        parsed->update);
}

// -------------------------------------------------------
// Default client info
// -------------------------------------------------------

InitializeParams ACPClient::MakeDefaultClientInfo()
{
    InitializeParams p;
    p.clientInfo = ClientInfo{.name = "codelite", .title = "CodeLite IDE", .version = "1.0.0"};
    return p;
}

} // namespace llm::acp
