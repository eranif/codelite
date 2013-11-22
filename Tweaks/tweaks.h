#ifndef __Tweaks__
#define __Tweaks__

#include "plugin.h"
#include "cl_command_event.h"
#include "tweaks_settings.h"

class Tweaks : public IPlugin
{
    TweaksSettings m_settings;
protected:
    IEditor* FindEditorByPage( wxWindow* page );
    
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
    void OnColourTab(clColourEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);
};

#endif //Tweaks
