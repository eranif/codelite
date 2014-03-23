#ifndef __LLDBDebugger__
#define __LLDBDebugger__

#include "plugin.h"

class LLDBDebugger : public IPlugin
{
public:
    LLDBDebugger(IManager *manager);
    ~LLDBDebugger();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();
};

#endif //LLDBDebugger
