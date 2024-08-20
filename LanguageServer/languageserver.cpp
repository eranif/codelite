#include "languageserver.h"

#include "LSPDetectorManager.hpp"
#include "LanguageServerConfig.h"
#include "LanguageServerSettingsDlg.h"
#include "StringUtils.h"
#include "clInfoBar.h"
#include "cl_standard_paths.h"
#include "clangd/CompileCommandsGenerator.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "macros.h"

#include <thread>
#include <wx/app.h>
#include <wx/datetime.h>
#include <wx/notifmsg.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    return new LanguageServerPlugin(manager);
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
    m_servers = new LanguageServerCluster(this);

    // add log view
    m_logView = new LanguageServerLogView(m_mgr->BookGet(PaneId::BOTTOM_BAR), m_servers);
    m_mgr->BookAddPage(PaneId::BOTTOM_BAR, m_logView, _("Language Server"));
    m_tabToggler.reset(new clTabTogglerHelper(_("Language Server"), m_logView, "", NULL));

    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &LanguageServerPlugin::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &LanguageServerPlugin::OnEditorContextMenu, this);
    wxTheApp->Bind(wxEVT_MENU, &LanguageServerPlugin::OnSettings, this, XRCID("language-server-settings"));
    wxTheApp->Bind(wxEVT_MENU, &LanguageServerPlugin::OnRestartLSP, this, XRCID("language-server-restart"));
    clGetManager()->GetInfoBar()->Bind(wxEVT_BUTTON, &LanguageServerPlugin::OnFixLSPPaths, this,
                                       XRCID("lsp-fix-paths"));

    EventNotifier::Get()->Bind(wxEVT_LSP_STOP_ALL, &LanguageServerPlugin::OnLSPStopAll, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_START_ALL, &LanguageServerPlugin::OnLSPStartAll, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_RESTART_ALL, &LanguageServerPlugin::OnLSPRestartAll, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_STOP, &LanguageServerPlugin::OnLSPStopOne, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_START, &LanguageServerPlugin::OnLSPStartOne, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_RESTART, &LanguageServerPlugin::OnLSPRestartOne, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_CONFIGURE, &LanguageServerPlugin::OnLSPConfigure, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_DELETE, &LanguageServerPlugin::OnLSPDelete, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_OPEN_SETTINGS_DLG, &LanguageServerPlugin::OnLSPShowSettingsDlg, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_ENABLE_SERVER, &LanguageServerPlugin::OnLSPEnableServer, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_DISABLE_SERVER, &LanguageServerPlugin::OnLSPDisableServer, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerPlugin::OnWorkspaceClosed, this);

    /// initialise the LSP library
    LSP::Initialise();

    CallAfter(&LanguageServerPlugin::CheckServers);
}

LanguageServerPlugin::~LanguageServerPlugin() { wxDELETE(m_servers); }

void LanguageServerPlugin::CheckServers()
{
    auto broken_lsps = GetBrokenLSPs();
    if (broken_lsps.empty()) {
        return;
    }

    clSYSTEM() << "The following LSPs contain paths to a non existing locations:" << broken_lsps << endl;
    // Show a notification message with a suggestion for a fix
    wxString message;
    message << "The following LSPs contain paths to a non existing locations: [";
    for (const auto& name : broken_lsps) {
        message << name << ", ";
    }
    message.RemoveLast(2);
    message << "]";

    clGetManager()->DisplayMessage(message, wxICON_WARNING,
                                   { { wxID_CANCEL, _("Cancel") }, { XRCID("lsp-fix-paths"), _("Attempt to fix") } });
}

void LanguageServerPlugin::CreateToolBar(clToolBarGeneric* toolbar)
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

    EventNotifier::Get()->Unbind(wxEVT_LSP_STOP_ALL, &LanguageServerPlugin::OnLSPStopAll, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_START_ALL, &LanguageServerPlugin::OnLSPStartAll, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_RESTART_ALL, &LanguageServerPlugin::OnLSPRestartAll, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_STOP, &LanguageServerPlugin::OnLSPStopOne, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_START, &LanguageServerPlugin::OnLSPStartOne, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_RESTART, &LanguageServerPlugin::OnLSPRestartOne, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_CONFIGURE, &LanguageServerPlugin::OnLSPConfigure, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_DELETE, &LanguageServerPlugin::OnLSPDelete, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_OPEN_SETTINGS_DLG, &LanguageServerPlugin::OnLSPShowSettingsDlg, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_ENABLE_SERVER, &LanguageServerPlugin::OnLSPEnableServer, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_DISABLE_SERVER, &LanguageServerPlugin::OnLSPDisableServer, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerPlugin::OnWorkspaceClosed, this);

    LanguageServerConfig::Get().Save();

    // before this plugin is un-plugged we must remove the tab we added
    if (!m_mgr->BookDeletePage(PaneId::BOTTOM_BAR, m_logView)) {
        m_logView->Destroy();
    }
    m_logView = nullptr;
    wxDELETE(m_servers);
}

void LanguageServerPlugin::OnSettings(wxCommandEvent& e)
{
    LanguageServerSettingsDlg dlg(EventNotifier::Get()->TopFrame(), false);
    if (dlg.ShowModal() == wxID_OK) {
        // restart all language servers
        dlg.Save();
        if (m_servers) {
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
    if (m_servers) {
        m_servers->Reload();
    }
}

void LanguageServerPlugin::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    // launch a thread to locate any LSP installed on this machine

    bool force = false;
    const auto& servers = LanguageServerConfig::Get().GetServers();
    for (auto& server : servers) {
        if (server.second.GetCommand().Contains(".codelite/lsp/clang-tools")) {
            force = true;
            break;
        }
    }

    if (LanguageServerConfig::Get().GetServers().empty() || force) {
        LSP_DEBUG() << "Scanning..." << endl;
        std::thread thr(
            [=](LanguageServerPlugin* plugin) {
                std::vector<LSPDetector::Ptr_t> matches;
                LSPDetectorManager detector;
                LSP_DEBUG() << "***"
                            << "Scanning for LSPs... ***" << endl;
                if (detector.Scan(matches)) {
                    LSP_DEBUG() << "   ******"
                                << "found!" << endl;
                }
                LSP_DEBUG() << "***"
                            << "Scanning for LSPs... is done ***" << endl;
                LSP_DEBUG() << "*** Calling   ConfigureLSPs" << endl;
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

    LanguageServerProtocol::Ptr_t lsp = m_servers->GetServerForEditor(editor);
    CHECK_PTR_RET(lsp);

    bool add_find_symbol = !lsp->CanHandle(FileExtManager::TypePhp);
    bool add_find_references = lsp->IsReferencesSupported();
    bool add_rename_symbol = lsp->IsRenameSupported();

    // nothing to be done here
    if (!add_find_symbol && !add_find_references && !add_rename_symbol) {
        return;
    }

    wxMenu* menu = event.GetMenu();
    if (add_find_references) {
        menu->PrependSeparator();
        menu->Prepend(XRCID("lsp_find_references"), _("Find references"));
    }
    menu->PrependSeparator();
    if (add_rename_symbol) {
        menu->Prepend(XRCID("lsp_rename_symbol"), _("Rename symbol"));
    }
    menu->Prepend(XRCID("lsp_find_symbol"), _("Find symbol"));

    menu->Bind(wxEVT_MENU, &LanguageServerPlugin::OnMenuFindSymbol, this, XRCID("lsp_find_symbol"));
    menu->Bind(wxEVT_MENU, &LanguageServerPlugin::OnMenuFindReferences, this, XRCID("lsp_find_references"));
    menu->Bind(wxEVT_MENU, &LanguageServerPlugin::OnMenuRenameSymbol, this, XRCID("lsp_rename_symbol"));
}

void LanguageServerPlugin::OnMenuRenameSymbol(wxCommandEvent& event)
{
    wxUnusedVar(event);

    LSP_DEBUG() << "OnMenuRenameSymbol is called" << endl;

    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    LanguageServerProtocol::Ptr_t lsp = m_servers->GetServerForEditor(editor);
    CHECK_PTR_RET(lsp);

    lsp->RenameSymbol(editor);
}

void LanguageServerPlugin::OnMenuFindReferences(wxCommandEvent& event)
{
    wxUnusedVar(event);

    LSP_DEBUG() << "OnMenuFindReferences is called" << endl;

    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    LanguageServerProtocol::Ptr_t lsp = m_servers->GetServerForEditor(editor);
    CHECK_PTR_RET(lsp);

    lsp->FindReferences(editor);
}

void LanguageServerPlugin::OnMenuFindSymbol(wxCommandEvent& event)
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    clCodeCompletionEvent findEvent(wxEVT_CC_FIND_SYMBOL);
    findEvent.SetPosition(editor->GetCurrentPosition());
    findEvent.SetFileName(editor->GetFileName().GetFullPath());
    EventNotifier::Get()->AddPendingEvent(findEvent);
}

void LanguageServerPlugin::ConfigureLSPs(const std::vector<LSPDetector::Ptr_t>& lsps)
{
    LSP_DEBUG() << "   ******"
                << "ConfigureLSPs is called!" << endl;
    if (lsps.empty()) {
        LSP_DEBUG() << "ConfigureLSPs: no LSPs found. Nothing to be done here" << endl;
        return;
    }

    LanguageServerConfig& config = LanguageServerConfig::Get();
    // remove clangd installed under ~/.codelite/lsp/clang-tools
    wxArrayString serversToRemove;
    for (const auto& server : config.GetServers()) {
        if (server.second.GetCommand().Contains(".codelite/lsp/clang-tools")) {
            serversToRemove.Add(server.first);
        }
    }

    bool force = !serversToRemove.IsEmpty();
    // remove all old entries
    for (const auto& name : serversToRemove) {
        LSP_SYSTEM() << "Removing broken LSP server:" << name << endl;
        config.RemoveServer(name);
    }

    LSP_DEBUG() << "ConfigureLSPs: there are currently" << config.GetServers().size() << "LSPs configured" << endl;
    if (config.GetServers().empty() || force) {
        LSP_DEBUG() << "No LSPs configured - auto configuring" << endl;
        // Only if the user did not configure LSP before, we configure it for him
        for (auto lsp : lsps) {
            LanguageServerEntry entry;
            lsp->GetLanguageServerEntry(entry);
            config.AddServer(entry);
            LSP_DEBUG() << "Adding LSP:" << entry.GetName() << endl;
        }
        config.SetEnabled(true);
        config.Save();
        if (m_servers) {
            m_servers->Reload();
        }
    }
}

void LanguageServerPlugin::OnLSPStopAll(clLanguageServerEvent& event)
{
    CHECK_PTR_RET(m_servers);
    m_servers->StopAll();
}

void LanguageServerPlugin::OnLSPStartAll(clLanguageServerEvent& event)
{
    CHECK_PTR_RET(m_servers);
    wxBusyCursor bc;
    m_servers->StartAll();
}

void LanguageServerPlugin::OnLSPRestartAll(clLanguageServerEvent& event)
{
    wxBusyCursor bc;
    LSP_DEBUG() << "LSP: restarting all LSPs" << endl;
    CHECK_PTR_RET(m_servers);
    m_servers->StopAll();
    m_servers->StartAll();
    LSP_DEBUG() << "LSP: restarting all LSPs...done" << endl;
}

void LanguageServerPlugin::OnLSPStopOne(clLanguageServerEvent& event)
{
    CHECK_PTR_RET(m_servers);
    LanguageServerProtocol::Ptr_t lsp = m_servers->GetServerByName(event.GetLspName());
    CHECK_PTR_RET(lsp);
    lsp->Stop();
}

void LanguageServerPlugin::OnLSPStartOne(clLanguageServerEvent& event)
{
    CHECK_PTR_RET(m_servers);
    auto lsp = m_servers->GetServerByName(event.GetLspName());
    CHECK_PTR_RET(lsp);
    lsp->Start();
}

void LanguageServerPlugin::OnLSPRestartOne(clLanguageServerEvent& event)
{
    CHECK_PTR_RET(m_servers);
    m_servers->RestartServer(event.GetLspName());
}

void LanguageServerPlugin::OnLSPConfigure(clLanguageServerEvent& event)
{

    LanguageServerEntry entry;
    LanguageServerEntry* pentry = &entry;

    auto d = LanguageServerConfig::Get().GetServer(event.GetLspName());
    if (!d.IsNull()) {
        LSP_DEBUG() << "an LSP with the same name:" << event.GetLspName() << "already exists. updating it" << endl;
        pentry = &d;
    }

    // Configure new LSP
    pentry->SetLanguages(event.GetLanguages());
    pentry->SetName(event.GetLspName());
    pentry->SetCommand(event.GetLspCommand());
    pentry->SetDisaplayDiagnostics(event.GetFlags() & clLanguageServerEvent::kDisaplyDiags);
    pentry->SetConnectionString(event.GetConnectionString());
    pentry->SetEnabled(event.GetFlags() & clLanguageServerEvent::kEnabled);
    pentry->SetWorkingDirectory(event.GetRootUri());
    LanguageServerConfig::Get().AddServer(*pentry);
}

void LanguageServerPlugin::OnLSPDelete(clLanguageServerEvent& event)
{
    CHECK_PTR_RET(m_servers);
    LSP_DEBUG() << "Deleting server:" << event.GetLspName() << endl;
    m_servers->DeleteServer(event.GetLspName());
    LSP_DEBUG() << "Success" << endl;
}

void LanguageServerPlugin::OnLSPShowSettingsDlg(clLanguageServerEvent& event)
{
    wxCommandEvent dummy;
    OnSettings(dummy);
}

wxString LanguageServerPlugin::GetEditorFilePath(IEditor* editor) const { return editor->GetRemotePathOrLocal(); }

void LanguageServerPlugin::OnLSPEnableServer(clLanguageServerEvent& event)
{
    auto& lsp_config = LanguageServerConfig::Get().GetServer(event.GetLspName());
    if (lsp_config.IsNull()) {
        return;
    }
    lsp_config.SetEnabled(true);
}

void LanguageServerPlugin::OnLSPDisableServer(clLanguageServerEvent& event)
{
    auto& lsp_config = LanguageServerConfig::Get().GetServer(event.GetLspName());
    if (lsp_config.IsNull()) {
        return;
    }
    lsp_config.SetEnabled(false);
}

void LanguageServerPlugin::LogMessage(const wxString& server_name, const wxString& message, int log_leve)
{
    auto& builder = m_logView->GetDvListCtrl()->GetBuilder(true);

    int ansi_colour_code = AnsiColours::NormalText();
    wxString label = "T "; // trace
    switch (log_leve) {
    case 1:
        ansi_colour_code = AnsiColours::Red(); // error
        label = "E ";
        break;
    case 2:
        ansi_colour_code = AnsiColours::Yellow(); // warning
        label = "W ";
        break;
    case 3:
        ansi_colour_code = AnsiColours::Green(); // info
        label = "I ";
    default:
        break;
    }

    builder.Add(label, ansi_colour_code);
    builder.Add(wxDateTime::Now().FormatISOTime() + " ", AnsiColours::Gray());
    builder.Add(server_name + " ", AnsiColours::Magenta());
    builder.Add(message, AnsiColours::NormalText());
    m_logView->GetDvListCtrl()->AddLine(builder.GetString(), false);
    m_logView->GetDvListCtrl()->ScrollToBottom();
}

void LanguageServerPlugin::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    m_logView->GetDvListCtrl()->DeleteAllItems();
}

void LanguageServerPlugin::OnFixLSPPaths(wxCommandEvent& event)
{
    // Hide and layout the view
    clGetManager()->GetInfoBar()->Hide();
    EventNotifier::Get()->TopFrame()->SendSizeEvent(wxSEND_EVENT_POST);

    wxUnusedVar(event);
    auto broken_lsps = GetBrokenLSPs();
    if (broken_lsps.empty()) {
        return;
    }

    wxBusyCursor bc;
    std::vector<LSPDetector::Ptr_t> matches;
    LSPDetectorManager detector;
    if (detector.Scan(matches)) {
        wxArrayString fixed;
        for (const wxString& broken_lsp : broken_lsps) {
            auto& lsp = LanguageServerConfig::Get().GetServer(broken_lsp);
            if (lsp.IsNull()) {
                // Could not find it
                continue;
            }

            // Check to see if
            for (size_t i = 0; i < matches.size(); ++i) {
                LanguageServerEntry entry;
                matches[i]->GetLanguageServerEntry(entry);
                if (entry.GetName() == broken_lsp) {
                    lsp = entry;
                    fixed.Add(broken_lsp);
                    break;
                }
            }
        }

        if (!fixed.empty()) {
            LanguageServerConfig::Get().Save();
            m_servers->Reload();
        }
    }
}

wxArrayString LanguageServerPlugin::GetBrokenLSPs() const
{
    wxArrayString broken_lsps;
    const auto& servers = LanguageServerConfig::Get().GetServers();
    for (const auto& [name, server] : servers) {
        auto argv = StringUtils::BuildArgv(server.GetCommand());
        // Check that the first argument (the executable path) exists
        if (server.IsEnabled() && argv.empty() || !wxFileName::FileExists(argv[0])) {
            broken_lsps.push_back(name);
        }
    }
    return broken_lsps;
}
