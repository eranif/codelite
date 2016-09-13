#ifndef __AutoSave__
#define __AutoSave__

#include "plugin.h"

class AutoSave : public IPlugin
{
public:
    AutoSave(IManager *manager);
    ~AutoSave();

    //--------------------------------------------
    //Abstract methods
    //--------------------------------------------
    virtual clToolBar *CreateToolBar(wxWindow *parent);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu *pluginsMenu);
    
    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();
};

#endif //AutoSave
