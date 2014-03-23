#ifndef __LLDBDebugger__
#define __LLDBDebugger__

#include "plugin.h"
#include "cl_command_event.h"
#include "LLDBDebugger.h"

class LLDBDebuggerPlugin : public IPlugin
{
    LLDBDebugger m_debugger;
    bool m_isRunning;
    
public:
    LLDBDebuggerPlugin(IManager *manager);
    ~LLDBDebuggerPlugin();

private:
    void ShowTerminal(const wxString& title);
    
protected:
    // UI debugger events
    void OnDebugStart(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnDebugStop(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);

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
