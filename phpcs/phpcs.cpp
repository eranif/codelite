#include "phpcs.h"
#include <wx/xrc/xmlres.h>

static phpcs* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == NULL) {
        thePlugin = new phpcs(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Anders Jenbo"));
    info.SetName(wxT("phpcs"));
    info.SetDescription(_("Run code style checking on PHP source files"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

phpcs::phpcs(IManager *manager)
    : IPlugin(manager)
{
    m_longName = _("Run code style checking on PHP source files");
    m_shortName = wxT("phpcs");
}

phpcs::~phpcs()
{
}

clToolBar *phpcs::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void phpcs::CreatePluginMenu(wxMenu *pluginsMenu)
{
}

void phpcs::UnPlug()
{
}
