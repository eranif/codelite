#include "csCodeCompleteHandler.h"
#include "csFindInFilesCommandHandler.h"
#include "csListCommandHandler.h"
#include "csManager.h"
#include "csNetworkThread.h"
#include "csParseFolderHandler.h"
#include "file_logger.h"
#include "JSON.h"
#include "search_thread.h"
#include <algorithm>
#include <iostream>
#include <wx/app.h>

csManager::csManager()
    : m_startupCalled(false)
    , m_exitNow(false)
{
    m_handlers.Register("list", csCommandHandlerBase::Ptr_t(new csListCommandHandler(this)));
    m_handlers.Register("find", csCommandHandlerBase::Ptr_t(new csFindInFilesCommandHandler(this)));
    m_handlers.Register("parse", csCommandHandlerBase::Ptr_t(new csParseFolderHandler(this)));
    m_handlers.Register("code-complete", csCommandHandlerBase::Ptr_t(new csCodeCompleteHandler(this)));

    SearchThreadST::Get()->Start();
    SearchThreadST::Get()->SetNotifyWindow(this);
    m_config.Load();
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
    SearchThreadST::Get()->Stop();
}

bool csManager::Startup()
{
    if(m_exitNow) {
        CallAfter(&csManager::OnExit);
        return true;
    }

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

    JSON root(m_options);
    JSONItem options = root.toElement();
    handler->Process(options);
    return true;
}

void csManager::OnCommandProcessedCompleted(clCommandEvent& event) { wxExit(); }

void csManager::OnSearchThreadMatch(wxCommandEvent& event)
{
    SearchResultList* res = reinterpret_cast<SearchResultList*>(event.GetClientData());
    SearchResultList::iterator iter = res->begin();
    JSONItem arr = m_findInFilesMatches->toElement();
    while(iter != res->end()) {
        arr.arrayAppend(iter->ToJSON());
        ++iter;
    }
    wxDELETE(res);
}

void csManager::OnSearchThreadStarted(wxCommandEvent& event)
{
    clDEBUG() << "Search started";
    m_findInFilesMatches.reset(new JSON(cJSON_Array));
}

void csManager::OnSearchThreadCancelled(wxCommandEvent& event) { OnSearchThreadEneded(event); }

void csManager::OnSearchThreadEneded(wxCommandEvent& event)
{
    SearchSummary* summary = reinterpret_cast<SearchSummary*>(event.GetClientData());
    m_findInFilesMatches->toElement().arrayAppend(summary->ToJSON());
    wxDELETE(summary);
    wxString output = m_findInFilesMatches->toElement().format(GetConfig().IsPrettyJSON());
    std::cout << output << std::endl;
    clDEBUG1() << output;
    clDEBUG() << "Search completed";
    wxExit();
}

void csManager::LoadCommandFromINI()
{
    m_command = m_config.GetCommand();
    m_options = m_config.GetOptions();
}

void csManager::OnExit() { wxExit(); }
