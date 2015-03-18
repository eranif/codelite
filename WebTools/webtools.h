#ifndef __WebTools__
#define __WebTools__

#include "plugin.h"

class WebTools : public IPlugin
{
public:
    WebTools(IManager *manager);
    ~WebTools();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    virtual void UnPlug();
};

#endif //WebTools
