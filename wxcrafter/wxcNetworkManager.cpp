#include "wxcNetworkManager.h"
#include "wxc_project_metadata.h"
#include <event_notifier.h>

wxcNetworkManager::wxcNetworkManager()
{
    EventNotifier::Get()->Bind(wxEVT_NETWORK_COMMAND_CONN_ESTASBLISHED, &wxcNetworkManager::OnNetConnEstablished, this);
}

wxcNetworkManager::~wxcNetworkManager() {}

void wxcNetworkManager::OnNetConnEstablished(wxcNetworkEvent& event)
{
    m_socket.reset(new clSocketBase(event.GetInt()));
}

void wxcNetworkManager::SendReply(wxcNetworkReply& reply)
{
    try {
        if(m_socket) {
            // Make sure we always return the name of the wxcp file loaded
            reply.SetWxcpFile(wxcProjectMetadata::Get().GetProjectFileName().GetFullPath());
            m_socket->WriteMessage(reply.ToJSON().format());
        }
    } catch(clSocketException& e) {
        wxUnusedVar(e);
    }
}
