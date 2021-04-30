#ifndef CLREMOTEFINDERHELPER_HPP
#define CLREMOTEFINDERHELPER_HPP

#include "clCodeLiteRemoteProcess.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wx/arrstr.h"
#include "wx/event.h"
#include "wx/stopwatch.h"

class WXDLLIMPEXP_SDK clRemoteFinderHelper
{
    wxStopWatch m_stopWatch;
    clCodeLiteRemoteProcess* m_codeliteRemote = nullptr;
    size_t m_matches_found = 0;

protected:
    wxWindow* GetSearchTab();

public:
    clRemoteFinderHelper();
    ~clRemoteFinderHelper();

    void SetCodeLiteRemote(clCodeLiteRemoteProcess* clr);

    /**
     * @brief convert find-in-files format into CodeLite's output tab view format
     * @param event
     */
    void ProcessSearchOutput(const clFindInFilesEvent& event, bool is_completed);

    /**
     * @brief execute a search
     */
    void Search(const wxString& root_dir, const wxString& findString, const wxString& fileExtensions, bool whole_word,
                bool icase);
};

#endif // CLREMOTEFINDERHELPER_HPP
