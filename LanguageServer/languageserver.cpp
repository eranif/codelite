#include "languageserver.h"
#include <wx/xrc/xmlres.h>
#include "ieditor.h"
#include <wx/stc/stc.h>
#include "event_notifier.h"
#include <macros.h>
#include "globals.h"
#include "LanguageServerConfig.h"
#include "LanguageServerSettingsDlg.h"
#include "cl_standard_paths.h"
#include "CompileCommandsGenerator.h"

static LanguageServerPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new LanguageServerPlugin(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran"));
    info.SetName(wxT("LanguageServerPlugin"));
    info.SetDescription(_("Support for Language Server Protocol (LSP)"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

LanguageServerPlugin::LanguageServerPlugin(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Support for Language Server Protocol (LSP)");
    m_shortName = wxT("LanguageServerPlugin");

    // Load the configuration
    LanguageServerConfig::Get().Load();

    // Start all the servers
    m_servers.reset(new LanguageServerCluster());
    m_servers->Reload();

    m_compileCommandsGenerator.reset(new CompileCommandsGenerator());

    EventNotifier::Get()->Bind(wxEVT_BUILD_ENDED, &LanguageServerPlugin::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_PROJ_FILE_ADDED, &LanguageServerPlugin::OnFilesAdded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &LanguageServerPlugin::OnWorkspaceLoaded, this);
}

LanguageServerPlugin::~LanguageServerPlugin() {}

void LanguageServerPlugin::CreateToolBar(clToolBar* toolbar)
{
    // You can add items to the main toolbar here
    wxUnusedVar(toolbar);
}

void LanguageServerPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(XRCID("language-server-settings"), _("Settings"));
    menu->Bind(wxEVT_MENU, &LanguageServerPlugin::OnSettings, this, XRCID("language-server-settings"));
    pluginsMenu->Append(wxID_ANY, _("Language Server"), menu);
}

void LanguageServerPlugin::UnPlug()
{
    LanguageServerConfig::Get().Save();
    m_servers.reset(nullptr);

    EventNotifier::Get()->Unbind(wxEVT_BUILD_ENDED, &LanguageServerPlugin::OnBuildEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_PROJ_FILE_ADDED, &LanguageServerPlugin::OnFilesAdded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &LanguageServerPlugin::OnWorkspaceLoaded, this);
}

void LanguageServerPlugin::OnSettings(wxCommandEvent& e)
{
    LanguageServerSettingsDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // restart all language servers
        dlg.Save();
        m_servers->Reload();
    }
}

void LanguageServerPlugin::OnBuildEnded(clBuildEvent& event)
{
    event.Skip();
    GenerateCompileCommands();
}

void LanguageServerPlugin::GenerateCompileCommands()
{
    // this is a self destruct objecy
    m_compileCommandsGenerator->GenerateCompileCommands();
}

void LanguageServerPlugin::OnFilesAdded(clCommandEvent& event)
{
    event.Skip();
    GenerateCompileCommands();
}

void LanguageServerPlugin::OnWorkspaceLoaded(wxCommandEvent& event)
{
    event.Skip();
    GenerateCompileCommands();
}
