#ifndef __Docker__
#define __Docker__

#include "clTabTogglerHelper.h"
#include "plugin.h"

class wxTerminal;
class Docker : public IPlugin
{
    wxTerminal* m_terminal;
    clTabTogglerHelper::Ptr_t m_tabToggler;

public:
    Docker(IManager* manager);
    virtual ~Docker();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBar* toolbar);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();
    
    wxTerminal* GetTerminal() { return m_terminal; }
    
};

#endif // Docker
