# Agent Client Protocol (ACP) — CodeLite Integration

ACP is a JSON-RPC 2.0 protocol for communicating with AI coding agents over a stdio transport. The agent runs as a child process; messages are newline-delimited UTF-8 JSON objects exchanged over stdin/stdout.

## Spec links

| Resource | URL |
|----------|-----|
| Overview | https://agentclientprotocol.com/protocol/v1/overview.md |
| Initialisation | https://agentclientprotocol.com/protocol/v1/initialization.md |
| Session setup | https://agentclientprotocol.com/protocol/v1/session-setup.md |
| Prompt turn | https://agentclientprotocol.com/protocol/v1/prompt-turn.md |
| Tool calls & permissions | https://agentclientprotocol.com/protocol/v1/tool-calls.md |
| Content blocks | https://agentclientprotocol.com/protocol/v1/content.md |
| Transports | https://agentclientprotocol.com/protocol/v1/transports.md |
| Authentication | https://agentclientprotocol.com/protocol/v1/authentication.md |
| Session modes | https://agentclientprotocol.com/protocol/v1/session-modes.md |
| Session config options | https://agentclientprotocol.com/protocol/v1/session-config-options.md |
| Agent plan | https://agentclientprotocol.com/protocol/v1/agent-plan.md |
| Slash commands | https://agentclientprotocol.com/protocol/v1/slash-commands.md |
| Terminals | https://agentclientprotocol.com/protocol/v1/terminals.md |
| File system | https://agentclientprotocol.com/protocol/v1/file-system.md |
| Extensibility | https://agentclientprotocol.com/protocol/v1/extensibility.md |
| Full schema | https://agentclientprotocol.com/protocol/v1/schema.md |

### RFDs (Requests for Dialog — in-progress proposals)

| Topic | URL |
|-------|-----|
| Request cancellation | https://agentclientprotocol.com/rfds/request-cancellation.md |
| Session resume | https://agentclientprotocol.com/rfds/session-resume.md |
| Session delete | https://agentclientprotocol.com/rfds/session-delete.md |
| Session list | https://agentclientprotocol.com/rfds/session-list.md |
| Session close | https://agentclientprotocol.com/rfds/session-close.md |
| Session config options | https://agentclientprotocol.com/rfds/session-config-options.md |
| Session info update | https://agentclientprotocol.com/rfds/session-info-update.md |
| Session fork | https://agentclientprotocol.com/rfds/session-fork.md |
| Auth methods | https://agentclientprotocol.com/rfds/auth-methods.md |
| Logout method | https://agentclientprotocol.com/rfds/logout-method.md |
| Elicitation | https://agentclientprotocol.com/rfds/elicitation.md |
| Streamable HTTP / WebSocket transport | https://agentclientprotocol.com/rfds/streamable-http-websocket-transport.md |
| MCP over ACP | https://agentclientprotocol.com/rfds/mcp-over-acp.md |
| End-turn token usage | https://agentclientprotocol.com/rfds/end-turn-token-usage.md |
| Custom LLM endpoint | https://agentclientprotocol.com/rfds/custom-llm-endpoint.md |
| Next-edit suggestions | https://agentclientprotocol.com/rfds/next-edit-suggestions.md |
| Plan operations | https://agentclientprotocol.com/rfds/plan-operations.md |
| Proxy chains | https://agentclientprotocol.com/rfds/proxy-chains.md |
| Agent registry | https://agentclientprotocol.com/rfds/acp-agent-registry.md |
| Message ID | https://agentclientprotocol.com/rfds/message-id.md |

---

## Files

| File | Purpose |
|------|---------|
| `ACPTypes.hpp` | All protocol data structures and JSON helpers |
| `ACPEvent.hpp` / `ACPEvent.cpp` | `clACPEvent` class and `wxEVT_ACP_*` event type declarations |
| `ACPClient.hpp` / `ACPClient.cpp` | `ACPClient` implementation + serialisation/parsing |

All code lives in the `llm::acp` namespace except `clACPEvent` which is in the global namespace (following the CodeLite convention for event classes).

---

## Typical Lifecycle

```cpp
// ACPClient is a wxEvtHandler subclass — bind directly to it
ACPClient client;

// Register for protocol events
client.Bind(wxEVT_ACP_OUTPUT,           &MyClass::OnOutput,      this);
client.Bind(wxEVT_ACP_PROMPT_DONE,      &MyClass::OnPromptDone,  this);
client.Bind(wxEVT_ACP_TOOL_CALL,        &MyClass::OnToolCall,    this);
client.Bind(wxEVT_ACP_TOOL_CALL_UPDATE, &MyClass::OnToolUpdate,  this);
client.Bind(wxEVT_ACP_USAGE,            &MyClass::OnUsage,       this);
client.Bind(wxEVT_ACP_PLAN,             &MyClass::OnPlan,        this);
client.Bind(wxEVT_ACP_PERMISSION,       &MyClass::OnPermission,  this);
client.Bind(wxEVT_ACP_ERROR,            &MyClass::OnError,       this);
client.Bind(wxEVT_ACP_TERMINATED,       &MyClass::OnTerminated,  this);

// Or redirect all events to a different handler:
// client.SetEventSink(someOtherHandler);

// Spawn agent
wxArrayString argv;
argv.Add("/path/to/agent");
argv.Add("--stdio");
client.Start(argv);

// Negotiate protocol version and capabilities (blocking, up to 30 s)
auto init = client.Initialize();

// Create a session (blocking, up to 30 s)
auto session = client.NewSession({ .cwd = "/home/user/project", .mcpServers = {} });

// Send a prompt (async — events fire as updates arrive)
client.Prompt({
    .sessionId = session->sessionId,
    .prompt    = { llm::acp::ContentBlockText{ "Explain this file" } },
});

// Cancel mid-turn if needed
client.Cancel(session->sessionId);

// Shut down
client.Stop();
```

---

## Events Reference

### `wxEVT_ACP_OUTPUT`

Fires for each streaming text chunk from the agent (`agent_message_chunk`) and for replayed user messages during `session/load` (`user_message_chunk`).

```cpp
void OnOutput(clACPEvent& event) {
    // Chunks sharing the same messageId belong to one logical message.
    auto id   = event.GetMessageId(); // std::optional<wxString>
    auto text = event.GetText();      // wxString — the chunk content
}
```

### `wxEVT_ACP_PROMPT_DONE`

Fires once when a `session/prompt` round-trip completes.

```cpp
void OnPromptDone(clACPEvent& event) {
    switch (event.GetStopReason()) {
    case llm::acp::StopReason::end_turn:         /* normal */    break;
    case llm::acp::StopReason::cancelled:        /* cancelled */ break;
    case llm::acp::StopReason::max_tokens:       /* hit limit */ break;
    case llm::acp::StopReason::refusal:          /* refused */   break;
    case llm::acp::StopReason::max_turn_requests:               break;
    }
}
```

### `wxEVT_ACP_TOOL_CALL`

Fires when the agent reports a new tool invocation.

```cpp
void OnToolCall(clACPEvent& event) {
    const auto& tc = *event.GetToolCall(); // llm::acp::UpdateToolCall
    // tc.toolCallId, tc.title, tc.kind, tc.status, tc.locations, tc.rawInput
}
```

### `wxEVT_ACP_TOOL_CALL_UPDATE`

Fires when the agent updates the status of a previously reported tool call.

```cpp
void OnToolCallUpdate(clACPEvent& event) {
    const auto& tcu = *event.GetToolCallUpdate(); // llm::acp::UpdateToolCallUpdate
    // tcu.toolCallId, tcu.status
}
```

### `wxEVT_ACP_USAGE`

Fires when the agent sends a token / cost update.

```cpp
void OnUsage(clACPEvent& event) {
    const auto& u = *event.GetUsage(); // llm::acp::UpdateUsage
    // u.used (tokens used), u.size (context size)
    // u.costAmount, u.costCurrency (optional)
}
```

### `wxEVT_ACP_PLAN`

Fires when the agent publishes an execution plan.

```cpp
void OnPlan(clACPEvent& event) {
    const auto& plan = *event.GetPlan(); // llm::acp::UpdatePlan
    for (const auto& entry : plan.entries) {
        // entry.content, entry.priority, entry.status
    }
}
```

### `wxEVT_ACP_PERMISSION`

Fires when the agent requests permission to invoke a tool. The handler **must** call `ResolvePermission` before returning, even when the turn is being cancelled.

```cpp
void OnPermission(clACPEvent& event) {
    const auto& req = *event.GetPermissionRequest(); // llm::acp::RequestPermissionParams
    // req.toolCallId, req.options (id, name, kind)

    // Allow once:
    client.ResolvePermission(event.GetRpcId(),
        { llm::acp::PermissionOutcomeSelected{ req.options[0].optionId } });

    // Or reject / cancel:
    client.ResolvePermission(event.GetRpcId(),
        { llm::acp::PermissionOutcomeCancelled{} });
}
```

#### Rejecting a tool with a "do this instead" message

ACP v1 has **no free-text field on permission outcomes** — rejection is always a bare option selection. There is no way to pass a reason or alternative instruction back to the agent as part of the `ResolvePermission` call itself.

The closest workaround is a two-step sequence:

1. Reject the tool call with the appropriate `reject_*` option. The current prompt turn then ends with `StopReason::end_turn` (the agent sees a rejection result for the tool and stops).
2. When `wxEVT_ACP_PROMPT_DONE` fires, immediately send a new `Prompt` in the same session carrying the alternative instruction.

The agent's context already contains the rejected tool call, so the follow-up message is understood as a correction.

```cpp
// Step 1 — reject in the permission handler
void MyClass::OnPermission(clACPEvent& event)
{
    const auto& req = *event.GetPermissionRequest();

    // Find the "reject once" option the agent offered
    wxString rejectId;
    for (const auto& opt : req.options) {
        if (opt.kind == "reject_once") {
            rejectId = opt.optionId;
            break;
        }
    }

    m_pendingAlternative = "Use ReadFile instead of executing a shell command";
    m_currentSessionId   = req.sessionId;

    client.ResolvePermission(event.GetRpcId(),
        { llm::acp::PermissionOutcomeSelected{ rejectId } });
}

// Step 2 — send the alternative instruction once the turn ends
void MyClass::OnPromptDone(clACPEvent& event)
{
    if (!m_pendingAlternative.empty()) {
        client.Prompt({
            .sessionId = m_currentSessionId,
            .prompt    = { llm::acp::ContentBlockText{ m_pendingAlternative } },
        });
        m_pendingAlternative.clear();
    }
}
```

This is a protocol limitation. The [elicitation RFD](https://agentclientprotocol.com/rfds/elicitation.md) and [plan operations RFD](https://agentclientprotocol.com/rfds/plan-operations.md) are the active proposals that may eventually enable richer feedback flows in v2.

---

### `wxEVT_ACP_ERROR`

Fires on any transport or JSON-RPC parse error.

```cpp
void OnError(clACPEvent& event) {
    wxLogError("ACP error: %s", event.GetErrorMessage());
}
```

### `wxEVT_ACP_TERMINATED`

Fires when the agent process exits unexpectedly (i.e. not via `Stop()`).

```cpp
void OnTerminated(clACPEvent& event) {
    wxUnusedVar(event);
    // reconnect or notify the user
}
```

---

## ACPClient API Reference

### Event sink

| Method | Description |
|--------|-------------|
| `SetEventSink(handler)` | Redirect all `wxEVT_ACP_*` events to `handler`. If not set (or `nullptr`) events are fired on `*this`. |

### Lifecycle

| Method | Description |
|--------|-------------|
| `Start(argv)` | Spawn the agent process (`wxArrayString`; `argv[0]` = executable). Calls `Stop()` first if already running. |
| `Stop()` | Terminate the child process and fail all pending requests. |
| `IsRunning()` | Returns `true` while the child process is alive. |

### Protocol methods

| Method | Blocking | Description |
|--------|----------|-------------|
| `Initialize(params)` | Yes (30 s) | Version negotiation. Call once after `Start`. |
| `NewSession(params)` | Yes (30 s) | Create a fresh conversation session. |
| `LoadSession(params)` | Yes (60 s) | Restore a prior session (requires `agentCapabilities.loadSession`). History is streamed back as `wxEVT_ACP_OUTPUT` events before the call returns. |
| `Prompt(params)` | No | Send a user turn. Returns the JSON-RPC id. |
| `Cancel(sessionId)` | No | Send `session/cancel`. Turn ends with `StopReason::cancelled`. |
| `ResolvePermission(rpcId, result)` | No | Respond to a `wxEVT_ACP_PERMISSION` event. |

---

## Thread Safety

`ACPClient` inherits `wxEvtHandler`. All ChildProcess I/O events (and therefore all `wxEVT_ACP_*` firings) arrive on the **main thread** via `AddPendingEvent`. The blocking methods (`Initialize`, `NewSession`, `LoadSession`) use a condition variable internally and must **not** be called from within a `wxEVT_ACP_*` handler, or a deadlock will result. Call them from the worker thread that owns the session setup logic.

---

## What Is Not Yet Implemented

- HTTP / Streamable-HTTP transport (stdio only for now)
- `authenticate` / `logout` methods
- `session/set_mode`, `session/delete`, `session/list`
- `fs/*` and `terminal/*` client-side methods
- Wiring into `llm::Manager` (process launch, endpoint config, UI)
