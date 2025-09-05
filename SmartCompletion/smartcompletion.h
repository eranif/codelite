#ifndef __SmartCompletion__
#define __SmartCompletion__

#include "SmartCompletionsConfig.h"
#include "cl_command_event.h"
#include "database/entry.h"
#include "plugin.h"
#include "wxStringHash.h"
#include <unordered_map>

class SmartCompletion : public IPlugin
{
    typedef std::unordered_map<wxString, int> WeightTable_t;
    WeightTable_t* m_pCCWeight;
    WeightTable_t* m_pGTAWeight;
    typedef std::pair<TagEntryPtr, int> QueueElement_t;
    SmartCompletionsConfig m_config;

protected:
    void OnCodeCompletionSelectionMade(clCodeCompletionEvent& event);
    void OnCodeCompletionShowing(clCodeCompletionEvent& event);
    void OnGotoAnythingSort(clGotoEvent& event);
    void OnGotoAnythingSelectionMade(clGotoEvent& event);
    void OnSettings(wxCommandEvent& e);

public:
    SmartCompletion(IManager* manager);
    ~SmartCompletion() override = default;

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
};

#endif // SmartCompletion
