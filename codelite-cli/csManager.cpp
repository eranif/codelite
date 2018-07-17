#include "csFindInFilesCommandHandler.h"
#include "csListCommandHandler.h"
#include "csManager.h"
#include "csNetworkReaderThread.h"
#include "csNetworkThread.h"
#include "file_logger.h"
#include "search_thread.h"
#include <algorithm>
#include <wx/app.h>

csManager::csManager()
    : m_networkThread(nullptr)
    , m_startupCalled(false)
{
    m_handlers.Register("list", csCommandHandlerBase::Ptr_t(new csListCommandHandler(this)));
    m_handlers.Register("find", csCommandHandlerBase::Ptr_t(new csFindInFilesCommandHandler(this)));

    SearchThreadST::Get()->Start();
    SearchThreadST::Get()->SetNotifyWindow(this);
}

csManager::~csManager()
{
    // First unbind all the events
    if(m_startupCalled) {
        Unbind(wxEVT_COMMAND_PROCESSED, &csManager::OnCommandProcessedCompleted, this);
        // Search thread events
        Unbind(wxEVT_SEARCH_THREAD_MATCHFOUND, &csManager::OnSearchThreadMatch, this);
        Unbind(wxEVT_SEARCH_THREAD_SEARCHSTARTED, &csManager::OnSearchThreadStarted, this);
        Unbind(wxEVT_SEARCH_THREAD_SEARCHCANCELED, &csManager::OnSearchThreadCancelled, this);
        Unbind(wxEVT_SEARCH_THREAD_SEARCHEND, &csManager::OnSearchThreadEneded, this);
    }

    // Now delete the threads
    wxDELETE(m_networkThread);

    // Delete all remaining threads
    std::for_each(m_threads.begin(), m_threads.end(), [&](csJoinableThread* thr) { wxDELETE(thr); });
    m_threads.clear();

    SearchThreadST::Get()->Stop();
}

bool csManager::Startup()
{
    m_config.Load();
    Bind(wxEVT_COMMAND_PROCESSED, &csManager::OnCommandProcessedCompleted, this);
    // Search thread events
    Bind(wxEVT_SEARCH_THREAD_MATCHFOUND, &csManager::OnSearchThreadMatch, this);
    Bind(wxEVT_SEARCH_THREAD_SEARCHSTARTED, &csManager::OnSearchThreadStarted, this);
    Bind(wxEVT_SEARCH_THREAD_SEARCHCANCELED, &csManager::OnSearchThreadCancelled, this);
    Bind(wxEVT_SEARCH_THREAD_SEARCHEND, &csManager::OnSearchThreadEneded, this);

    m_startupCalled = true;

    clDEBUG() << "Command:" << GetCommand();
    clDEBUG() << "Options:" << GetOptions();

    // Make sure we know how to handle this command
    csCommandHandlerBase::Ptr_t handler = m_handlers.FindHandler(m_command);
    if(handler == nullptr) {
        clERROR() << "Don't know how to handle command:" << m_command;
        return false;
    }

    JSONRoot root(m_options);
    JSONElement options = root.toElement();
    handler->Process(options);
    return true;
}

void csManager::OnCommandProcessedCompleted(clCommandEvent& event) { wxExit(); }

void csManager::OnSearchThreadMatch(wxCommandEvent& event)
{
}

void csManager::OnSearchThreadStarted(wxCommandEvent& event)
{
}

void csManager::OnSearchThreadCancelled(wxCommandEvent& event)
{
}

void csManager::OnSearchThreadEneded(wxCommandEvent& event)
{
}
