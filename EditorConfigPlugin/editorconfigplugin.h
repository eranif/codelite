#ifndef __EditorConfigPlugin__
#define __EditorConfigPlugin__

#include "plugin.h"
#include "cl_command_event.h"

class EditorConfigPlugin : public IPlugin
{
public:
    EditorConfigPlugin(IManager *manager);
    ~EditorConfigPlugin();

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
    
    void OnEditorConfigLoading(clEditorConfigEvent& event);
};

#endif //EditorConfigPlugin
