#include "NodeDebugger.h"
#include "NodeFileManager.h"
#include "NodeJSDevToolsProtocol.h"
#include "NodeJSEvents.h"
#include "RemoteObject.h"
#include "SocketAPI/clSocketBase.h"
#include "clDebugRemoteObjectEvent.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "JSON.h"

class RemoteObject;
NodeJSDevToolsProtocol::NodeJSDevToolsProtocol() {}

NodeJSDevToolsProtocol::~NodeJSDevToolsProtocol() {}

NodeJSDevToolsProtocol& NodeJSDevToolsProtocol::Get()
{
    static NodeJSDevToolsProtocol protocol;
    return protocol;
}

void NodeJSDevToolsProtocol::SendSimpleCommand(clWebSocketClient& socket, const wxString& command,
                                               const JSONItem& params)
{
    try {
        JSON root(cJSON_Object);
        JSONItem e = root.toElement();
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
    JSON root(msg);
    if(!root.isOk()) { return; }
    JSONItem rootElement = root.toElement();
    wxString method = rootElement.namedObject("method").toString();
    long replyId = rootElement.namedObject("id").toInt(wxNOT_FOUND);
    if(!method.IsEmpty()) {
        // Debugger event arrived
        NodeMessageBase::Ptr_t handler = m_handlers.GetHandler(method);
        if(handler) {
            clDEBUG() << "Invoking handler:" << handler->GetEventName();
            handler->Process(socket, rootElement.namedObject("params"));
        }
    } else if(replyId != wxNOT_FOUND) {
        if(rootElement.hasNamedObject("result")) {
            // Invoke the action for this reply
            if(m_waitingReplyCommands.count(replyId)) {
                CommandHandler& handler = m_waitingReplyCommands[replyId];
                if(handler.action) { handler.action(rootElement.namedObject("result")); }
                // Delete the handler
                m_waitingReplyCommands.erase(replyId);
            }
        }
    }
}

void NodeJSDevToolsProtocol::SendStartCommands(clWebSocketClient& socket)
{
    SendSimpleCommand(socket, "Runtime.enable");
    SendSimpleCommand(socket, "Debugger.enable");
    {
        JSONItem params = JSONItem::createObject("params");
        params.addProperty("state", "uncaught");
        SendSimpleCommand(socket, "Debugger.setPauseOnExceptions", params);
    }
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
        JSONItem params = bp.ToJSON("params");
        // Nodejs is using 0 based line numbers, while the editor starts from 1
        params.removeProperty("lineNumber");
        params.addProperty("lineNumber", bp.GetLine() - 1);
        // Send the command
        SendSimpleCommand(socket, "Debugger.setBreakpointByUrl", params);

        // Register a handler to handle this command when it returns
        CommandHandler handler(message_id, [=](const JSONItem& result) {
            wxString breakpointId = result.namedObject("breakpointId").toString();
            NodeJSBreakpoint& b = m_debugger->GetBreakpointsMgr()->GetBreakpoint(bp.GetFilename(), bp.GetLine());
            if(b.IsOk()) {
                b.SetNodeBpID(breakpointId);
                clDebugEvent bpEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
                EventNotifier::Get()->AddPendingEvent(bpEvent);
            }
        });
        m_waitingReplyCommands.insert({ handler.m_commandID, handler });
    } catch(clSocketException& e) {
        clWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::DeleteBreakpoint(clWebSocketClient& socket, const NodeJSBreakpoint& bp)
{
    try {
        JSONItem params = JSONItem::createObject("params");
        params.addProperty("breakpointId", bp.GetNodeBpID());
        // Send the command
        SendSimpleCommand(socket, "Debugger.removeBreakpoint", params);

        // Register a handler to handle this command when it returns
        CommandHandler handler(message_id, [=](const JSONItem& result) {
            clDebugEvent bpEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
            EventNotifier::Get()->AddPendingEvent(bpEvent);
        });
        m_waitingReplyCommands.insert({ handler.m_commandID, handler });
    } catch(clSocketException& e) {
        clWARNING() << e.what();
    }
}

void NodeJSDevToolsProtocol::GetScriptSource(clWebSocketClient& socket, const wxString& scriptId)
{
    JSONItem params = JSONItem::createObject("params");
    params.addProperty("scriptId", scriptId);
    // Send the command
    SendSimpleCommand(socket, "Debugger.getScriptSource", params);
    // Register a handler to handle this command when it returns
    CommandHandler handler(message_id, [=](const JSONItem& result) {
        wxString fileContent = result.namedObject("scriptSource").toString();
        NodeFileManager::Get().CacheRemoteCopy(scriptId, fileContent);
    });
    m_waitingReplyCommands.insert({ handler.m_commandID, handler });
}

void NodeJSDevToolsProtocol::Eval(clWebSocketClient& socket, const wxString& expression, const wxString& frameId)
{
    JSONItem params = JSONItem::createObject("params");
    params.addProperty("callFrameId", frameId);
    params.addProperty("expression", expression);
    params.addProperty("generatePreview", true);
    SendSimpleCommand(socket, "Debugger.evaluateOnCallFrame", params);
    // Register a handler to handle this command when it returns
    CommandHandler handler(message_id, [=](const JSONItem& result) {
        if(result.hasNamedObject("result")) {
            nSerializableObject::Ptr_t ro(new RemoteObject());
            ro->To<RemoteObject>()->SetExpression(expression);
            ro->To<RemoteObject>()->FromJSON(result.namedObject("result"));
            clDebugRemoteObjectEvent evt(wxEVT_NODEJS_DEBUGGER_EVAL_RESULT);
            evt.SetRemoteObject(ro);
            EventNotifier::Get()->AddPendingEvent(evt);
        }
    });
    m_waitingReplyCommands.insert({ handler.m_commandID, handler });
}

void NodeJSDevToolsProtocol::GetObjectProperties(clWebSocketClient& socket, const wxString& objectId,
                                                 wxEventType eventType)
{
    JSONItem params = JSONItem::createObject("params");
    params.addProperty("objectId", objectId);
    SendSimpleCommand(socket, "Runtime.getProperties", params);
    // Register a handler to handle this command when it returns
    CommandHandler handler(message_id, [=](const JSONItem& result) {
        if(result.hasNamedObject("result")) {
            clDebugEvent evt(eventType);
            evt.SetString(result.namedObject("result").format(false));
            evt.SetStartupCommands(objectId);
            EventNotifier::Get()->AddPendingEvent(evt);
        }
    });
    m_waitingReplyCommands.insert({ handler.m_commandID, handler });
}

void NodeJSDevToolsProtocol::CreateObject(clWebSocketClient& socket, const wxString& expression,
                                          const wxString& frameId)
{
    // Same as eval but without the preview
    JSONItem params = JSONItem::createObject("params");
    params.addProperty("callFrameId", frameId);
    params.addProperty("expression", expression);
    params.addProperty("generatePreview", false);
    SendSimpleCommand(socket, "Debugger.evaluateOnCallFrame", params);
    // Register a handler to handle this command when it returns
    CommandHandler handler(message_id, [=](const JSONItem& result) {
        if(result.hasNamedObject("result")) {
            nSerializableObject::Ptr_t ro(new RemoteObject());
            ro->To<RemoteObject>()->SetExpression(expression);
            ro->To<RemoteObject>()->FromJSON(result.namedObject("result"));
            clDebugRemoteObjectEvent evt(wxEVT_NODEJS_DEBUGGER_CREATE_OBJECT);
            evt.SetRemoteObject(ro);
            EventNotifier::Get()->AddPendingEvent(evt);
        }
    });
    m_waitingReplyCommands.insert({ handler.m_commandID, handler });
}

void NodeJSDevToolsProtocol::DeleteBreakpointByID(clWebSocketClient& socket, const wxString& bpid)
{
    try {
        JSONItem params = JSONItem::createObject("params");
        params.addProperty("breakpointId", bpid);
        // Send the command
        SendSimpleCommand(socket, "Debugger.removeBreakpoint", params);
    } catch(clSocketException& e) {
        clWARNING() << e.what();
    }
}
