#include "NodeJSDevToolsProtocol.h"
#include "SocketAPI/clSocketBase.h"
#include "file_logger.h"
#include "json_node.h"

NodeJSDevToolsProtocol::NodeJSDevToolsProtocol() {}

NodeJSDevToolsProtocol::~NodeJSDevToolsProtocol() {}

void NodeJSDevToolsProtocol::SendStartCommands(clWebSocketClient& socket)
{
    try {
        {
            JSONRoot root(cJSON_Object);
            JSONElement e = root.toElement();
            e.addProperty("id", ++message_id);
            e.addProperty("method", "Runtime.enable");
            socket.Send(e.format(false));
        }
        {
            JSONRoot root(cJSON_Object);
            JSONElement e = root.toElement();
            e.addProperty("id", ++message_id);
            e.addProperty("method", "Debugger.enable");

            socket.Send(e.format(false));
        }
        {
            JSONRoot root(cJSON_Object);
            JSONElement e = root.toElement();
            e.addProperty("id", ++message_id);
            e.addProperty("method", "Runtime.runIfWaitingForDebugger");

            socket.Send(e.format(false));
        }
    } catch(clSocketException& e) {
        clWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::ProcessMessage(const wxString& msg, clWebSocketClient& socket)
{
    JSONRoot root(msg);
    if(!root.isOk()) { return; }
    if(!root.toElement().hasNamedObject("method")) { return; }
    wxString method = root.toElement().namedObject("method").toString();
    if(method.IsEmpty()) { return; }

    NodeDbgEventBase::Ptr_t handler = m_handlers.GetHandler(method);
    if(handler) { 
        clDEBUG() << "Invoking handler:" << handler->GetEventName();
        handler->Process(root.toElement().namedObject("params")); 
    }
}
