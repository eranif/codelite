#include "LSPNetworkSocket.h"
#include "JSON.h"
#include <sstream>

LSPNetworkSocket::LSPNetworkSocket() {}

LSPNetworkSocket::~LSPNetworkSocket() { Close(); }

void LSPNetworkSocket::Close() { m_socket.reset(nullptr); }

void LSPNetworkSocket::Open(const LSPNetwork::StartupInfo& info)
{
    m_startupInfo = std::move(info);
    m_socket.reset(new clSocketClientAsync());
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &LSPNetworkSocket::OnSocketConnected, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &LSPNetworkSocket::OnSocketConnectionLost, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &LSPNetworkSocket::OnSocketConnectionError, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_ERROR, &LSPNetworkSocket::OnSocketError, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_INPUT, &LSPNetworkSocket::OnSocketData, this);
    m_socket->Connect("tcp://127.0.0.1:12898");
}

void LSPNetworkSocket::Send(const std::string& data)
{
    if(m_socket) { m_socket->Send(data); }
}

void LSPNetworkSocket::OnSocketConnected(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_CONNECTED);
    AddPendingEvent(evt);

    // Build custom command that tells the helper to start the LSP server on its end
    JSON json(cJSON_Object);
    JSONItem item = json.toElement();
    item.addProperty("command", m_startupInfo.GetCommand());
    item.addProperty("method", wxString("execute"));
    item.addProperty("rootUri", m_startupInfo.GetRootUri());
    item.addProperty("languages", m_startupInfo.GetLanguages());

    char* str = item.FormatRawString(true);
    std::string jsonStr = str;
    free(str);

    std::stringstream ss;
    ss << "Content-Length: " << jsonStr.length() << "\r\n\r\n";
    ss << jsonStr;

    // Send the data
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
