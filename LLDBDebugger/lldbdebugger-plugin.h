#ifndef __LLDBDebugger__
#define __LLDBDebugger__

#include "plugin.h"
#include "cl_command_event.h"
#include <wx/stc/stc.h>

#include "LLDBProtocol/LLDBConnector.h"
#include "LLDBProtocol/LLDBEvent.h"

class LLDBLocalsView;
class LLDBBreakpointsPane;
class ConsoleFrame;
class LLDBCallStackPane;
class LLDBDebuggerPlugin : public IPlugin
{
    LLDBConnector m_connector;
    wxString m_defaultPerspective;
    
    /// ------------------------------------
    /// UI elements
    /// ------------------------------------
    LLDBCallStackPane*      m_callstack;
    LLDBBreakpointsPane*    m_breakpointsView;
    LLDBLocalsView*         m_localsView;
    int                     m_terminalPID;
    
public:
    LLDBDebuggerPlugin(IManager *manager);
    ~LLDBDebuggerPlugin();
    
    LLDBConnector* GetLLDB() {
        return &m_connector;
    }
    
    IManager* GetManager() {
        return m_mgr;
    }
    
private:
    void TerminateTerminal();
    
    void ClearDebuggerMarker();
    void SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno);
    void LoadLLDBPerspective();
    void SaveLLDBPerspective();
    void ShowLLDBPane(const wxString &paneName, bool show = true);
    void RestoreDefaultPerspective();
    void InitializeUI();
    void DestroyUI();
    void DoCleanup();
    
protected:
    // Other codelite events
    void OnWorkspaceLoaded(wxCommandEvent &event);
    void OnWorkspaceClosed(wxCommandEvent &event);
    void OnSettings(wxCommandEvent &event);
    
    // UI debugger events
    void OnIsDebugger(clDebugEvent& event);
    void OnDebugStart(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnDebugStop(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnDebugCanInteract(clDebugEvent& event);
    void OnDebugStepIn(clDebugEvent& event);
    void OnDebugStepOut(clDebugEvent& event);
    void OnToggleBreakpoint(clDebugEvent& event);
    void OnToggleInerrupt(clDebugEvent& event);
    void OnBuildStarting(clBuildEvent& event);

    // LLDB events
    void OnLLDBCrashed(LLDBEvent& event);
    void OnLLDBStarted(LLDBEvent& event);
    void OnLLDBExited(LLDBEvent& event);
    void OnLLDBStopped(LLDBEvent &event);
    void OnLLDBStoppedOnEntry(LLDBEvent &event);
    void OnLLDBRunning(LLDBEvent &event);
    void OnLLDBDeletedAllBreakpoints(LLDBEvent &event);
    void OnLLDBBreakpointsUpdated(LLDBEvent &event);
    
public:    
    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();
};

#endif //LLDBDebuggerPlugin
