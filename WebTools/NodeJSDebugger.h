#ifndef NODEJSDEBUGGER_H
#define NODEJSDEBUGGER_H

#include <wx/event.h>
#include "cl_command_event.h"
#include <wx/socket.h>
#include "NodeJSSocket.h"

class IProcess;
class NodeJSDebugger : public wxEvtHandler
{
    NodeJSSocket::Ptr_t m_socket;
    IProcess* m_node;
protected:
    // Debugger event handlers
    void OnDebugStart(clDebugEvent& event);
    void OnDebugContinue(clDebugEvent& event);
    void OnStopDebugger(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnToggleBreakpoint(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnVoid(clDebugEvent& event);
    void OnDebugStepIn(clDebugEvent& event);
    void OnDebugStepOut(clDebugEvent& event);
    void OnTooltip(clDebugEvent& event);
    void OnCanInteract(clDebugEvent& event);
    
    // Process event
    void OnNodeTerminated(clProcessEvent& event);
    void OnNodeOutput(clProcessEvent& event);
    
protected:
    bool IsConnected();
    
public:
    NodeJSDebugger();
    virtual ~NodeJSDebugger();
    
    /**
     * @brief nodejs debugger socket connected
     */
    void ConnectionEstablished();
    /**
     * @brief nodejs debugger socket disconnected
     */
    void ConnectionLost();
};

#endif // NODEJSDEBUGGER_H
