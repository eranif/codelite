#include "clRemoteFinderHelper.hpp"

#include "Notebook.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "processreaderthread.h"
#include "search_thread.h"

#include <wx/tokenzr.h>

clRemoteFinderHelper::clRemoteFinderHelper() {}

clRemoteFinderHelper::~clRemoteFinderHelper() {}

void clRemoteFinderHelper::ProcessSearchOutput(const clFindInFilesEvent& event, bool is_completed)
{
    auto search_tab = GetSearchTab();
    if(!search_tab) {
        clWARNING() << "clRemoteFinderHelper: search tab is hidden" << endl;
        return;
    }

    // process the output
    SearchResultList* resList = nullptr;
    const auto& matches = event.GetMatches();

    if(!matches.empty()) {
        resList = new SearchResultList;
        for(const auto& file_match : matches) {
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
    }
    m_matches_found += (resList ? resList->size() : 0);

    if(resList && !resList->empty()) {
        wxCommandEvent matchs_event(wxEVT_SEARCH_THREAD_MATCHFOUND);
        matchs_event.SetClientData(resList);
        search_tab->GetEventHandler()->AddPendingEvent(matchs_event);
    } else if(resList) {
        wxDELETE(resList);
    }

    if(is_completed) {
        // stop the clock
        long elapsed_time = m_stopWatch.Time();
        SearchSummary* summary = new SearchSummary;
        summary->SetElapsedTime(elapsed_time);

        // set the total number of matches found
        summary->SetNumMatchesFound(m_matches_found);
        wxCommandEvent end_event(wxEVT_SEARCH_THREAD_SEARCHEND);
        summary->SetNumFileScanned(event.GetInt());
        end_event.SetClientData(summary);
        search_tab->GetEventHandler()->AddPendingEvent(end_event);

        // prepare for next search
        m_matches_found = 0;
    }
}

void clRemoteFinderHelper::Search(const wxString& root_dir, const wxString& findString, const wxString& fileExtensions,
                                  bool whole_word, bool icase)
{
    // start ssh process
    if(!m_codeliteRemote || !m_codeliteRemote->IsRunning()) {
        return;
    }
    m_stopWatch.Start();
    m_matches_found = 0;

    if(!GetSearchTab()) {
        clWARNING() << "clRemoteFinderHelper: search ignored, search tab is hidden" << endl;
        return;
    }

    m_codeliteRemote->Search(root_dir, fileExtensions, findString, whole_word, icase);

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

void clRemoteFinderHelper::SetCodeLiteRemote(clCodeLiteRemoteProcess* clr) { m_codeliteRemote = clr; }

void clRemoteFinderHelper::NotifySearchCancelled()
{
    CHECK_PTR_RET(GetSearchTab());
    // Notify that the search is cancelled
    wxCommandEvent event_cacnelled{ wxEVT_SEARCH_THREAD_SEARCHCANCELED };
    GetSearchTab()->GetEventHandler()->AddPendingEvent(event_cacnelled);

    // the UI is also expecting the wxEVT_SEARCH_THREAD_SEARCHEND event
    wxCommandEvent end_event{ wxEVT_SEARCH_THREAD_SEARCHEND };
    end_event.SetClientData(nullptr);
    GetSearchTab()->GetEventHandler()->AddPendingEvent(end_event);
}
