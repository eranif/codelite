#include "NodeJSSocket.h"
#include "NodeJSDebugger.h"
#include "file_logger.h"
#include "NodeJSHandlerBase.h"
#include <wx/regex.h>

NodeJSSocket::NodeJSSocket(NodeJSDebugger* debugger)
    : m_socket(NULL)
    , m_sequence(0)
    , m_debugger(debugger)
    , m_firstTimeConnected(true)
{
    m_socket = new wxSocketClient(wxSOCKET_NOWAIT_READ);
    m_socket->SetEventHandler(*this);
    m_socket->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    m_socket->Notify(true);
    Bind(wxEVT_SOCKET, &NodeJSSocket::OnSocketEvent, this);
}

NodeJSSocket::~NodeJSSocket() { Destroy(); }

void NodeJSSocket::OnSocketEvent(wxSocketEvent& event)
{
    switch(event.GetSocketEvent()) {
    case wxSOCKET_INPUT: {
        // Read all the content from the socket
        ReadSocketContent();
        ProcessInputBuffer();
        break;
    }
    case wxSOCKET_LOST:
        Destroy();
        m_debugger->CallAfter(&NodeJSDebugger::ConnectionLost);
        break;

    case wxSOCKET_CONNECTION:
        // socket is now connected
        m_debugger->CallAfter(&NodeJSDebugger::ConnectionEstablished);
        break;

    default:
        break;
    }
}

void NodeJSSocket::Destroy()
{
    Unbind(wxEVT_SOCKET, &NodeJSSocket::OnSocketEvent, this);
    if(m_socket) {
        m_socket->Shutdown();
        wxDELETE(m_socket);
    }
    m_firstTimeConnected = true;
}

void NodeJSSocket::ReadSocketContent()
{
    char buff[1024];
    while(true) {
        memset(buff, 0, sizeof(buff));
        m_socket->Read(buff, (sizeof(buff) - 1));
        if(m_socket->LastReadCount() == 0) { // EOF
            break;
        }
        m_inBuffer << (const char*)buff;
    }
    CL_DEBUG("Node.js  <<<< %s", m_inBuffer);
}

void NodeJSSocket::WriteReply(const wxString& reply)
{
    if(!IsConnected()) return;
    wxString content;
    content << "Content-Length:" << reply.length() << "\r\n\r\n";
    content << reply;

    wxCharBuffer cb = content.mb_str(wxConvUTF8).data();
    m_socket->Write(cb.data(), cb.length());
}

void NodeJSSocket::ProcessInputBuffer()
{
    if(m_firstTimeConnected) {
        m_firstTimeConnected = false;
        m_debugger->SetBreakpoints();
        m_debugger->Continue();
        m_inBuffer.Clear();

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
                    m_debugger->GotControl((json.namedObject("command").toString() != "backtrace"));
                } else {
                    m_debugger->SetCanInteract(false);
                }
            } else {

                // Notify the debugger that we got control
                if((json.namedObject("type").toString() == "event") &&
                   (json.namedObject("event").toString() == "break")) {
                    m_debugger->GotControl(true);
                } else {
                    m_debugger->SetCanInteract(false);
                }
            }
            // Check to see if we got more reponses in the in-buffer
            buffer = GetResponse();
        }
    }
}

bool NodeJSSocket::Connect(const wxString& ip, int port)
{
    wxIPV4address addr4;
    addr4.Hostname(ip);
    addr4.Service(port);

    // Connect async
    return m_socket->Connect(addr4, false) && (m_socket->LastError() != wxSOCKET_WOULDBLOCK);
}

wxString NodeJSSocket::LastError() const
{
    if(m_socket) {
        switch(m_socket->LastError()) {
        case wxSOCKET_NOERROR:
            return "No error";
        case wxSOCKET_INVOP:
            return "Invalid operation";
        case wxSOCKET_IOERR:
            return "Input/Output error";
        case wxSOCKET_INVADDR:
            return "Invalid address passed to wxSocket";
        case wxSOCKET_INVSOCK:
            return "Invalid socket (uninitialized)";
        case wxSOCKET_NOHOST:
            return "No corresponding host";
        case wxSOCKET_INVPORT:
            return "Invalid port";
        case wxSOCKET_WOULDBLOCK:
            return "The socket is non-blocking and the operation would block";
        case wxSOCKET_TIMEDOUT:
            return "The timeout for this operation expired";
        case wxSOCKET_MEMERR:
            return "Memory exhausted";
        case wxSOCKET_OPTERR:
            return "Operation error";
        default:
            break;
        }
    }
    return "";
}

wxSocketError NodeJSSocket::LastErrorCode() const
{
    if(m_socket) return m_socket->LastError();
    return wxSOCKET_NOERROR;
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
    wxCharBuffer cb = content.mb_str(wxConvUTF8).data();
    m_socket->Write(cb.data(), cb.length());

    // Keep the handler
    if(handler) {
        m_handlers.insert(std::make_pair(seq, handler));
    }
}

wxString NodeJSSocket::GetResponse()
{
    wxRegEx re("Content-Length:[ ]*([0-9]+)");
    if(re.Matches(m_inBuffer)) {
        wxString wholeLine = re.GetMatch(m_inBuffer, 0);
        long len;
        re.GetMatch(m_inBuffer, 1).ToCLong(&len);

        // Remove the "Content-Length: NN\r\n\r\n"
        m_inBuffer = m_inBuffer.Mid(wholeLine.length() + 4);
        wxString response = m_inBuffer.Mid(0, len);
        m_inBuffer = m_inBuffer.Mid(len);
        return response;
    }
    return "";
}
