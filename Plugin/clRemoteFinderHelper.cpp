#include "Notebook.h"
#include "asyncprocess.h"
#include "clRemoteFinderHelper.hpp"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "processreaderthread.h"
#include "search_thread.h"
#include <wx/tokenzr.h>

clRemoteFinderHelper::clRemoteFinderHelper()
{
}

clRemoteFinderHelper::~clRemoteFinderHelper()
{
}

void clRemoteFinderHelper::ProcessSearchOutput(const clFindInFilesEvent& event)
{
    auto search_tab = GetSearchTab();
    if(!search_tab) {
        clWARNING() << "clRemoteFinderHelper: search tab is hidden" << endl;
        return;
    }

    long elapsed_time = m_stopWatch.Time();

    // process the output
    SearchResultList* resList = nullptr;
    if(!event.GetMatches().empty()) {
        resList = new SearchResultList;
    }

    for(const auto& file_match : event.GetMatches()) {
        const wxString& filename = file_match.file;
        for(const auto& location : file_match.locations) {
            SearchResult res;
            res.SetColumn(location.column_start);
            res.SetLen(location.column_end - location.column_start);
            res.SetFileName(filename);
            res.SetLineNumber(location.line);
            res.SetPattern(location.pattern);
            resList->push_back(res);
        }
    }

    if(resList && !resList->empty()) {
        wxCommandEvent matchs_event(wxEVT_SEARCH_THREAD_MATCHFOUND);
        matchs_event.SetClientData(resList);
        search_tab->GetEventHandler()->AddPendingEvent(matchs_event);
    } else if(resList) {
        wxDELETE(resList);
    }

    SearchSummary* summary = new SearchSummary;
    summary->SetElapsedTime(elapsed_time);
    summary->SetNumMatchesFound(resList ? resList->size() : 0);
    wxCommandEvent end_event(wxEVT_SEARCH_THREAD_SEARCHEND);
    end_event.SetClientData(summary);
    search_tab->GetEventHandler()->AddPendingEvent(end_event);
}

void clRemoteFinderHelper::Search(const wxString& root_dir, const wxString& findString, const wxString& fileExtensions)
{
    // start ssh process
    if(!m_codeliteRemote || !m_codeliteRemote->IsRunning()) {
        return;
    }
    m_stopWatch.Start();

    if(!GetSearchTab()) {
        clWARNING() << "clRemoteFinderHelper: search ignored, search tab is hidden" << endl;
        return;
    }

    m_codeliteRemote->Search(root_dir, fileExtensions, findString, 0);

    SearchData sd;
    sd.SetEncoding("UTF-8");
    sd.SetFindString(findString);
    sd.SetExtensions(fileExtensions);
    wxCommandEvent start_event(wxEVT_SEARCH_THREAD_SEARCHSTARTED);
    start_event.SetClientData(new SearchData(sd));
    GetSearchTab()->GetEventHandler()->AddPendingEvent(start_event);
}

wxWindow* clRemoteFinderHelper::GetSearchTab()
{
    auto book = clGetManager()->GetOutputPaneNotebook();
    for(size_t i = 0; i < book->GetPageCount(); ++i) {
        if(book->GetPageText(i) == _("Search")) {
            return book->GetPage(i);
        }
    }
    return nullptr;
}

void clRemoteFinderHelper::SetCodeLiteRemote(clCodeLiteRemoteProcess* clr)
{
    m_codeliteRemote = clr;
}
