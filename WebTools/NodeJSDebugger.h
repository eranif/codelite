#ifndef NODEJSDEBUGGER_H
#define NODEJSDEBUGGER_H

#include <wx/event.h>
#include "cl_command_event.h"
#include <wx/socket.h>
#include "NodeJSSocket.h"
#include "NodeJSDebuggerBreakpointManager.h"

class IProcess;
class NodeJSDebugger : public wxEvtHandler
{
    NodeJSSocket::Ptr_t m_socket;
    IProcess* m_node;
    NodeJSBptManager m_bptManager;
    bool m_canInteract;

public:
    typedef SmartPtr<NodeJSDebugger> Ptr_t;

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

    // CodeLite events
    void OnWorkspaceOpened(wxCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);

    // Process event
    void OnNodeTerminated(clProcessEvent& event);
    void OnNodeOutput(clProcessEvent& event);
    void OnHighlightLine(clDebugEvent& event);
    void OnSelectFrame(clDebugEvent& event);

protected:
    bool IsConnected();

public:
    NodeJSDebugger();
    virtual ~NodeJSDebugger();

    void SetDebuggerMarker(IEditor* editor, int lineno);
    void ClearDebuggerMarker();

    void SetCanInteract(bool canInteract);
    bool IsCanInteract() const { return m_canInteract; }

    //--------------------------------------------------
    // API
    //--------------------------------------------------

    /**
     * @brief delete breakpoint from NodeJS. This function does not updat the breakpoint manager
     * nor it does not update the UI
     */
    void DeleteBreakpoint(const NodeJSBreakpoint& bp);

    /**
     * @brief set a breakpoint in nodejs debugger
     */
    void SetBreakpoint(const NodeJSBreakpoint& bp);

    /**
     * @brief set all breakpoints
     */
    void SetBreakpoints();

    /**
     * @brief continue execution
     */
    void Continue();

    /**
     * @brief request for callstack
     */
    void Callstack();
    
    /**
     * @brief select new frame (usually by d-clicking a frame entry in the callstack view)
     */
    void SelectFrame(int frameId);
    
    //--------------------------------------------------
    // API END
    //--------------------------------------------------

    /**
     * @brief return the breakpoints manager
     */
    NodeJSBptManager* GetBreakpointsMgr() { return &m_bptManager; }

    /**
     * @brief nodejs debugger socket connected
     */
    void ConnectionEstablished();
    /**
     * @brief nodejs debugger socket disconnected
     */
    void ConnectionLost();

    /**
     * @brief this function is called by the various handlers whenever the CodeLite
     * gets the control from NodeJS
     */
    void GotControl(bool requestBacktrace);
};

#endif // NODEJSDEBUGGER_H
