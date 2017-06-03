#include "phprefactoring.h"
#include <wx/xrc/xmlres.h>

static PHPRefactoring* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) {
        thePlugin = new PHPRefactoring(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Anders Jenbo"));
    info.SetName(wxT("PHPRefactoring"));
    info.SetDescription(_("Uses PHP Refactoring Browser to provide refactoring capabilities for php"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}

PHPRefactoring::PHPRefactoring(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Uses PHP Refactoring Browser to provide refactoring capabilities for php");
    m_shortName = wxT("PHPRefactoring");
}

PHPRefactoring::~PHPRefactoring()
{
}

clToolBar* PHPRefactoring::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void PHPRefactoring::CreatePluginMenu(wxMenu* pluginsMenu)
{
}

void PHPRefactoring::UnPlug()
{
}
