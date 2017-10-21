#ifndef __SmartCompletion__
#define __SmartCompletion__

#include "plugin.h"
#include "cl_command_event.h"
#include <unordered_map>
#include "wxStringHash.h"
#include "entry.h"
#include "SmartCompletionsConfig.h"

class SmartCompletion : public IPlugin
{
    std::unordered_map<wxString, int>* m_pWeight;
    typedef std::pair<TagEntryPtr, int> QueueElement_t;
    SmartCompletionsConfig m_config;

protected:
    void OnCodeCompletionSelectionMade(clCodeCompletionEvent& event);
    void OnCodeCompletionShowing(clCodeCompletionEvent& event);
    void OnSettings(wxCommandEvent& e);

public:
    SmartCompletion(IManager* manager);
    ~SmartCompletion();

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

#endif // SmartCompletion
