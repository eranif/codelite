#include "CompileCommandsGenerator.h"
#include "LSPDetectorManager.hpp"
#include "LanguageServerConfig.h"
#include "LanguageServerSettingsDlg.h"
#include "cl_standard_paths.h"
#include "event_notifier.h"
#include "globals.h"
#include "ieditor.h"
#include "languageserver.h"
#include <macros.h>
#include <thread>
#include <wx/app.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

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
    m_servers.reset(new LanguageServerCluster());
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &LanguageServerPlugin::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_INFO_BAR_BUTTON, &LanguageServerPlugin::OnInfoBarButton, this);
    wxTheApp->Bind(wxEVT_MENU, &LanguageServerPlugin::OnSettings, this, XRCID("language-server-settings"));
    wxTheApp->Bind(wxEVT_MENU, &LanguageServerPlugin::OnRestartLSP, this, XRCID("language-server-restart"));
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
    menu->Append(XRCID("language-server-restart"), _("Restart Language Servers"));
    pluginsMenu->Append(wxID_ANY, _("Language Server"), menu);
}

void LanguageServerPlugin::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &LanguageServerPlugin::OnSettings, this, XRCID("language-server-settings"));
    wxTheApp->Unbind(wxEVT_MENU, &LanguageServerPlugin::OnRestartLSP, this, XRCID("language-server-restart"));
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &LanguageServerPlugin::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_INFO_BAR_BUTTON, &LanguageServerPlugin::OnInfoBarButton, this);
    LanguageServerConfig::Get().Save();
    m_servers.reset(nullptr);
}

void LanguageServerPlugin::OnSettings(wxCommandEvent& e)
{
    LanguageServerSettingsDlg dlg(EventNotifier::Get()->TopFrame(), false);
    if(dlg.ShowModal() == wxID_OK) {
        // restart all language servers
        dlg.Save();
        if(m_servers) { m_servers->Reload(); }
    }
}

void LanguageServerPlugin::OnRestartLSP(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(m_servers) { m_servers->Reload(); }
}

void LanguageServerPlugin::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    // Launch a thread to locate any LSP installed on this machine
    // But do this only if we don't have any LSP defined already
    bool autoScan = clConfig::Get().Read("LSPAutoScanOnStartup", true);
    if(autoScan && LanguageServerConfig::Get().GetServers().empty()) {
        clConfig::Get().Write("LSPAutoScanOnStartup", false);
        std::thread thr(
            [=](LanguageServerPlugin* plugin) {
                std::vector<LSPDetector::Ptr_t> matches;
                LSPDetectorManager detector;
                if(detector.Scan(matches)) {
                    // Prompt the user to configure LSP
                    plugin->CallAfter(&LanguageServerPlugin::PromptUserToConfigureLSP);
                }
            },
            this);
        thr.detach();
    }
}

void LanguageServerPlugin::PromptUserToConfigureLSP()
{
    clGetManager()->DisplayMessage(
        _("CodeLite found Language Servers installed on your machine\nWould you like to configure them now?"),
        wxICON_QUESTION, { { XRCID("language-server-settings"), _("Yes") }, { wxID_CANCEL, _("No") } });
}

void LanguageServerPlugin::OnInfoBarButton(clCommandEvent& event)
{
    event.Skip();
    if(event.GetInt() == XRCID("language-server-settings")) {
        event.Skip(false);
        LanguageServerSettingsDlg dlg(EventNotifier::Get()->TopFrame(), true);
        if(dlg.ShowModal() == wxID_OK) {
            // restart all language servers
            dlg.Save();
            if(m_servers) { m_servers->Reload(); }
        }
    }
}
