#include "asyncprocess.h"
#include "clRemoteFinderHelper.hpp"
#include "event_notifier.h"
#include "file_logger.h"
#include "processreaderthread.h"
#include "search_thread.h"
#include <wx/tokenzr.h>

clRemoteFinderHelper::clRemoteFinderHelper()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clRemoteFinderHelper::OnSearchTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clRemoteFinderHelper::OnSearchOutput, this);
}

clRemoteFinderHelper::~clRemoteFinderHelper()
{
    wxDELETE(m_searchProcess);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clRemoteFinderHelper::OnSearchTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clRemoteFinderHelper::OnSearchOutput, this);
}

void clRemoteFinderHelper::OnSearchOutput(clProcessEvent& event)
{
    m_searchOutput.reserve(m_searchOutput.length() + event.GetOutput().length());
    m_searchOutput << event.GetOutput();
}

void clRemoteFinderHelper::OnSearchTerminated(clProcessEvent& event)
{
    m_searchOutput << event.GetOutput();
    wxDELETE(m_searchProcess);
    clDEBUG() << "remote search completed:" << m_searchOutput << endl;

    long elapsed_time = m_stopWatch.Time();

    // process the output
    wxArrayString lines = ::wxStringTokenize(m_searchOutput, "\r\n", wxTOKEN_DEFAULT);
    SearchResultList* resList = nullptr;
    if(!lines.empty()) {
        resList = new SearchResultList;
    }

    for(auto& line : lines) {
        line.Trim();
        // find the file name
        wxString filepath = line.BeforeFirst(':');
        line = line.AfterFirst(':');

        // find the line number
        wxString line_number_str = line.BeforeFirst(':');

        long nLineNumber = wxNOT_FOUND;
        if(!line_number_str.ToCLong(&nLineNumber)) {
            continue;
        }
        --nLineNumber; // zero based line numbers
        // the remainder is the pattern
        line = line.AfterFirst(':');

        SearchResult res;
        res.SetColumn(0);
        res.SetLen(0);

        res.SetFileName(filepath);
        res.SetLineNumber(nLineNumber);
        res.SetPattern(line);
        resList->push_back(res);
    }

    if(resList && !resList->empty()) {
        wxCommandEvent matchs_event(wxEVT_SEARCH_THREAD_MATCHFOUND);
        matchs_event.SetClientData(resList);
        EventNotifier::Get()->AddPendingEvent(matchs_event);
    } else if(resList) {
        wxDELETE(resList);
    }

    SearchSummary* summary = new SearchSummary;
    summary->SetElapsedTime(elapsed_time);
    summary->SetNumMatchesFound(resList ? resList->size() : 0);
    wxCommandEvent end_event(wxEVT_SEARCH_THREAD_SEARCHEND);
    end_event.SetClientData(summary);
    EventNotifier::Get()->AddPendingEvent(end_event);
}

void clRemoteFinderHelper::Search(const wxArrayString& args, const wxString& accountName, const wxString& findString,
                                  const wxString& fileExtensions)
{
    // start ssh process
    m_searchOutput.clear();
    m_stopWatch.Start();

    m_searchProcess = ::CreateAsyncProcess(this, args, IProcessCreateDefault | IProcessWrapInShell | IProcessCreateSSH,
                                           wxEmptyString, nullptr, accountName);

    SearchData sd;
    sd.SetEncoding("UTF-8");
    sd.SetFindString(findString);
    sd.SetExtensions(fileExtensions);
    wxCommandEvent start_event(wxEVT_SEARCH_THREAD_SEARCHSTARTED);
    start_event.SetClientData(new SearchData(sd));
    EventNotifier::Get()->AddPendingEvent(start_event);
}
