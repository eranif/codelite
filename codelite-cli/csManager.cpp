#include "csListCommandHandler.h"
#include "csManager.h"
#include "csNetworkReaderThread.h"
#include "csNetworkThread.h"
#include "file_logger.h"
#include <algorithm>
#include <wx/app.h>

csManager::csManager()
    : m_networkThread(nullptr)
    , m_startupCalled(false)
{
    m_handlers.Register("list", csCommandHandlerBase::Ptr_t(new csListCommandHandler(this)));
}

csManager::~csManager()
{
    // First unbind all the events
    if(m_startupCalled) {
        // Unbind(wxEVT_SOCKET_CONNECTION_READY, &csManager::OnNewConnection, this);
        // Unbind(wxEVT_SOCKET_SERVER_ERROR, &csManager::OnServerError, this);
        // Unbind(wxEVT_THREAD_GOING_DOWN, &csManager::OnWorkerThreadTerminated, this);
        Unbind(wxEVT_COMMAND_PROCESSED, &csManager::OnCommandProcessedCompleted, this);
    }

    // Now delete the threads
    wxDELETE(m_networkThread);

    // Delete all remaining threads
    std::for_each(m_threads.begin(), m_threads.end(), [&](csJoinableThread* thr) { wxDELETE(thr); });
    m_threads.clear();
}

bool csManager::Startup()
{
    m_config.Load();
    // Bind(wxEVT_SOCKET_CONNECTION_READY, &csManager::OnNewConnection, this);
    // Bind(wxEVT_SOCKET_SERVER_ERROR, &csManager::OnServerError, this);
    // Bind(wxEVT_THREAD_GOING_DOWN, &csManager::OnWorkerThreadTerminated, this);
    
    Bind(wxEVT_COMMAND_PROCESSED, &csManager::OnCommandProcessedCompleted, this);
    m_startupCalled = true;

    clDEBUG() << "Command:" << GetCommand();
    clDEBUG() << "Options:" << GetOptions();

    // Make sure we know how to handle this command
    csCommandHandlerBase::Ptr_t handler = m_handlers.FindHandler(m_command);
    if(handler == nullptr) {
        clERROR() << "Don't know how to handle command:" << m_command;
        return false;
    }
    
    handler->Process(m_options);

    // Create the network thread
    // m_networkThread = new csNetworkThread(this, m_config);
    // m_networkThread->Start();

    return true;
}

// void csManager::OnServerError(clCommandEvent& event)
// {
//     wxUnusedVar(event);
//     wxExit();
// }
//
// void csManager::OnNewConnection(clCommandEvent& event)
// {
//     // Create new thread to handle the connection
//     clSocketBase* socket = reinterpret_cast<clSocketBase*>(event.GetClientData());
//     csNetworkReaderThread* thread = new csNetworkReaderThread(this, socket);
//     thread->Start();
//     m_threads.insert(thread);
// }
//
// void csManager::OnWorkerThreadTerminated(clCommandEvent& event)
// {
//     // The thread terminated unexpectedly (maybe remote client closed connection, or some exception occured)
//     // remove the thread from our managed set and delete it
//     csJoinableThread* thread = reinterpret_cast<csJoinableThread*>(event.GetClientData());
//     // Remove this thread from our set
//     m_threads.erase(thread);
//     // Delete it
//     wxDELETE(thread);
// }

void csManager::OnCommandProcessedCompleted(clCommandEvent& event) { wxExit(); }
