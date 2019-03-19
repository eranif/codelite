#include "LSPNetworkSocketClient.h"
#include "file_logger.h"

LSPNetworkSocketClient::LSPNetworkSocketClient() {}

LSPNetworkSocketClient::~LSPNetworkSocketClient() {}

void LSPNetworkSocketClient::Close() {}
bool LSPNetworkSocketClient::IsConnected() const { return m_socket != nullptr; }
void LSPNetworkSocketClient::Open(const LSPStartupInfo& info) { m_startupInfo = info; }
void LSPNetworkSocketClient::Send(const std::string& data)
{
    if(m_socket) {
        clDEBUG() << "LSP socket:\n" << data;
        m_socket->Send(data);
    } else {
        clDEBUG() << "LSP socket: no socket !?";
    }
}

void LSPNetworkSocketClient::OnSocketConnected(clCommandEvent& event) {}

void LSPNetworkSocketClient::OnSocketConnectionLost(clCommandEvent& event) {}

void LSPNetworkSocketClient::OnSocketConnectionError(clCommandEvent& event) {}

void LSPNetworkSocketClient::OnSocketError(clCommandEvent& event) {}

void LSPNetworkSocketClient::OnSocketData(clCommandEvent& event) {}

void LSPNetworkSocketClient::OnProcessTerminated(wxProcessEvent& event) {}
