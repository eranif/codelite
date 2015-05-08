#ifndef __HelpPlugin__
#define __HelpPlugin__

#include "plugin.h"
#include "cl_command_event.h"

class HelpPlugin : public IPlugin
{
protected:
    void OnEditorContextMenu(clContextMenuEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnHelpSettings(wxCommandEvent& event);
    
    wxString DoBuildQueryString();
    
public:
    HelpPlugin(IManager* manager);
    ~HelpPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();
};

#endif // HelpPlugin
