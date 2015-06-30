#include "NodeJSSocket.h"
#include "NodeJSDebugger.h"

NodeJSSocket::NodeJSSocket(NodeJSDebugger* debugger)
    : m_socket(NULL)
    , m_sequence(0)
    , m_debugger(debugger)
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

void NodeJSSocket::ProcessInputBuffer() {}

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
