#ifndef RUNINTERMINALHELPER_HPP
#define RUNINTERMINALHELPER_HPP

#include "AsyncProcess/asyncprocess.h"
#include "AsyncProcess/processreaderthread.h"
#include "cl_command_event.h"

#include <vector>
#include <wx/event.h>

class RunInTerminalHelper : public wxEvtHandler
{
    IProcess* m_process = nullptr;
    int m_processId = wxNOT_FOUND;

protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnDebugEnded(clDebugEvent& event);

public:
    RunInTerminalHelper();
    virtual ~RunInTerminalHelper();

    /**
     * @brief run process, return its process id
     */
    int RunProcess(const std::vector<wxString>& command, const wxString& wd, const clEnvList_t& env);
    void Terminate();
    int GetProcessId() const { return m_processId; }
};

#endif // RUNINTERMINALHELPER_HPP
