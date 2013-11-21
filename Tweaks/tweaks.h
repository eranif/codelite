#ifndef __Tweaks__
#define __Tweaks__

#include "plugin.h"

class Tweaks : public IPlugin
{
public:
    Tweaks(IManager *manager);
    ~Tweaks();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void HookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
    virtual void UnPlug();
    
    // Event handlers
    void OnSettings(wxCommandEvent &e);
};

#endif //Tweaks
