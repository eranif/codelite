#ifndef NODEJSDEVTOOLSPROTOCOL_H
#define NODEJSDEVTOOLSPROTOCOL_H

#include "MessageManager.h"
#include "NodeJSDebuggerBreakpoint.h"
#include "SocketAPI/clWebSocketClient.h"
#include <unordered_map>
#include <wx/arrstr.h>

class NodeDebugger;
typedef std::function<void(const JSONItem&)> ResultFunc_t;
class CommandHandler
{
public:
    ResultFunc_t action = nullptr;
    long m_commandID = wxNOT_FOUND;
    CommandHandler() {}
    CommandHandler(long commandID, const ResultFunc_t& func)
        : action(func)
        , m_commandID(commandID)
    {
    }
};

class NodeJSDevToolsProtocol
{
    NodeDebugger* m_debugger = nullptr;
    long message_id = 0;
    MessageManager m_handlers;
    std::unordered_map<long, CommandHandler> m_waitingReplyCommands;

protected:
    void SendSimpleCommand(clWebSocketClient& socket, const wxString& command,
                           const JSONItem& params = JSONItem(NULL));

    NodeJSDevToolsProtocol();

public:
    static NodeJSDevToolsProtocol& Get();
    virtual ~NodeJSDevToolsProtocol();
    void Clear();

    void SetDebugger(NodeDebugger* debugger) { this->m_debugger = debugger; }
    NodeDebugger* GetDebugger() { return m_debugger; }
    void SendStartCommands(clWebSocketClient& socket);
    void ProcessMessage(const wxString& msg, clWebSocketClient& socket);
    void Next(clWebSocketClient& socket);
    void StepIn(clWebSocketClient& socket);
    void StepOut(clWebSocketClient& socket);
    void Continue(clWebSocketClient& socket);
    void SetBreakpoint(clWebSocketClient& socket, const NodeJSBreakpoint& bp);
    void DeleteBreakpoint(clWebSocketClient& socket, const NodeJSBreakpoint& bp);
    void DeleteBreakpointByID(clWebSocketClient& socket, const wxString& bpid);
    void GetScriptSource(clWebSocketClient& socket, const wxString& scriptId);
    void Eval(clWebSocketClient& socket, const wxString& expression, const wxString& frameId);
    void GetObjectProperties(clWebSocketClient& socket, const wxString& objectId, wxEventType eventType);
    void CreateObject(clWebSocketClient& socket, const wxString& expression, const wxString& frameId);
};

#endif // NODEJSDEVTOOLSPROTOCOL_H
