#ifndef __PHPRefactoring__
#define __PHPRefactoring__

#include "plugin.h"

class PHPRefactoring : public IPlugin
{
public:
    PHPRefactoring(IManager *manager);
    ~PHPRefactoring();

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

#endif //PHPRefactoring
