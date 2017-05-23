#ifndef __CodeliteVim__
#define __CodeliteVim__

#include "plugin.h"

class VimManager;

class CodeliteVim : public IPlugin
{
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

private:
    VimManager* m_vimM;
};

#endif // CodeliteVim
