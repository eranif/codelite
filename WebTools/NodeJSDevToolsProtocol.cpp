#include "NodeDebugger.h"
#include "NodeJSDevToolsProtocol.h"
#include "SocketAPI/clSocketBase.h"
#include "file_logger.h"
#include "json_node.h"

NodeJSDevToolsProtocol::NodeJSDevToolsProtocol(NodeDebugger* debugger)
    : m_debugger(debugger)
{
}

NodeJSDevToolsProtocol::~NodeJSDevToolsProtocol() {}

void NodeJSDevToolsProtocol::SendSimpleCommand(clWebSocketClient& socket, const wxString& command,
                                               const JSONElement& params)
{
    try {
        JSONRoot root(cJSON_Object);
        JSONElement e = root.toElement();
        e.addProperty("id", ++message_id);
        e.addProperty("method", command);
        if(params.isOk()) { e.append(params); }
        wxString command = e.format(false);
        clDEBUG() << "-->" << command;
        socket.Send(command);
    } catch(clSocketException& e) {
        clWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::ProcessMessage(const wxString& msg, clWebSocketClient& socket)
{
    JSONRoot root(msg);
    if(!root.isOk()) { return; }
    JSONElement rootElement = root.toElement();
    wxString method = rootElement.namedObject("method").toString();
    long replyId = rootElement.namedObject("id").toInt(wxNOT_FOUND);
    if(!method.IsEmpty()) {
        // Debugger event arrived
        NodeMessageBase::Ptr_t handler = m_handlers.GetHandler(method);
        if(handler) {
            clDEBUG() << "Invoking handler:" << handler->GetEventName();
            handler->Process(rootElement.namedObject("params"));
        }
    } else if(replyId != wxNOT_FOUND) {
        if(rootElement.hasNamedObject("result")) {
            // Invoke the action for this reply
            if(m_waitingReplyCommands.count(replyId)) {
                CommandHandler& handler = m_waitingReplyCommands[replyId];
                if(handler.action) { handler.action(rootElement.namedObject("result")); }
            }
        }
    }
}

void NodeJSDevToolsProtocol::SendStartCommands(clWebSocketClient& socket)
{
    SendSimpleCommand(socket, "Runtime.enable");
    SendSimpleCommand(socket, "Debugger.enable");
    SendSimpleCommand(socket, "Runtime.runIfWaitingForDebugger");
}

void NodeJSDevToolsProtocol::Next(clWebSocketClient& socket) { SendSimpleCommand(socket, "Debugger.stepOver"); }

void NodeJSDevToolsProtocol::StepIn(clWebSocketClient& socket) { SendSimpleCommand(socket, "Debugger.stepInto"); }

void NodeJSDevToolsProtocol::StepOut(clWebSocketClient& socket) { SendSimpleCommand(socket, "Debugger.stepOut"); }

void NodeJSDevToolsProtocol::Continue(clWebSocketClient& socket) { SendSimpleCommand(socket, "Debugger.resume"); }

void NodeJSDevToolsProtocol::Clear() { m_waitingReplyCommands.clear(); }

void NodeJSDevToolsProtocol::SetBreakpoint(clWebSocketClient& socket, const NodeJSBreakpoint& bp)
{
    try {
        JSONElement params = bp.ToJSON("params");
        // Nodejs is using 0 based line numbers, while the editor starts from 1
        params.removeProperty("lineNumber");
        params.addProperty("lineNumber", bp.GetLine() - 1);
        // Send the command
        SendSimpleCommand(socket, "Debugger.setBreakpointByUrl", params);
        
        // Register a handler to handle this command when it returns
        CommandHandler handler(message_id, [&](const JSONElement& result) {
            wxString breakpointId = result.namedObject("breakpointId").toString();
            NodeJSBreakpoint& b = m_debugger->GetBreakpointsMgr()->GetBreakpoint(bp.GetFilename(), bp.GetLine());
            if(b.IsOk()) { b.SetNodeBpID(breakpointId); }
        });
        m_waitingReplyCommands.insert({ handler.m_commandID, handler });
    } catch(clSocketException& e) {
        clWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::DeleteBreakpoint(clWebSocketClient& socket, const NodeJSBreakpoint& bp) {}
