#ifndef NODEJSCLIDEBUGGER_H
#define NODEJSCLIDEBUGGER_H

#include "NodeJSCLIDebuggerOutputParser.h"
#include "NodeJSDebuggerBase.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <wx/event.h>

typedef std::function<void(const wxString&)> CommandHandlerFunc_t;
class NodeJSCliCommandHandler
{
public:
    wxString m_commandText;
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
    NodeJSCLIDebuggerOutputParser m_outputParser;
    bool m_canInteract = false;
    std::vector<NodeJSCliCommandHandler> m_commands;
    std::unordered_map<long, NodeJSCliCommandHandler> m_waitingReplyCommands;
    wxString m_workingDirectory;

public:
    virtual void Callstack();
    NodeJSCLIDebugger();
    virtual ~NodeJSCLIDebugger();
    bool IsRunning() const;
    bool IsCanInteract() const;
    void StartDebugger(const wxString& command, const wxString& workingDirectory);

    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

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
