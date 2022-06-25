#ifndef NODEJSCLIDEBUGGER_H
#define NODEJSCLIDEBUGGER_H

#include "NodeJSDebuggerBreakpoint.h"
#include "NodeJSDebuggerBreakpointManager.h"
#include "NodeJSDevToolsProtocol.h"
#include "SocketAPI/clWebSocketClient.h"
#include "asyncprocess.h"
#include "cl_command_event.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <wx/event.h>

class NodeDebugger : public wxEvtHandler
{
    IProcess* m_process = nullptr;
    bool m_canInteract = false;
    wxString m_workingDirectory;
    clWebSocketClient m_socket;
    NodeJSBptManager m_bptManager;
    wxString m_activeFrame;

public:
    static wxString NODE_CLI_DEBUGGER_NAME;

public:
    typedef wxSharedPtr<NodeDebugger> Ptr_t;

public:
    NodeDebugger();
    virtual ~NodeDebugger();
    bool IsRunning() const;
    bool IsCanInteract() const;
    void StartDebugger(const wxString& command, const wxString& command_args, const wxString& workingDirectory);
    void StopDebugger();
    void ListBreakpoints();
    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    void SendToDebuggee(const wxString& command);
    void Eval(const wxString& command, const wxString& frameId);
    void GetObjectProperties(const wxString& objectId, wxEventType eventType);
    NodeJSBptManager* GetBreakpointsMgr() { return &m_bptManager; }

    void SetDebuggerMarker(IEditor* editor, int lineno);
    void SetDebuggerMarker(const wxString& path, int lineno);
    void ClearDebuggerMarker();

    void SetBreakpoint(const wxFileName& file, int lineNumber);
    void DeleteBreakpointByID(const wxString& bpid);
    void DeleteBreakpoint(const NodeJSBreakpoint& bp);
    void DeleteAllBreakpoints();

    void SetActiveFrame(const wxString& activeFrame) { this->m_activeFrame = activeFrame; }
    const wxString& GetActiveFrame() const { return m_activeFrame; }

protected:
    void OnDebugStart(clDebugEvent& event);
    void OnStopDebugger(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnDebugStepIn(clDebugEvent& event);
    void OnDebugStepOut(clDebugEvent& event);
    void OnDebugContinue(clDebugEvent& event);
    void OnToggleBreakpoint(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnWebSocketConnected(clCommandEvent& event);
    void OnWebSocketError(clCommandEvent& event);
    void OnWebSocketOnMessage(clCommandEvent& event);
    void OnWebSocketDisconnected(clCommandEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnInteract(clDebugEvent& event);
    void OnTooltip(clDebugEvent& event);
    // Helpers
    void DoCleanup();
    wxString GetBpRelativeFilePath(const NodeJSBreakpoint& bp) const;
    void ApplyAllBerakpoints();
    void DoHighlightLine(const wxString& filename, int lineNo);
};

#endif // NODEJSCLIDEBUGGER_H
