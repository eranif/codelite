#ifndef RUNINTERMINALHELPER_HPP
#define RUNINTERMINALHELPER_HPP

#include "asyncprocess.h"
#include "clModuleLogger.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "processreaderthread.h"

#include <vector>
#include <wx/event.h>

class RunInTerminalHelper : public wxEvtHandler
{
    IProcess* m_process = nullptr;
    clModuleLogger& LOG;
    int m_processId = wxNOT_FOUND;

protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnDebugEnded(clDebugEvent& event);

public:
    RunInTerminalHelper(clModuleLogger& log);
    virtual ~RunInTerminalHelper();

    /**
     * @brief run process, return its process id
     */
    int RunProcess(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env);
    void Terminate();
    int GetProcessId() const { return m_processId; }
};

#endif // RUNINTERMINALHELPER_HPP
