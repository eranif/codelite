#include "php.h"

#include "Debugger/debuggermanager.h"
#include "PHPDebugPane.h"
#include "PHPXDebugSetupWizard.h"
#include "XDebugSettingsDlg.h"
#include "cl_standard_paths.h"
#include "evalpane.h"
#include "event_notifier.h"
#include "globals.h"
#include "localsview.h"
#include "php_settings_dlg.h"
#include "php_strings.h"
#include "php_utils.h"
#include "plugin.h"
#include "xdebugevent.h"

#include <wx/app.h>
#include <wx/xrc/xmlres.h>

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager) { return new PhpPlugin(manager); }

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("PHP"));
    info.SetDescription(_("XDebug support for PHP"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

PhpPlugin::PhpPlugin(IManager* manager)
    : IPlugin(manager)
    , m_debuggerPane(nullptr)
    , m_xdebugLocalsView(nullptr)
    , m_xdebugEvalPane(nullptr)
{
    m_longName = _("XDebug support for PHP");
    m_shortName = wxT("PHP");

    XDebugManager::Initialize(this);

    // Let the user pick XDebug as the debugger of a File System Workspace
    wxArrayString debuggers;
    debuggers.Add(XDebugSettings::DEBUGGER_NAME);
    DebuggerMgr::Get().RegisterDebuggers(m_shortName, debuggers);

    // Connect events
    EventNotifier::Get()->Connect(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS,
                                  clDebugEventHandler(PhpPlugin::OnXDebugDeleteAllBreakpoints),
                                  nullptr,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_PHP_LOAD_URL, PHPEventHandler(PhpPlugin::OnLoadURL), nullptr, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR_MARGIN, &PhpPlugin::OnMarginContextMenu, this);

    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_STARTED, &PhpPlugin::OnDebugStarted, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_ENDED, &PhpPlugin::OnDebugEnded, this);

    // Menu bar actions
    wxTheApp->Bind(wxEVT_MENU, &PhpPlugin::OnRunXDebugDiagnostics, this, wxID_PHP_RUN_XDEBUG_DIAGNOSTICS);
    wxTheApp->Bind(wxEVT_MENU, &PhpPlugin::OnMenuCommand, this, wxID_PHP_SETTINGS);
    wxTheApp->Bind(wxEVT_MENU, &PhpPlugin::OnXDebugSettings, this, wxID_XDEBUG_SETTING);
    wxTheApp->Bind(wxEVT_UPDATE_UI, &PhpPlugin::OnXDebugSettingsUI, this, wxID_XDEBUG_SETTING);
    wxTheApp->Bind(wxEVT_MENU, &PhpPlugin::OnXDebugWaitForConnection, this, wxID_XDEBUG_WAIT_FOR_CONNECTION);
    wxTheApp->Bind(wxEVT_UPDATE_UI, &PhpPlugin::OnXDebugWaitForConnectionUI, this, wxID_XDEBUG_WAIT_FOR_CONNECTION);

    CallAfter(&PhpPlugin::FinalizeStartup);
}

void PhpPlugin::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void PhpPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    if (m_mgr->GetMenuBar()) {
        DoPlaceMenuBar(m_mgr->GetMenuBar());
    }
}

void PhpPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void PhpPlugin::UnPlug()
{
    DebuggerMgr::Get().UnregisterDebuggers(m_shortName);
    XDebugManager::Free();
    EventNotifier::Get()->Disconnect(wxEVT_DBG_UI_DELETE_ALL_BREAKPOINTS,
                                     clDebugEventHandler(PhpPlugin::OnXDebugDeleteAllBreakpoints),
                                     nullptr,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_PHP_LOAD_URL, PHPEventHandler(PhpPlugin::OnLoadURL), nullptr, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR_MARGIN, &PhpPlugin::OnMarginContextMenu, this);

    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_STARTED, &PhpPlugin::OnDebugStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_ENDED, &PhpPlugin::OnDebugEnded, this);

    // Menu bar actions
    wxTheApp->Unbind(wxEVT_MENU, &PhpPlugin::OnRunXDebugDiagnostics, this, wxID_PHP_RUN_XDEBUG_DIAGNOSTICS);
    wxTheApp->Unbind(wxEVT_MENU, &PhpPlugin::OnMenuCommand, this, wxID_PHP_SETTINGS);
    wxTheApp->Unbind(wxEVT_MENU, &PhpPlugin::OnXDebugSettings, this, wxID_XDEBUG_SETTING);
    wxTheApp->Unbind(wxEVT_UPDATE_UI, &PhpPlugin::OnXDebugSettingsUI, this, wxID_XDEBUG_SETTING);
    wxTheApp->Unbind(wxEVT_MENU, &PhpPlugin::OnXDebugWaitForConnection, this, wxID_XDEBUG_WAIT_FOR_CONNECTION);
    wxTheApp->Unbind(wxEVT_UPDATE_UI, &PhpPlugin::OnXDebugWaitForConnectionUI, this, wxID_XDEBUG_WAIT_FOR_CONNECTION);

    SafelyDetachAndDestroyPane(m_debuggerPane, "XDebug");
    SafelyDetachAndDestroyPane(m_xdebugLocalsView, "XDebugLocals");
    SafelyDetachAndDestroyPane(m_xdebugEvalPane, "XDebugEval");
}

void PhpPlugin::OnMarginContextMenu(clContextMenuEvent& e)
{
    e.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    if (!editor || !IsPHPFileByExt(editor->GetFileName().GetFullPath())) {
        return;
    }

    // Remove the breakpoint entries that XDebug does not support
    wxMenu* menu = e.GetMenu();
    for (const char* id : {"insert_temp_breakpoint",
                           "insert_disabled_breakpoint",
                           "insert_cond_breakpoint",
                           "ignore_breakpoint",
                           "toggle_breakpoint_enabled_status",
                           "edit_breakpoint"}) {
        if (menu->FindItem(XRCID(id))) {
            menu->Remove(XRCID(id));
        }
    }
}

void PhpPlugin::DoPlaceMenuBar(wxMenuBar* menuBar)
{
    // Add our menu bar
    wxMenu* phpMenuBarMenu = new wxMenu();
    phpMenuBarMenu->Append(wxID_PHP_SETTINGS, _("PHP Settings..."), _("PHP Settings..."));
    phpMenuBarMenu->Append(wxID_XDEBUG_SETTING, _("XDebug Settings..."), _("XDebug settings of the open workspace"));
    phpMenuBarMenu->Append(wxID_XDEBUG_WAIT_FOR_CONNECTION,
                           _("Wait for XDebug to Connect"),
                           _("Start a debug session when XDebug connects, for example from a web browser"));
    phpMenuBarMenu->Append(
        wxID_PHP_RUN_XDEBUG_DIAGNOSTICS, _("Run XDebug Setup Wizard..."), _("Run XDebug Setup Wizard..."));

    int helpLoc = menuBar->FindMenu(_("Help"));
    if (helpLoc != wxNOT_FOUND) {
        menuBar->Insert(helpLoc, phpMenuBarMenu, _("P&HP"));
    }
}

void PhpPlugin::OnMenuCommand(wxCommandEvent& e)
{
    switch (e.GetId()) {
    case wxID_PHP_SETTINGS: {
        PHPSettingsDlg dlg(FRAME);
        dlg.ShowModal();
    } break;
    default:
        e.Skip();
        break;
    }
}

void PhpPlugin::OnXDebugSettings(wxCommandEvent& e)
{
    XDebugSettings settings;
    if (!settings.Load()) {
        return;
    }
    XDebugSettingsDlg dlg(FRAME, settings);
    dlg.ShowModal();
}

void PhpPlugin::OnXDebugSettingsUI(wxUpdateUIEvent& e)
{
    // Remote workspaces have no settings file
    e.Enable(XDebugSettings::GetSettingsFile().IsOk());
}

void PhpPlugin::OnXDebugWaitForConnection(wxCommandEvent& e) { XDebugManager::Get().StartListener(); }

void PhpPlugin::OnXDebugWaitForConnectionUI(wxUpdateUIEvent& e)
{
    e.Enable(XDebugSettings::IsActive() && !XDebugManager::Get().IsDebugSessionRunning());
}

void PhpPlugin::OnLoadURL(PHPEvent& e)
{
    e.Skip();
    ::wxLaunchDefaultBrowser(e.GetUrl());
}

// Debugger events
void PhpPlugin::OnDebugEnded(XDebugEvent& e)
{
    e.Skip();

    // Save the layout
    wxFileName fnConfig(clStandardPaths::Get().GetUserDataDir(), "xdebug-perspective");
    fnConfig.AppendDir("config");

    wxFFile fp(fnConfig.GetFullPath(), "w+b");
    if (fp.IsOpened()) {
        fp.Write(m_mgr->GetDockingManager()->SavePerspective());
        fp.Close();
    }

    if (!m_savedPerspective.IsEmpty()) {
        m_mgr->GetDockingManager()->LoadPerspective(m_savedPerspective);
        m_savedPerspective.Clear();
    }
}

void PhpPlugin::OnDebugStarted(XDebugEvent& e)
{
    e.Skip();
    DoEnsureXDebugPanesVisible();
}

void PhpPlugin::OnXDebugDeleteAllBreakpoints(clDebugEvent& e)
{
    e.Skip();
    PHPEvent eventDelAllBP(wxEVT_PHP_DELETE_ALL_BREAKPOINTS);
    EventNotifier::Get()->AddPendingEvent(eventDelAllBP);
}

void PhpPlugin::DoEnsureXDebugPanesVisible(const wxString& selectWindow)
{
    // Save the current layout to be the normal layout
    m_savedPerspective = m_mgr->GetDockingManager()->SavePerspective();
    m_debuggerPane->SelectTab(selectWindow);

    // If we have an old perspective, load it
    wxFileName fnConfig(clStandardPaths::Get().GetUserDataDir(), "xdebug-perspective");
    fnConfig.AppendDir("config");

    if (fnConfig.Exists()) {
        wxFFile fp(fnConfig.GetFullPath(), "rb");
        if (fp.IsOpened()) {
            wxString perspective;
            fp.ReadAll(&perspective);

            m_mgr->GetDockingManager()->LoadPerspective(perspective, false);
        }
    }

    EnsureAuiPaneIsVisible("XDebug");
    EnsureAuiPaneIsVisible("XDebugEval");
    EnsureAuiPaneIsVisible("XDebugLocals", true);
}

void PhpPlugin::SafelyDetachAndDestroyPane(wxWindow* pane, const wxString& name)
{
    if (pane) {
        wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(name);
        if (pi.IsOk()) {
            m_mgr->GetDockingManager()->DetachPane(pane);
            pane->Destroy();
        }
    }
}

void PhpPlugin::EnsureAuiPaneIsVisible(const wxString& paneName, bool update)
{
    wxAuiPaneInfo& pi = m_mgr->GetDockingManager()->GetPane(paneName);
    if (pi.IsOk() && !pi.IsShown()) {
        pi.Show();
    }
    if (update) {
        m_mgr->GetDockingManager()->Update();
    }
}

void PhpPlugin::RunXDebugDiagnostics()
{
    PHPXDebugSetupWizard wiz(EventNotifier::Get()->TopFrame());
    if (wiz.RunWizard(wiz.GetFirstPage())) {}
}

void PhpPlugin::OnRunXDebugDiagnostics(wxCommandEvent& e)
{
    wxUnusedVar(e);
    RunXDebugDiagnostics();
}

void PhpPlugin::FinalizeStartup()
{
    // Create the debugger windows (hidden)
    wxWindow* parent = m_mgr->GetDockingManager()->GetManagedWindow();
    m_debuggerPane = new PHPDebugPane(parent);
    m_mgr->GetDockingManager()->AddPane(m_debuggerPane,
                                        wxAuiPaneInfo()
                                            .Name("XDebug")
                                            .Caption("Call Stack & Breakpoints")
                                            .Hide()
                                            .CloseButton()
                                            .MaximizeButton()
                                            .Bottom()
                                            .Position(3));

    m_xdebugLocalsView = new LocalsView(parent);
    m_mgr->GetDockingManager()->AddPane(
        m_xdebugLocalsView,
        wxAuiPaneInfo().Name("XDebugLocals").Caption("Locals").Hide().CloseButton().MaximizeButton().Bottom());

    m_xdebugEvalPane = new EvalPane(parent);
    m_mgr->GetDockingManager()->AddPane(
        m_xdebugEvalPane,
        wxAuiPaneInfo().Name("XDebugEval").Caption("PHP").Hide().CloseButton().MaximizeButton().Bottom().Position(2));
}
