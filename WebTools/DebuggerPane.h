#ifndef NODEJSCLIDEBUGGERPANE_H
#define NODEJSCLIDEBUGGERPANE_H

#include "WebToolsBase.h"
#include "clDebugCallFramesEvent.h"
#include "cl_command_event.h"

class wxTerminal;
class DebuggerPane : public NodeJSCliDebuggerPaneBase
{
    wxTerminal* m_terminal = nullptr;
    wxTerminal* m_node_console = nullptr;

protected:
    void OnUpdateBacktrace(clDebugCallFramesEvent& event);
    void OnDebuggerStopped(clDebugEvent& event);
    void OnMarkLine(clDebugEvent& event);
    void OnInteract(clDebugEvent& event);
    void OnUpdateBreakpoints(clDebugEvent& event);
    void OnRunTerminalCommand(clCommandEvent& event);
    void OnEval(clCommandEvent& event);
    void OnConsoleOutput(clDebugEvent& event);

public:
    DebuggerPane(wxWindow* parent);
    virtual ~DebuggerPane();
};
#endif // NODEJSCLIDEBUGGERPANE_H
