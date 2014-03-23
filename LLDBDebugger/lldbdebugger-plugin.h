#ifndef __LLDBDebugger__
#define __LLDBDebugger__

#include "plugin.h"

class LLDBDebuggerPlugin : public IPlugin
{
public:
    LLDBDebuggerPlugin(IManager *manager);
    ~LLDBDebuggerPlugin();

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
