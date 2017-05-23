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
    ~CodeliteVim();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual clToolBar* CreateToolBar(wxWindow* parent);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed
         * (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();

    // virtual int FilterEvent(wxEvent &event);
protected:
    void onVimSetting(wxCommandEvent& event);
};

#endif // CodeliteVim
