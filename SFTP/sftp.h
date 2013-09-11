#ifndef __SFTP__
#define __SFTP__

#include "plugin.h"

class SFTP : public IPlugin
{
public:
    SFTP(IManager *manager);
    ~SFTP();
    
protected:
    void OnSettings(wxCommandEvent &e);
    
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

#endif //SFTP
