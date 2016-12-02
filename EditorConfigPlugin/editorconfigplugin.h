#ifndef __EditorConfigPlugin__
#define __EditorConfigPlugin__

#include "plugin.h"
#include "cl_command_event.h"
#include "EditorConfigCache.h"
#include <wx/filename.h>
#include "clEditorConfig.h"

class EditorConfigPlugin : public IPlugin
{
    EditorConfigCache m_cache;

protected:
    bool DoGetEditorConfigForFile(const wxFileName& filename, clEditorConfigSection& section);

public:
    EditorConfigPlugin(IManager* manager);
    ~EditorConfigPlugin();

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

    void OnEditorConfigLoading(clEditorConfigEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
};

#endif // EditorConfigPlugin
