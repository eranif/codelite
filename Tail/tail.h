#ifndef __Tail__
#define __Tail__

#include "plugin.h"
#include "clTabTogglerHelper.h"
#include "clEditorEditEventsHandler.h"

class TailPanel;
class TailFrame;
class Tail : public IPlugin
{
    friend class TailFrame;
    TailPanel* m_view;
    clTabTogglerHelper::Ptr_t m_tabHelper;
    clEditEventsHandler::Ptr_t m_editEventsHandler;

protected:
    void OnInitDone(wxCommandEvent& event);
    void DoDetachWindow();
    void InitTailWindow(wxWindow* parent, bool isNotebook);
    
public:
    Tail(IManager* manager);
    ~Tail();
    
    /**
     * @brief detach the tail window from the output notebook
     */
    void DetachTailWindow();
    
    /**
     * @brief dock the tail window back to the output notebook
     */
    void DockTailWindow();
    
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
