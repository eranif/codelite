#ifndef NODEJSCLIDEBUGGERPANE_H
#define NODEJSCLIDEBUGGERPANE_H

#include "WebToolsBase.h"
#include "clDebugCallFramesEvent.h"
#include "clDebugRemoteObjectEvent.h"
#include "cl_command_event.h"

class CallFrame;
class NodeDebuggerTooltip;
class wxTerminal;
class NodeDebuggerPane : public NodeJSCliDebuggerPaneBase
{
    wxTerminal* m_terminal = nullptr;
    wxTerminal* m_node_console = nullptr;
    NodeDebuggerTooltip* m_debuggerTooltip = nullptr;
    std::unordered_map<wxString, wxTreeItemId> m_localsPendingItems;
    nSerializableObject::Vec_t m_frames;

protected:
    virtual void OnStackContextMenu(wxDataViewEvent& event);
    virtual void OnStackEntryActivated(wxDataViewEvent& event);
    virtual void OnLocalExpanding(wxTreeEvent& event);
    void DoDestroyTip();
    void DoPrintStartupMessages();
    void DoUpdateLocalsView(CallFrame* callFrame);
    wxString GetLocalObjectItem(const wxTreeItemId& item) const;
    CallFrame* GetFrameById(const wxString& frameId) const;
    void SelectTab(const wxString& label);

protected:
    void OnUpdateBacktrace(clDebugCallFramesEvent& event);
    void OnDebuggerStopped(clDebugEvent& event);
    void OnMarkLine(clDebugEvent& event);
    void OnInteract(clDebugEvent& event);
    void OnUpdateBreakpoints(clDebugEvent& event);
    void OnRunTerminalCommand(clCommandEvent& event);
    void OnEval(clCommandEvent& event);
    void OnConsoleOutput(clDebugEvent& event);
    void OnDebugSessionStarted(clDebugEvent& event);
    void OnLocalProperties(clDebugEvent& event);
    void OnEvalResult(clDebugRemoteObjectEvent& event);
    void OnCreateObject(clDebugRemoteObjectEvent& event);
    void OnDestroyTip(clCommandEvent& event);
    void OnClearAllBreakpoints(wxCommandEvent& event);
    void OnClearAllBreakpointsUI(wxUpdateUIEvent& event);
    void OnDeleteBreakpoint(wxCommandEvent& event);
    void OnDeleteBreakpointUI(wxUpdateUIEvent& event);

public:
    NodeDebuggerPane(wxWindow* parent);
    virtual ~NodeDebuggerPane();
};
#endif // NODEJSCLIDEBUGGERPANE_H
