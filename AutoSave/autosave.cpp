#include "autosave.h"
#include <wx/xrc/xmlres.h>

static AutoSave* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new AutoSave(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("PC"));
    info.SetName(wxT("AutoSave"));
    info.SetDescription(_("Automatically save modified source files"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

AutoSave::AutoSave(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Automatically save modified source files");
    m_shortName = wxT("AutoSave");
}

AutoSave::~AutoSave() {}

clToolBar* AutoSave::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void AutoSave::CreatePluginMenu(wxMenu* pluginsMenu) {}

void AutoSave::UnPlug() {}
