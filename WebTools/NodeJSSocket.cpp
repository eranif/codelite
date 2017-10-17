#include "NodeJSSocket.h"
#include "NodeJSDebugger.h"
#include "file_logger.h"
#include "NodeJSHandlerBase.h"
#include <wx/regex.h>

NodeJSSocket::NodeJSSocket(NodeJSDebugger* debugger)
    : m_socket(this)
    , m_connected(false)
    , m_sequence(0)
    , m_debugger(debugger)
    , m_firstTimeConnected(true)
{
    Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &NodeJSSocket::OnSocketConnected, this);
    Bind(wxEVT_ASYNC_SOCKET_ERROR, &NodeJSSocket::OnSocketError, this);
    Bind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &NodeJSSocket::OnSocketConnectionLost, this);
    Bind(wxEVT_ASYNC_SOCKET_INPUT, &NodeJSSocket::OnSocketInput, this);
    Bind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &NodeJSSocket::OnSocketConnectError, this);

    // set of commands that we don't ask for backtrace
    // from Node.js after their execution
    m_noBacktraceCommands.insert("backtrace");
    m_noBacktraceCommands.insert("setbreakpoint");
    m_noBacktraceCommands.insert("frame");
    m_noBacktraceCommands.insert("source");
    m_noBacktraceCommands.insert("evaluate");
    m_noBacktraceCommands.insert("lookup");
}

NodeJSSocket::~NodeJSSocket() { Destroy(); }

void NodeJSSocket::OnSocketConnected(clCommandEvent& event)
{
    CL_DEBUG("CodeLite >>>> Connection established with Node.js");
    // socket is now connected
    m_debugger->CallAfter(&NodeJSDebugger::ConnectionEstablished);
    m_connected = true;
}

void NodeJSSocket::OnSocketError(clCommandEvent& event)
{
    m_errorString = event.GetString();
    CL_DEBUG("Socket error: %s", m_errorString);
    Destroy();
    m_debugger->CallAfter(&NodeJSDebugger::ConnectionLost, m_errorString);
}

void NodeJSSocket::OnSocketConnectionLost(clCommandEvent& event)
{
    CL_DEBUG("CodeLite >>>> Lost connection to Node.js");
    Destroy();
    m_debugger->CallAfter(&NodeJSDebugger::ConnectionLost, m_errorString);
}

void NodeJSSocket::OnSocketInput(clCommandEvent& event)
{
    CL_DEBUG("CodeLite >>>> Got something on the socket...");
    m_inBuffer << event.GetString();
    CL_DEBUG("Node.js  <<<< %s", m_inBuffer);
    ProcessInputBuffer();
}

void NodeJSSocket::OnSocketConnectError(clCommandEvent& event)
{
    CL_DEBUG("CodeLite >>>> connect error");
    m_errorString = event.GetString();
    m_debugger->CallAfter(&NodeJSDebugger::ConnectError, m_errorString);
    Destroy();
}

void NodeJSSocket::Destroy()
{
    Unbind(wxEVT_ASYNC_SOCKET_CONNECTED, &NodeJSSocket::OnSocketConnected, this);
    Unbind(wxEVT_ASYNC_SOCKET_ERROR, &NodeJSSocket::OnSocketError, this);
    Unbind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &NodeJSSocket::OnSocketConnectionLost, this);
    Unbind(wxEVT_ASYNC_SOCKET_INPUT, &NodeJSSocket::OnSocketInput, this);
    Unbind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &NodeJSSocket::OnSocketInput, this);

    m_socket.Disconnect();
    m_firstTimeConnected = true;
    m_connected = false;
    CL_DEBUG("CodeLite >>>> Cleaning up socket with Node.js");
}

void NodeJSSocket::WriteReply(const wxString& reply)
{
    if(!IsConnected()) return;
    wxString content;
    content << "Content-Length:" << reply.length() << "\r\n\r\n";
    content << reply;
    m_socket.Send(content);
}

void NodeJSSocket::ProcessInputBuffer()
{
    if(m_firstTimeConnected) {
        m_firstTimeConnected = false;
        // Apply breakpoints
        m_debugger->SetBreakpoints();
        // When an uncaught exception is thrown, break
        m_debugger->BreakOnException();
        m_inBuffer.Clear();
        // Let codelite know that we have control
        m_debugger->GotControl(true);
    } else {

        wxString buffer = GetResponse();
        while(!buffer.IsEmpty()) {
            JSONRoot root(buffer);
            JSONElement json = root.toElement();
            int reqSeq = json.namedObject("request_seq").toInt();
            if(reqSeq != wxNOT_FOUND) {
                std::map<size_t, NodeJSHandlerBase::Ptr_t>::iterator iter = m_handlers.find((size_t)reqSeq);
                if(iter != m_handlers.end()) {
                    NodeJSHandlerBase::Ptr_t handler = iter->second;
                    handler->Process(m_debugger, buffer);
                    m_handlers.erase(iter);
                }

                if(json.hasNamedObject("running") && !json.namedObject("running").toBool()) {
                    wxString responseCommand = json.namedObject("command").toString();
                    m_debugger->GotControl((m_noBacktraceCommands.count(responseCommand) == 0));
                } else {
                    m_debugger->SetCanInteract(false);
                }
            } else {

                // Notify the debugger that we got control
                if((json.namedObject("type").toString() == "event")) {
                    if(json.namedObject("event").toString() == "break") {
                        // breakpoint hit, notify we got control + request for backtrace
                        m_debugger->GotControl(true);
                    } else if(json.namedObject("event").toString() == "exception") {

                        JSONElement body = json.namedObject("body");
                        NodeJSDebuggerException exc;
                        exc.message = body.namedObject("exception").namedObject("text").toString();
                        ;
                        exc.line = body.namedObject("sourceLine").toInt();
                        exc.script = body.namedObject("script").namedObject("name").toString();
                        exc.column = body.namedObject("sourceColumn").toInt();

                        // the vm execution stopped due to an exception
                        m_debugger->ExceptionThrown(exc);
                    }
                } else {
                    m_debugger->SetCanInteract(false);
                }
            }
            // Check to see if we got more reponses in the in-buffer
            buffer = GetResponse();
        }
    }
}

void NodeJSSocket::Connect(const wxString& ip, int port)
{
    m_socket.Connect((wxString() << "tcp://" << ip << ":" << port), "");
}

void NodeJSSocket::WriteRequest(JSONElement& request, NodeJSHandlerBase::Ptr_t handler)
{
    if(!IsConnected()) return;
    size_t seq = NextSequence();
    request.addProperty("seq", seq);

    wxString content, str;
    str = request.format();
    content << "Content-Length:" << str.length() << "\r\n\r\n";
    content << str;

    CL_DEBUG("CodeLite >>>> %s", content);
    m_socket.Send(content);

    // Keep the handler
    if(handler) {
        m_handlers.insert(std::make_pair(seq, handler));
    }
}

wxString NodeJSSocket::GetResponse()
{
    wxRegEx re("Content-Length:[ ]*([0-9]+)", wxRE_ADVANCED);
    if(re.Matches(m_inBuffer)) {
        size_t start, wholeline_len;
        if(!re.GetMatch(&start, &wholeline_len, 0)) {
            return "";
        }

        // Remove anything before the 'start' position
        m_inBuffer = m_inBuffer.Mid(start);

        // At this point, start = 0
        start = 0;
        wxString wholine = m_inBuffer.Mid(start, wholeline_len);

        // wholine should now contains a string like:
        // Content-Length: 1234
        // Extract the message length
        wxRegEx reContentLen("[0-9]+");
        if(!reContentLen.Matches(wholine)) return ""; // Can't happen...
        long messageLength;
        if(!reContentLen.GetMatch(wholine).ToCLong(&messageLength)) {
            // just to be safe
            return "";
        }

        // Remove the "Content-Length: NN\r\n\r\n"
        size_t headerLen = wholeline_len + 4;

        // Did we read enough from the socket to process?
        if(m_inBuffer.length() >= (messageLength + headerLen)) {
            m_inBuffer = m_inBuffer.Mid(headerLen);
            wxString response = m_inBuffer.Mid(0, messageLength);
            // Remove this message from the input buffer
            m_inBuffer = m_inBuffer.Mid(messageLength);
            return response;

        } else {
            return "";
        }
    }
    return "";
}

void NodeJSSocket::Shutdown() { m_socket.Disconnect(); }
