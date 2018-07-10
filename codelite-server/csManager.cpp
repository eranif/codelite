#include "csManager.h"
#include "csNetworkReaderThread.h"
#include "csNetworkThread.h"
#include <wx/app.h>
#include <algorithm>

csManager::csManager()
    : m_networkThread(nullptr)
{
    m_config.Load();
    Bind(wxEVT_SOCKET_CONNECTION_READY, &csManager::OnNewConnection, this);
    Bind(wxEVT_SOCKET_SERVER_ERROR, &csManager::OnServerError, this);
    Bind(wxEVT_THREAD_GOING_DOWN, &csManager::OnWorkerThreadTerminated, this);
}

csManager::~csManager()
{
    // First unbind all the events
    Unbind(wxEVT_SOCKET_CONNECTION_READY, &csManager::OnNewConnection, this);
    Unbind(wxEVT_SOCKET_SERVER_ERROR, &csManager::OnServerError, this);
    Unbind(wxEVT_THREAD_GOING_DOWN, &csManager::OnWorkerThreadTerminated, this);

    // Now delete the threads
    wxDELETE(m_networkThread);
    
    // Delete all remaining threads
    std::for_each(m_threads.begin(), m_threads.end(), [&](csJoinableThread* thr) { wxDELETE(thr); });
    m_threads.clear();
}

void csManager::Startup()
{
    // Create the network thread
    m_networkThread = new csNetworkThread(this, m_config);
    m_networkThread->Start();
}

void csManager::OnServerError(clCommandEvent& event)
{
    wxUnusedVar(event);
    wxExit();
}

void csManager::OnNewConnection(clCommandEvent& event)
{
    // Create new thread to handle the connection
    clSocketBase* socket = reinterpret_cast<clSocketBase*>(event.GetClientData());
    csNetworkReaderThread* thread = new csNetworkReaderThread(this, socket);
    thread->Start();
    m_threads.insert(thread);
}

void csManager::OnWorkerThreadTerminated(clCommandEvent& event)
{
    // The thread terminated unexpectedly (maybe remote client closed connection, or some exception occured)
    // remove the thread from our managed set and delete it
    csJoinableThread* thread = reinterpret_cast<csJoinableThread*>(event.GetClientData());
    // Remove this thread from our set
    m_threads.erase(thread);
    // Delete it
    wxDELETE(thread);
}
