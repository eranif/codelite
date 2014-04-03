#ifndef __LLDBDebugger__
#define __LLDBDebugger__

#include "plugin.h"
#include "cl_command_event.h"
#include "LLDBDebugger.h"
#include <wx/stc/stc.h>

class LLDBBreakpointsPane;
class ConsoleFrame;
class LLDBCallStackPane;
class LLDBDebuggerPlugin : public IPlugin
{
    LLDBDebugger m_debugger;
    bool   m_isRunning;
    wxString m_defaultPerspective;
    
    /// ------------------------------------
    /// UI elements
    /// ------------------------------------
    LLDBCallStackPane*   m_callstack;
    ConsoleFrame *       m_console;
    LLDBBreakpointsPane* m_breakpointsView;
    
public:
    LLDBDebuggerPlugin(IManager *manager);
    ~LLDBDebuggerPlugin();

private:
    void ShowTerminal(const wxString& title);
    
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

    // LLDB events
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
