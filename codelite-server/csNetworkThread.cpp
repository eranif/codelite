#include "csConfig.h"
#include "csNetworkThread.h"
#include <SocketAPI/clSocketServer.h>
#include <file_logger.h>

wxDEFINE_EVENT(wxEVT_SOCKET_CONNECTION_READY, clCommandEvent);
wxDEFINE_EVENT(wxEVT_SOCKET_SERVER_ERROR, clCommandEvent);

csNetworkThread::csNetworkThread(wxEvtHandler* manager, const csConfig& config)
    : csJoinableThread(manager)
    , m_config(config)
{
}

csNetworkThread::~csNetworkThread() {}

void* csNetworkThread::Entry()
{
    clSocketServer server;
    clDEBUG() << "[csNetworkThread] starting...";

    try {
        server.Start(m_config.GetConnectionString());
    } catch(clSocketException& e) {
        clERROR() << "[csNetworkThread] csNetworkThread: failed to start server on '" << m_config.GetConnectionString()
                  << "'." << e.what();
        clCommandEvent errorEvent(wxEVT_SOCKET_SERVER_ERROR);
        m_manager->AddPendingEvent(errorEvent);
        return NULL;
    }

    clDEBUG() << "[csNetworkThread] waiting for new connection...";
    while(true) {
        if(TestDestroy()) {
            clDEBUG() << "[csNetworkThread] going down";
            break;
        }
        clSocketBase::Ptr_t conn = server.WaitForNewConnection(1);
        if(conn) {
            clDEBUG() << "[csNetworkThread] Received new connection";
            clSocketBasePtr_t pConn = conn.get();

            // Call reset with a dummy Deleter
            // so we can get the raw pointer without destroying it
            conn.reset(nullptr, [&](void* p) {});
            clCommandEvent newConnEvent(wxEVT_SOCKET_CONNECTION_READY);
            newConnEvent.SetClientData(static_cast<void*>(pConn));
            m_manager->AddPendingEvent(newConnEvent);
        }
    }
    clDEBUG() << "[csNetworkThread] bye";
    return NULL;
}
