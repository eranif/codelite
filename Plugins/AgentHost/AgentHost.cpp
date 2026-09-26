#include "AgentHost.hpp"

#include "AgentHostSettgingsDlg.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "Platform/Platform.hpp"
#include "clSideBarCtrl.hpp"
#include "globals.h"
#include "open_resource_dialog.h"

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager) { return new AgentHost(manager); }

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("AgentHost"));
    info.SetDescription(_("Integrate Claude Code & Kiro CLI into CodeLite."));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

AgentHost::AgentHost(IManager* manager)
    : IPlugin(manager)
{
    m_showClaudeCode = std::make_shared<std::function<void()>>(
        [this]() { CallAfter(&AgentHost::ShowAgentTerminal, AgentType::kClaudeCode); });
    m_showKiroCli = std::make_shared<std::function<void()>>(
        [this]() { CallAfter(&AgentHost::ShowAgentTerminal, AgentType::kKiroCli); });
    m_mgr->GetLeftSideBarCtrl()->AddActionButton(
        "claude-code", _("Launch Claude Code for the Current Workspace"), m_showClaudeCode);
    m_mgr->GetLeftSideBarCtrl()->AddActionButton("kiro", _("Launch Kiro Cli for the Current Workspace"), m_showKiroCli);
    EventNotifier::Get()->Bind(wxEVT_NOTIFY_PAGE_CLOSING, &AgentHost::OnPageClosing, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &AgentHost::OnAllPagesClosed, this);
}

AgentHost::~AgentHost()
{
    EventNotifier::Get()->Unbind(wxEVT_NOTIFY_PAGE_CLOSING, &AgentHost::OnPageClosing, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &AgentHost::OnAllPagesClosed, this);
}

void AgentHost::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }
void AgentHost::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(nullptr);
    item = new wxMenuItem(menu,
                          XRCID("launch_claude_code"),
                          _("Launch Claude Code\tCtrl-Shift-C"),
                          _("Launch Claude Code"),
                          wxITEM_NORMAL);
    menu->Append(item);
    item = new wxMenuItem(
        menu, XRCID("launch_kiro_cli"), _("Launch Kiro Cli\tCtrl-Shift-K"), _("Launch Kiro Cli"), wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("agent_host_settings"), _("Settings..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Agent Host"), menu);
    menu->Bind(wxEVT_MENU, &AgentHost::OnSettings, this, XRCID("agent_host_settings"));
    menu->Bind(wxEVT_MENU, &AgentHost::OnShowClaudeCode, this, XRCID("launch_claude_code"));
    menu->Bind(wxEVT_MENU, &AgentHost::OnShowKiroCli, this, XRCID("launch_kiro_cli"));

    clKeyboardManager::Get()->AddAccelerator(_("Agent Host"),
                                             {
                                                 {"launch_claude_code", _("Launch Claude Code"), "Ctrl-Shift-C"},
                                                 {"launch_kiro_cli", _("Launch Kiro Cli"), "Ctrl-Shift-K"},
                                                 {"agent_host_settings", _("Options...")},
                                             });
}

void AgentHost::UnPlug() {}
void AgentHost::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    AgentHostSettgingsDlg dlg{EventNotifier::Get()->TopFrame()};
    if (dlg.ShowModal() == wxID_OK) {
        auto claude_exec = dlg.GetClaudeCodeExecutable();
        clConfig::Get().Write(kAgentHostClaudeCodeExecutable, claude_exec);
    }
}

namespace
{
struct ExecResult {
    std::optional<wxString> executable{std::nullopt};
    std::optional<SSHAccountInfo> ssh_account{std::nullopt};
};

std::optional<ExecResult> ResolveExecutable(AgentType agent_type)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    std::optional<wxString> tool_executable{std::nullopt};
    std::optional<SSHAccountInfo> sshAccount{std::nullopt};

    wxString defaultExec;
    wxString configParam;
    switch (agent_type) {
    case AgentType::kClaudeCode:
        defaultExec = "claude";
        configParam = kAgentHostClaudeCodeExecutable;
        break;
    case AgentType::kKiroCli:
        defaultExec = "kiro-cli";
        configParam = kAgentHostKiroCliExecutable;
        break;
    }
    if (workspace->IsRemote()) {
        // On remote machines, always use the claude executable defined by the PATH
        tool_executable = defaultExec;
        sshAccount = SSHAccountInfo::FindAccount(workspace->GetSshAccount());
    } else {
        // On local executions, use the configured claude executable first if one is not set, locate using
        // the environment variables.
        auto configured_tool_executable = clConfig::Get().Read(configParam, wxString{});
        if (configured_tool_executable.empty())
            tool_executable = ThePlatform->Which(defaultExec);
        else
            tool_executable = configured_tool_executable;

        if (!tool_executable) {
            wxMessageBox(wxString::Format(_("Could not locate '%s' executable"), defaultExec),
                         "CodeLite",
                         wxICON_WARNING | wxOK | wxOK_DEFAULT);
            return std::nullopt;
        }
    }

    return ExecResult{
        .executable = tool_executable,
        .ssh_account = sshAccount,
    };
}

} // namespace
void AgentHost::ShowAgentTerminal(AgentType agent_type)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr) {
        wxMessageBox(_("Launching Claude Code requires a workspace"), "CodeLite", wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }

    auto iter = m_pages.find(agent_type);
    if (iter != m_pages.end()) {
        if (!clGetManager()->SelectPage(iter->second)) {
            clWARNING() << "Could not select agent page!" << endl;
        }
        return;
    }

    auto result = ResolveExecutable(agent_type);
    if (!result) {
        // ResolveExecutable already prompts with an error message.
        return;
    }

    // Define the working directory & the ssh account (if a remote workspace)
    auto agentPage = new AgentHostPage(clGetManager()->GetMainNotebook());
    CHECK_PTR_RET(agentPage);

    m_pages[agent_type] = agentPage;
    switch (agent_type) {
    case AgentType::kClaudeCode:
        clGetManager()->GetMainNotebook()->AddPage(agentPage, _("Claude Code"), true);
        break;
    case AgentType::kKiroCli:
        clGetManager()->GetMainNotebook()->AddPage(agentPage, _("Kiro"), true);
        break;
    }
    agentPage->StartAgentHost(AgentInfo{
        .agent_type = agent_type,
        .executable = result->executable.value(),
        .workingDirectory = clWorkspaceManager::Get().GetWorkspace()->GetDir(),
        .sshAccount = result->ssh_account,
    });
}

void AgentHost::OnPageClosing(wxNotifyEvent& event)
{
    const wxWindow* win = reinterpret_cast<wxWindow*>(event.GetClientData());
    if (win) {
        for (const auto& [agent_type, agent_page] : m_pages) {
            if (win == agent_page) {
                m_pages.erase(agent_type);
                return;
            }
        }
    }
    event.Skip();
}

void AgentHost::OnAllPagesClosed(wxCommandEvent& event)
{
    m_pages.clear();
    event.Skip();
}

void AgentHost::OnShowClaudeCode(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ShowAgentTerminal(AgentType::kClaudeCode);
}

void AgentHost::OnShowKiroCli(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ShowAgentTerminal(AgentType::kKiroCli);
}
