#ifndef CLREMOTEFINDERHELPER_HPP
#define CLREMOTEFINDERHELPER_HPP

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wx/arrstr.h"
#include "wx/event.h"
#include "wx/stopwatch.h"

class WXDLLIMPEXP_SDK clRemoteFinderHelper
{
    wxStopWatch m_stopWatch;
    size_t m_matches_found = 0;
    wxString m_findOutput;
    size_t m_fif_matches_count = 0;

protected:
    wxWindow* GetSearchTab();
    void ProcessFindOutput(bool is_completed);

public:
    clRemoteFinderHelper();
    ~clRemoteFinderHelper();

    /**
     * @brief convert find-in-files format into CodeLite's output tab view format
     * @param event
     */
    void ProcessSearchOutput(const clFindInFilesEvent::Match::vec_t& matches, bool is_completed);

    /**
     * @brief execute a search
     */
    void Search(const wxString& root_dir, const wxString& findString, const wxString& fileExtensions, bool whole_word,
                bool icase);

    /**
     * @brief send wxEVT_SEARCH_THREAD_SEARCHCANCELED event
     */
    void NotifySearchCancelled();
};

#endif // CLREMOTEFINDERHELPER_HPP
