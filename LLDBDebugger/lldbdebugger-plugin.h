#ifndef __LLDBDebugger__
#define __LLDBDebugger__

#include "plugin.h"
#include "cl_command_event.h"
#include "LLDBDebugger.h"
#include <wx/stc/stc.h>

class LLDBDebuggerPlugin : public IPlugin
{
    LLDBDebugger m_debugger;
    bool m_isRunning;
    bool m_canInteract;
    
public:
    LLDBDebuggerPlugin(IManager *manager);
    ~LLDBDebuggerPlugin();

private:
    void ShowTerminal(const wxString& title);
    
    // Set/clear debugger line marker
    void ClearDebuggerMarker();
    void SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno);

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

    // LLDB events
    void OnLLDBStarted(LLDBEvent& event);
    void OnLLDBExited(LLDBEvent& event);
    void OnLLDBStopped(LLDBEvent &event);
    void OnLLDBStoppedOnEntry(LLDBEvent &event);

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
