#include "csNetworkThread.h"
#include "csConfig.h"
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
    // FileLoggerNameRegistrar logName("Network");
    // clSocketServer server;
    // clDEBUG() << "Network thread is starting...";
    //
    // try {
    //    server.Start(m_config.GetConnectionString());
    //} catch(clSocketException& e) {
    //    clERROR() << "Network thread failed to start on '" << m_config.GetConnectionString() << "'." << e.what();
    //    clCommandEvent errorEvent(wxEVT_SOCKET_SERVER_ERROR);
    //    m_manager->AddPendingEvent(errorEvent);
    //    return NULL;
    //}
    //
    // clDEBUG() << "Waiting for new connection...";
    // while(true) {
    //    if(TestDestroy()) { break; }
    //    clSocketBasePtr_t conn = server.WaitForNewConnectionRaw(1);
    //    if(conn) {
    //        clDEBUG() << "Received new connection";
    //        // Call reset with a dummy Deleter
    //        // so we can get the raw pointer without destroying it
    //        clCommandEvent newConnEvent(wxEVT_SOCKET_CONNECTION_READY);
    //        newConnEvent.SetClientData(static_cast<void*>(conn));
    //        m_manager->AddPendingEvent(newConnEvent);
    //    }
    //}
    // clDEBUG() << "Network thread is going down";
    return NULL;
}
