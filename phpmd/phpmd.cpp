#include "phpmd.h"
#include <wx/xrc/xmlres.h>

static phpmd* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new phpmd(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Anders Jenbo"));
    info.SetName(wxT("phpmd"));
    info.SetDescription(_("Run phpmd on load and save"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

phpmd::phpmd(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Run phpmd on load and save");
    m_shortName = wxT("phpmd");
}

phpmd::~phpmd()
{
}

clToolBar* phpmd::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void phpmd::CreatePluginMenu(wxMenu* pluginsMenu)
{
}

void phpmd::UnPlug()
{
}
