#include "CompileCommandsGenerator.h"
#include "LSPDetectorManager.hpp"
#include "LanguageServerConfig.h"
#include "LanguageServerSettingsDlg.h"
#include "ServiceProviderManager.h"
#include "cl_standard_paths.h"
#include "event_notifier.h"
#include "file_logger.h"
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
    if(thePlugin == NULL) {
        thePlugin = new LanguageServerPlugin(manager);
    }
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
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &LanguageServerPlugin::OnEditorContextMenu, this);
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
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &LanguageServerPlugin::OnEditorContextMenu, this);
    LanguageServerConfig::Get().Save();
    m_servers.reset(nullptr);
}

void LanguageServerPlugin::OnSettings(wxCommandEvent& e)
{
    LanguageServerSettingsDlg dlg(EventNotifier::Get()->TopFrame(), false);
    if(dlg.ShowModal() == wxID_OK) {
        // restart all language servers
        dlg.Save();
        if(m_servers) {
            // Lets assume that we fixed something in the settings
            // and clear all the restart counters
            m_servers->ClearRestartCounters();
            m_servers->Reload();
        }
    }
}

void LanguageServerPlugin::OnRestartLSP(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(m_servers) {
        m_servers->Reload();
    }
}

void LanguageServerPlugin::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    // launch a thread to locate any LSP installed on this machine
    if(LanguageServerConfig::Get().GetServers().empty()) {
        clDEBUG() << "Scanning..." << clEndl;
        std::thread thr(
            [=](LanguageServerPlugin* plugin) {
                std::vector<LSPDetector::Ptr_t> matches;
                LSPDetectorManager detector;
                clDEBUG() << "***"
                          << "Scanning for LSPs... ***" << clEndl;
                if(detector.Scan(matches)) {
                    clDEBUG() << "   ******"
                              << "found!" << clEndl;
                }
                clDEBUG() << "***"
                          << "Scanning for LSPs... is done ***" << clEndl;
                clDEBUG() << "*** Calling   ConfigureLSPs" << clEndl;
                plugin->CallAfter(&LanguageServerPlugin::ConfigureLSPs, matches);
            },
            this);
        thr.detach();
    }
}

void LanguageServerPlugin::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    CHECK_COND_RET(m_servers);

    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    LanguageServerProtocol::Ptr_t lsp = m_servers->GetServerForFile(editor->GetFileName());
    CHECK_PTR_RET(lsp);

    auto langs = lsp->GetSupportedLanguages();

    static wxString cppfile = "file.cpp";
    static wxString phpfile = "file.php";
    // CXX, C and PHP have their own context menus, dont add ours as well
    if(lsp->CanHandle(cppfile) || lsp->CanHandle(phpfile)) {
        return;
    }

    // TODO :: add here
    // Goto definition
    // Goto implementation
    // menu entries
    wxMenu* menu = event.GetMenu();
    menu->PrependSeparator();
    menu->Prepend(XRCID("lsp_find_symbol"), _("Find Symbol"));

    menu->Bind(wxEVT_MENU, &LanguageServerPlugin::OnMenuFindSymbol, this, XRCID("lsp_find_symbol"));
}

void LanguageServerPlugin::OnMenuFindSymbol(wxCommandEvent& event)
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    clCodeCompletionEvent findEvent(wxEVT_CC_FIND_SYMBOL);
    findEvent.SetEventObject(editor->GetCtrl());
    findEvent.SetEditor(editor->GetCtrl());
    findEvent.SetPosition(editor->GetCurrentPosition());
    ServiceProviderManager::Get().ProcessEvent(findEvent);
}

void LanguageServerPlugin::ConfigureLSPs(const std::vector<LSPDetector::Ptr_t>& lsps)
{
    clDEBUG() << "   ******"
              << "ConfigureLSPs is called!" << clEndl;
    if(lsps.empty()) {
        clDEBUG() << "ConfigureLSPs: no LSPs found. Nothing to be done here" << clEndl;
        return;
    }

    LanguageServerConfig& config = LanguageServerConfig::Get();
    // remove clangd installed under ~/.codelite/lsp/clang-tools
    wxArrayString serversToRemove;
    for(const auto& server : config.GetServers()) {
        if(server.second.GetCommand().Contains(".codelite/lsp/clang-tools")) {
            serversToRemove.Add(server.first);
        }
    }

    bool force = !serversToRemove.IsEmpty();
    // remove all old entries
    for(const auto& name : serversToRemove) {
        clSYSTEM() << "Removing broken LSP server:" << name << endl;
        config.RemoveServer(name);
    }

    clDEBUG() << "ConfigureLSPs: there are currently" << config.GetServers().size() << "LSPs configured" << clEndl;
    if(config.GetServers().empty() || force) {
        clDEBUG() << "No LSPs configured - auto configuring" << clEndl;
        // Only if the user did not configure LSP before, we configure it for him
        for(auto lsp : lsps) {
            LanguageServerEntry entry;
            lsp->GetLanguageServerEntry(entry);
            entry.SetEnabled(true);
            config.AddServer(entry);
            clDEBUG() << "Adding LSP:" << entry.GetName() << clEndl;
        }
        config.SetEnabled(true);
        config.Save();
        if(m_servers) {
            m_servers->Reload();
        }
    }
}
