#ifndef __phpmd__
#define __phpmd__

#include "plugin.h"

class phpmd : public IPlugin
{
public:
    phpmd(IManager *manager);
    ~phpmd();

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

#endif //phpmd
