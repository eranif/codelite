#include "NodeJSDevToolsProtocol.h"
#include "SocketAPI/clSocketBase.h"
#include "file_logger.h"
#include "json_node.h"

NodeJSDevToolsProtocol::NodeJSDevToolsProtocol() {}

NodeJSDevToolsProtocol::~NodeJSDevToolsProtocol() {}

void NodeJSDevToolsProtocol::SendSimpleCommand(clWebSocketClient& socket, const wxString& command)
{
    try {
        JSONRoot root(cJSON_Object);
        JSONElement e = root.toElement();
        e.addProperty("id", ++message_id);
        e.addProperty("method", command);
        socket.Send(e.format(false));
    } catch(clSocketException& e) {
        clWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::ProcessMessage(const wxString& msg, clWebSocketClient& socket)
{
    JSONRoot root(msg);
    if(!root.isOk()) { return; }
    if(!root.toElement().hasNamedObject("method")) { return; }
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
        // Invoke the action for this reply
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
