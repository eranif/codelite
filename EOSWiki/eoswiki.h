#ifndef __EOSWiki__
#define __EOSWiki__

#include "plugin.h"
#include "EOSProjectData.h"
#include "macros.h"

class EOSWiki : public IPlugin
{
public:
    void OnNewProject(wxCommandEvent& event);

protected:
    void CreateProject(const EOSProjectData& data);
    void CreateSampleFile(ProjectPtr proj, const EOSProjectData& d);
    void CreateCMakeListsFile(ProjectPtr proj, const EOSProjectData& d);
    
    void ExtractResources();
    wxString ReadResource(const wxString& filename) const;
    void ReplacePlaceHolders(wxString& text, const wxStringMap_t& m);

public:
    EOSWiki(IManager* manager);
    ~EOSWiki() override = default;

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

#endif // EOSWiki
