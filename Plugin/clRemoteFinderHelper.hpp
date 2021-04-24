#ifndef CLREMOTEFINDERHELPER_HPP
#define CLREMOTEFINDERHELPER_HPP

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wx/arrstr.h"
#include "wx/event.h"
#include "wx/stopwatch.h"

class IProcess;
class WXDLLIMPEXP_SDK clRemoteFinderHelper : public wxEvtHandler
{
    wxString m_searchOutput;
    wxStopWatch m_stopWatch;
    IProcess* m_searchProcess = nullptr;

protected:
    void OnSearchOutput(clProcessEvent& event);
    void OnSearchTerminated(clProcessEvent& event);

public:
    clRemoteFinderHelper();
    ~clRemoteFinderHelper();

    /**
     * @brief execute a search
     */
    void Search(const wxArrayString& args, const wxString& accountname, const wxString& findString,
                const wxString& fileExtensions);

    /**
     * @brief are we running?
     */
    bool IsRunning() const
    {
        return m_searchProcess != nullptr;
    }
};

#endif // CLREMOTEFINDERHELPER_HPP
