#ifndef __Docker__
#define __Docker__

#include "clDockerDriver.h"
#include "clTabTogglerHelper.h"
#include "plugin.h"

class DockerOutputPane;
class Docker : public IPlugin
{
    DockerOutputPane* m_outputView;
    clTabTogglerHelper::Ptr_t m_tabToggler;
    clDockerDriver::Ptr_t m_driver;

public:
    Docker(IManager* manager);
    ~Docker() override = default;

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    void CreatePluginMenu(wxMenu* pluginsMenu) override;

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    void UnPlug() override;

    DockerOutputPane* GetTerminal() { return m_outputView; }
    clDockerDriver::Ptr_t GetDriver() { return m_driver; }
};

#endif // Docker
