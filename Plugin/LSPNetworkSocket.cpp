#include "LSPNetworkSocket.h"
#include "JSON.h"
#include <sstream>
#include "file_logger.h"
#include "clcommandlineparser.h"

thread_local std::vector<int> availablePorts;
thread_local std::vector<int> returnedPorts;

static void ReturnPort(int& port)
{
    returnedPorts.push_back(port);
    port = wxNOT_FOUND;
}

static int GetNextPort()
{
    if(!availablePorts.empty()) {
        int port = availablePorts.back();
        availablePorts.pop_back();
        return port;
    } else {
        // no more ports, swap between the available and returnedPorts
        returnedPorts.swap(availablePorts);
        return GetNextPort();
    }
    return wxNOT_FOUND;
}

LSPNetworkSocket::LSPNetworkSocket()
{
    if(availablePorts.empty()) {
        for(int i = 12850; i < 12899; ++i) {
            availablePorts.push_back(i);
        }
    }
    Bind(wxEVT_END_PROCESS, &LSPNetworkSocket::OnProcessTerminated, this);
}

LSPNetworkSocket::~LSPNetworkSocket()
{
    Unbind(wxEVT_END_PROCESS, &LSPNetworkSocket::OnProcessTerminated, this);
    Close();
}

void LSPNetworkSocket::Close()
{
    if(m_lspServer) {
        long pid = m_lspServer->GetPid();
        m_lspServer->Detach();
        wxKillError rc;
        ::wxKill(pid, wxSIGTERM, &rc, wxKILL_CHILDREN);
        m_lspServer = nullptr;
    }
    ReturnPort(m_port);
    m_socket.reset(nullptr);
}

void LSPNetworkSocket::Open(const LSPStartupInfo& siInfo)
{
    m_startupInfo = siInfo;

    // Start the LSP server first
    Close();
    // Close resets our port number
    m_port = GetNextPort();
    if(m_port == wxNOT_FOUND) {
        clCommandEvent evt(wxEVT_LSP_NET_ERROR);
        evt.SetString(wxString() << "Cant run LSP: can't allocate port");
        AddPendingEvent(evt);
        return;
    }

    wxString helperScript;
    helperScript << m_startupInfo.GetHelperCommand() << " " << m_port;
    clDEBUG() << "Starting helper script:" << helperScript;

    // First, start the helper script
    m_lspServer = new wxProcess(this);

    size_t flags = wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | wxEXEC_HIDE_CONSOLE;

    if(m_startupInfo.GetFlags() & LSPStartupInfo::kShowConsole) { flags &= ~wxEXEC_HIDE_CONSOLE; }

    if(::wxExecute(helperScript, flags, m_lspServer) <= 0) {
        clCommandEvent evt(wxEVT_LSP_NET_ERROR);
        evt.SetString(wxString() << "Failed to execute: " << m_startupInfo.GetLspServerCommand());
        AddPendingEvent(evt);
        return;
    }

    // Start the network client
    m_socket.reset(new clSocketClientAsync());
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &LSPNetworkSocket::OnSocketConnected, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &LSPNetworkSocket::OnSocketConnectionLost, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &LSPNetworkSocket::OnSocketConnectionError, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_ERROR, &LSPNetworkSocket::OnSocketError, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_INPUT, &LSPNetworkSocket::OnSocketData, this);
    m_socket->ConnectNonBlocking(wxString() << "tcp://127.0.0.1:" << m_port);
}

void LSPNetworkSocket::Send(const std::string& data)
{
    if(m_socket) {
        clDEBUG() << "LSP socket:\n" << data;
        m_socket->Send(data);
    } else {
        clDEBUG() << "LSP socket: no socket !?";
    }
}

void LSPNetworkSocket::OnSocketConnected(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_CONNECTED);
    AddPendingEvent(evt);

    // Build custom command that tells the helper to start the LSP server on its end
    JSON json(cJSON_Object);
    JSONItem item = json.toElement();

    clCommandLineParser parser(m_startupInfo.GetLspServerCommand());
    wxArrayString argv = parser.ToArray();
    if(argv.IsEmpty()) { return; }
    item.addProperty("command", argv.Item(0));
    argv.RemoveAt(0, 1);
    item.addProperty("args", argv);
    item.addProperty("wd", m_startupInfo.GetLspServerCommandWorkingDirectory().IsEmpty()
                               ? ::wxGetCwd()
                               : m_startupInfo.GetLspServerCommandWorkingDirectory());
    item.addProperty("method", wxString("execute"));

    char* str = item.FormatRawString(true);
    std::string jsonStr = str;
    free(str);

    std::stringstream ss;
    ss << "Content-Length: " << jsonStr.length() << "\r\n\r\n";
    ss << jsonStr;

    // Send the data
    clDEBUG() << "Sending request\n" << ss.str();
    Send(ss.str());
}

void LSPNetworkSocket::OnSocketConnectionLost(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocket::OnSocketConnectionError(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocket::OnSocketError(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocket::OnSocketData(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_DATA_READY);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

bool LSPNetworkSocket::IsConnected() const { return m_socket != nullptr; }

void LSPNetworkSocket::OnProcessTerminated(wxProcessEvent& event)
{
    wxDELETE(m_lspServer);
    clDEBUG() << "LSPNetworkSocket: helper program terminated:" << m_startupInfo.GetLspServerCommand();
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    AddPendingEvent(evt);
}
