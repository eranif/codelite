#ifndef NODEJSCLIDEBUGGER_H
#define NODEJSCLIDEBUGGER_H

#include "NodeJSCLIDebuggerOutputParser.h"
#include "NodeJSDebuggerBase.h"
#include "NodeJSDebuggerBreakpoint.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <wx/event.h>
#include <wx/timer.h>
#include <wx/process.h>

class NodeJSCLIDebugger;
typedef std::function<void(const wxString&)> CommandHandlerFunc_t;
class NodeJSCliCommandHandler
{
public:
    wxString m_commandText;
    CommandHandlerFunc_t action = nullptr;
    long m_commandID = wxNOT_FOUND;

    NodeJSCliCommandHandler(const wxString& command, long commandID, const CommandHandlerFunc_t& func)
        : m_commandText(command)
        , action(func)
        , m_commandID(commandID)
    {
    }
};

class NodeProcess : public wxProcess
{
    NodeJSCLIDebugger* m_debugger = nullptr;
    wxTimer* m_timer = nullptr;
    long m_pid = wxNOT_FOUND;

public:
    NodeProcess(NodeJSCLIDebugger* debugger)
        : m_debugger(debugger)
    {
        Redirect();
    }
    
    virtual ~NodeProcess();
    void OnTimer(wxTimerEvent& event);
    void OnTerminate(int pid, int status);
    void Start(const wxString& command, const wxString& workingDirectory);
    void Terminate();
};

class NodeJSCLIDebugger : public NodeJSDebuggerBase
{
    IProcess* m_process = nullptr;
    NodeJSCLIDebuggerOutputParser m_outputParser;
    bool m_canInteract = false;
    std::vector<NodeJSCliCommandHandler> m_commands;
    std::unordered_map<long, NodeJSCliCommandHandler> m_waitingReplyCommands;
    wxString m_workingDirectory;
    wxString m_bufferedOutput;

public:
    NodeJSCLIDebugger();
    virtual ~NodeJSCLIDebugger();
    bool IsRunning() const;
    bool IsCanInteract() const;
    void StartDebugger(const wxString& command, const wxString& command_args, const wxString& workingDirectory);
    void ListBreakpoints();
    void Callstack();
    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    long GetCommandId() const;

protected:
    void OnDebugStart(clDebugEvent& event);
    void OnStopDebugger(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnDebugContinue(clDebugEvent& event);
    void OnToggleBreakpoint(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

    // Helpers
    void DoCleanup();
    void ProcessQueue();
    void PushCommand(const NodeJSCliCommandHandler& commandHandler);
    void SetBreakpoint(const wxFileName& file, int lineNumber, bool useVoidHandler = false);
    void DeleteBreakpoint(const NodeJSBreakpoint& bp);
    wxString GetBpRelativeFilePath(const NodeJSBreakpoint& bp) const;
    void ApplyAllBerakpoints();
};

#endif // NODEJSCLIDEBUGGER_H
