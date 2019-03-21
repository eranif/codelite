#include "LSPNetworkSocketClient.h"
#include "file_logger.h"
#include "dirsaver.h"

LSPNetworkSocketClient::LSPNetworkSocketClient() {}

LSPNetworkSocketClient::~LSPNetworkSocketClient() {}

void LSPNetworkSocketClient::Close()
{
    if(m_pid != wxNOT_FOUND) {
        wxKillError rc;
        ::wxKill(m_pid, wxSIGTERM, &rc, wxKILL_CHILDREN);
    }
    
    // a detached process, no need to self delete it
    m_lspServer = nullptr;
    m_pid = wxNOT_FOUND;
    m_socket.reset(nullptr);
}

bool LSPNetworkSocketClient::IsConnected() const { return m_socket != nullptr; }
void LSPNetworkSocketClient::Open(const LSPStartupInfo& info)
{
    m_startupInfo = info;
    // Start the process
    m_lspServer = new wxProcess(this);
    size_t flags = wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | wxEXEC_HIDE_CONSOLE;
    if(m_startupInfo.GetFlags() & LSPStartupInfo::kShowConsole) { flags &= ~wxEXEC_HIDE_CONSOLE; }

    DirSaver ds;

    if(!m_startupInfo.GetLspServerCommandWorkingDirectory().IsEmpty()) {
        ::wxSetWorkingDirectory(m_startupInfo.GetLspServerCommandWorkingDirectory());
    }

    // Start the process
    if(::wxExecute(m_startupInfo.GetLspServerCommand(), flags, m_lspServer) <= 0) {
        clCommandEvent evt(wxEVT_LSP_NET_ERROR);
        evt.SetString(wxString() << "Failed to execute: " << m_startupInfo.GetLspServerCommand());
        AddPendingEvent(evt);
        return;
    }
    m_pid = m_lspServer->GetPid();
    m_lspServer->Detach();

    // Now that the process is up, connect to the server
    m_socket.reset(new clAsyncSocket(m_startupInfo.GetHelperCommand(), kAsyncSocketBuffer | kAsyncSocketClient));
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &LSPNetworkSocketClient::OnSocketConnected, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &LSPNetworkSocketClient::OnSocketConnectionLost, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &LSPNetworkSocketClient::OnSocketConnectionError, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_ERROR, &LSPNetworkSocketClient::OnSocketError, this);
    m_socket->Bind(wxEVT_ASYNC_SOCKET_INPUT, &LSPNetworkSocketClient::OnSocketData, this);
    m_socket->Start();
}

void LSPNetworkSocketClient::Send(const std::string& data)
{
    if(m_socket) {
        clDEBUG() << "LSP socket:\n" << data;
        m_socket->Send(data);
    } else {
        clDEBUG() << "LSP socket: no socket !?";
    }
}

void LSPNetworkSocketClient::OnSocketConnected(clCommandEvent& event)
{
    // Now that everything is up, send the "initialize" request
    clCommandEvent evtReady(wxEVT_LSP_NET_CONNECTED);
    AddPendingEvent(evtReady);
}

void LSPNetworkSocketClient::OnSocketConnectionLost(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocketClient::OnSocketConnectionError(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocketClient::OnSocketError(clCommandEvent& event)
{
    clCommandEvent evt(wxEVT_LSP_NET_ERROR);
    evt.SetString(event.GetString());
    AddPendingEvent(evt);
}

void LSPNetworkSocketClient::OnSocketData(clCommandEvent& event)
{
    const wxString& dataRead = event.GetString();
    clCommandEvent evt(wxEVT_LSP_NET_DATA_READY);
    evt.SetString(dataRead);
    AddPendingEvent(evt);
}
