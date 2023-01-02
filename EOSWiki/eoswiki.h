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
    virtual ~EOSWiki();

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
};

#endif // EOSWiki
