#include "Notebook.h"
#include "asyncprocess.h"
#include "clRemoteFinderHelper.hpp"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
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
    
    auto search_tab = GetSearchTab();
    if(!search_tab) {
        clWARNING() << "clRemoteFinderHelper: search tab is hidden" << endl;
        return;
    }

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

void clRemoteFinderHelper::Search(const wxArrayString& args, const wxString& accountName, const wxString& findString,
                                  const wxString& fileExtensions)
{
    // start ssh process
    m_searchOutput.clear();
    m_stopWatch.Start();

    if(!GetSearchTab()) {
        clWARNING() << "clRemoteFinderHelper: search ignored, search tab is hidden" << endl;
        return;
    }

    m_searchProcess = ::CreateAsyncProcess(this, args, IProcessCreateDefault | IProcessWrapInShell | IProcessCreateSSH,
                                           wxEmptyString, nullptr, accountName);

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
