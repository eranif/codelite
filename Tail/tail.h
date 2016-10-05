#ifndef __Tail__
#define __Tail__

#include "plugin.h"
#include "clTabTogglerHelper.h"

class TailPanel;
class Tail : public IPlugin
{
    TailPanel* m_view;
    clTabTogglerHelper::Ptr_t m_tabHelper;

protected:
    void OnInitDone(wxCommandEvent& event);

public:
    Tail(IManager* manager);
    ~Tail();

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

#endif // Tail
