#ifndef __CodeliteVim__
#define __CodeliteVim__

#include "plugin.h"
#include "VimSettings.h"

class VimManager;

class CodeliteVim : public IPlugin
{
private:
    VimManager* m_vimM;
    VimSettings m_settings;

public:
    CodeliteVim(IManager* manager);
    ~CodeliteVim() override;

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    void CreatePluginMenu(wxMenu* pluginsMenu) override;

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed
         * (e.g. unbind events, destroy allocated windows)
     */
    void UnPlug() override;

    // int FilterEvent(wxEvent &event) override;
protected:
    void onVimSetting(wxCommandEvent& event);
};

#endif // CodeliteVim
