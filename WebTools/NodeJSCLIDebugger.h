#ifndef NODEJSCLIDEBUGGER_H
#define NODEJSCLIDEBUGGER_H

#include "NodeJSDebuggerBase.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include <functional>
#include <vector>
#include <wx/event.h>

typedef std::function<void(const wxString&)> CommandHandlerFunc_t;
class NodeJSCliCommandHandler
{
public:
    wxString m_commandText;
    bool in_progress = false;
    CommandHandlerFunc_t action = nullptr;

    NodeJSCliCommandHandler(const wxString& command, const CommandHandlerFunc_t& func)
        : m_commandText(command)
        , action(func)
    {
    }
};

class NodeJSCLIDebugger : public NodeJSDebuggerBase
{
    IProcess* m_process = nullptr;
    wxString m_debuggerOutput;
    bool m_canInteract = false;
    std::vector<NodeJSCliCommandHandler> m_commands;
    wxString m_workingDirectory;

public:
    virtual void Callstack();
    NodeJSCLIDebugger();
    virtual ~NodeJSCLIDebugger();
    bool IsRunning() const;
    bool IsCanInteract() const;
    void StartDebugger(const wxString& command, const wxString& workingDirectory);

protected:
    void OnDebugStart(clDebugEvent& event);
    void OnDebugContinue(clDebugEvent& event);
    void OnStopDebugger(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

    // Helpers
    void DoCleanup();
    void ProcessQueue();
    void PushCommand(const NodeJSCliCommandHandler& commandHandler);
};

#endif // NODEJSCLIDEBUGGER_H
