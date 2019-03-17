#ifndef __LanguageServerPlugin__
#define __LanguageServerPlugin__

#include "plugin.h"
#include "cl_command_event.h"
#include "LanguageServerCluster.h"
#include "cl_command_event.h"
#include "CompileCommandsGenerator.h"

class LanguageServerPlugin : public IPlugin
{
    LanguageServerCluster::Ptr_t m_servers;
    IProcess* m_process = nullptr;
    
protected:
    void OnSettings(wxCommandEvent& e);

public:
    LanguageServerPlugin(IManager* manager);
    virtual ~LanguageServerPlugin();

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
};

#endif // LanguageServerPlugin
