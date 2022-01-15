#include "LSPNetworkSocketClient.h"

#include "cl_exception.h"
#include "dirsaver.h"
#include "file_logger.h"
#include "sftp_settings.h"
#include "ssh_account_info.h"

LSPNetworkSocketClient::LSPNetworkSocketClient() {}

LSPNetworkSocketClient::~LSPNetworkSocketClient() { Close(); }

void LSPNetworkSocketClient::Close()
{
    if(m_lspServer) {
        m_lspServer->Detach();
    }
    wxDELETE(m_lspServer);
    m_socket.reset(nullptr);
    m_pid = wxNOT_FOUND;
}

bool LSPNetworkSocketClient::IsConnected() const { return m_socket != nullptr; }
void LSPNetworkSocketClient::Open(const LSPStartupInfo& info)
{
    m_startupInfo = info;
    // Start the process
    if(m_startupInfo.GetFlags() & LSPStartupInfo::kAutoStart) {
        wxString cmd = BuildCommand(m_startupInfo.GetLspServerCommand());
        // if remote is enabled for this LSP, start it on the remote machine
        if(m_startupInfo.GetFlags() & LSPStartupInfo::kRemoteLSP) {
            throw clException("Executing LSP over SSH is enabled only for STDIO based LSPs");
        } else {
            m_lspServer = ::CreateAsyncProcess(this, cmd, IProcessCreateDefault, m_startupInfo.GetWorkingDirectory());
        }
        if(!m_lspServer) {
            throw clException(wxString() << "Failed to execute process: " << cmd);
        }

        m_lspServer->Detach(); // we dont want events
        m_pid = m_lspServer->GetPid();
    } else {
        m_pid = wxNOT_FOUND;
    }

    // Now that the process is up, connect to the server
    m_socket.reset(new clAsyncSocket(m_startupInfo.GetConnectioString(), kAsyncSocketBuffer | kAsyncSocketClient));
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
