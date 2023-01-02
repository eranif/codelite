#ifndef __Tail__
#define __Tail__

#include "plugin.h"
#include "clTabTogglerHelper.h"
#include "clEditorEditEventsHandler.h"
#include "TailData.h"

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
    void InitTailWindow(wxWindow* parent, bool isNotebook, const TailData& d, bool selectPage);
    
public:
    Tail(IManager* manager);
    ~Tail();
    
    /**
     * @brief detach the tail window from the output notebook
     */
    void DetachTailWindow(const TailData& d);
    
    /**
     * @brief dock the tail window back to the output notebook
     */
    void DockTailWindow(const TailData& d);
    
    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(clToolBarGeneric* toolbar);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();
    
    TailPanel* GetView() const { return m_view; }
};

#endif // Tail
