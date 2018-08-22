#include "clDockerWorkspace.h"
#include "clWorkspaceManager.h"
#include "docker.h"
#include <wx/xrc/xmlres.h>

static Docker* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new Docker(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("Docker"));
    info.SetDescription(_("Docker for CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

Docker::Docker(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Docker for CodeLite");
    m_shortName = wxT("Docker");

    clWorkspaceManager::Get().RegisterWorkspace(new clDockerWorkspace(false));
    clDockerWorkspace::Get(); // Make sure that the workspace instance is up and all events are hooked
}

Docker::~Docker() {}

void Docker::CreateToolBar(clToolBar* toolbar)
{
    // You can add items to the main toolbar here
    wxUnusedVar(toolbar);
}

void Docker::CreatePluginMenu(wxMenu* pluginsMenu) {}

void Docker::UnPlug() {}
