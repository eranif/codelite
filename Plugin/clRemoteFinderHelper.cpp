#include "clRemoteFinderHelper.hpp"

#include "Notebook.h"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "clRemoteHost.hpp"
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
    clDEBUG() << "Processing:" << event.GetMatches().size() << "match entries" << endl;
    clDEBUG() << "is_completed?" << is_completed << endl;

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

void clRemoteFinderHelper::ProcessFindOutput(bool is_completed)
{
    bool last_line_incomplete = !m_findOutput.EndsWith("\n");
    wxArrayString files = ::wxStringTokenize(m_findOutput, "\n", wxTOKEN_STRTOK);
    if(!files.empty()) {
        m_findOutput.clear();
        if(last_line_incomplete && !is_completed) {
            // the last line is incomplete, keep it for next iteration
            m_findOutput = files.back();
            files.pop_back();
        }

        clFindInFilesEvent::Match::vec_t matches;
        matches.reserve(files.size());
        for(wxString& entry : files) {
            wxString mod_line;
            StringUtils::StripTerminalColouring(entry, mod_line);
            /// mod_line => /home/eran/devl/codelite/Plugin/LSP/LSPNetworkRemoteSTDIO.cpp:39:    pattern
            clFindInFilesEvent::Match match;
            match.file = mod_line.BeforeFirst(':');
            match.file.Trim().Trim(false);

            /// mod_line => 39:    pattern
            mod_line = mod_line.AfterFirst(':');
            clFindInFilesEvent::Location location;
            wxString line = mod_line.BeforeFirst(':');
            line.Trim().Trim(false);
            unsigned long line_number = 0;
            if(line.ToULong(&line_number)) {
                location.line = line_number;
            } else {
                clWARNING() << "failed to parse line number:" << line << endl;
                continue;
            }

            location.pattern = mod_line.AfterFirst(':'); // the remainder is the matched pattern
            match.locations.push_back(location);
            matches.push_back(match);
            m_fif_matches_count++;
        }

        if(!matches.empty()) {
            clFindInFilesEvent event(wxEVT_CODELITE_REMOTE_FIND_RESULTS);
            event.SetMatches(matches);
            ProcessSearchOutput(event, false);
        }
    }
    if(is_completed) {
        clFindInFilesEvent event_done(wxEVT_CODELITE_REMOTE_FIND_RESULTS_DONE);
        ProcessSearchOutput(event_done, true);
    }
}

void clRemoteFinderHelper::Search(const wxString& root_dir, const wxString& findString, const wxString& fileExtensions,
                                  bool whole_word, bool icase)
{
    if(!GetSearchTab()) {
        clWARNING() << "clRemoteFinderHelper: search ignored, search tab is hidden" << endl;
        return;
    }

    auto extarr = ::wxStringTokenize(fileExtensions, ",; |", wxTOKEN_STRTOK);
    std::vector<wxString> grep_command = { "grep", "-r", "-n" };
    if(icase) {
        grep_command.push_back("-i");
    }

    if(whole_word) {
        grep_command.push_back("-w");
    }

    for(const auto& ext : extarr) {
        grep_command.push_back("--include=" + ext);
    }
    grep_command.push_back(findString);
    grep_command.push_back(root_dir);

    auto callback = [this](const std::string& str, clRemoteCommandStatus status) {
        m_findOutput << wxString::FromUTF8(str);
        ProcessFindOutput(status == clRemoteCommandStatus::DONE || status == clRemoteCommandStatus::DONE_WITH_ERROR);
    };
    m_findOutput.clear();
    m_fif_matches_count = 0;
    m_matches_found = 0;
    m_stopWatch.Start();
    clDEBUG() << "Running remote command:" << grep_command << endl;
    clRemoteHost::Instance()->run_command_with_callback(grep_command, root_dir, {}, callback);

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
