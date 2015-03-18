#include "webtools.h"
#include <wx/xrc/xmlres.h>

static WebTools* thePlugin = NULL;

// Define the plugin entry point
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new WebTools(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("WebTools"));
    info.SetDescription(wxT("Support for JavScript, HTML and other web development tools"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

WebTools::WebTools(IManager* manager)
    : IPlugin(manager)
{
    m_longName = wxT("Support for JavScript, HTML and other web development tools");
    m_shortName = wxT("WebTools");
}

WebTools::~WebTools() {}

clToolBar* WebTools::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void WebTools::CreatePluginMenu(wxMenu* pluginsMenu)
{
}

void WebTools::UnPlug()
{
}
