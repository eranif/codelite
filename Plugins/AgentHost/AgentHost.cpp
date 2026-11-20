#include "AgentHost.hpp"

#include "AgentHostSettgingsDlg.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "LaunchAIAgentDlg.hpp"
#include "Platform/Platform.hpp"
#include "clSideBarCtrl.hpp"
#include "globals.h"
#include "open_resource_dialog.h"

#include <vector>
#include <wx/choicdlg.h>

// The resource ID of the "Launch an AI Agent..." menu item: the tooltip of the side bar button shows its shortcut
namespace
{
const char* kLaunchAgentAction = "launch_agent";
const wxString kAgentWorkingDirectory = "WorkingDirectory";
const wxString kAgentHostLastAgent = "LastAgent";
const wxString kCategoryAgentHost = "AgentHost";

wxString GetNameFromType(AgentType type)
{
    wxString agent_name;
    switch (type) {
    case AgentType::kClaudeCode:
        agent_name = _("Claude Code");
        break;
    case AgentType::kKiroCli:
        agent_name = _("Kiro CLI");
        break;
    case AgentType::kOpenCode:
        agent_name = _("OpenCode");
        break;
    }
    return agent_name;
}
} // namespace

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
    m_showAgentsDialog = std::make_shared<std::function<void()>>([this]() { CallAfter(&AgentHost::ShowAgentsDialog); });
    m_mgr->GetLeftSideBarCtrl()->AddActionButton(
        "claude-code", _("Launch an AI Agent for the Current Workspace"), kLaunchAgentAction, m_showAgentsDialog);
    EventNotifier::Get()->Bind(wxEVT_KEYBOARD_SHORTCUTS_UPDATED, &AgentHost::OnShortcutsUpdated, this);
}

AgentHost::~AgentHost()
{
    EventNotifier::Get()->Unbind(wxEVT_KEYBOARD_SHORTCUTS_UPDATED, &AgentHost::OnShortcutsUpdated, this);
}

void AgentHost::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }
void AgentHost::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(nullptr);
    item = new wxMenuItem(menu,
                          XRCID(kLaunchAgentAction),
                          _("Launch an AI Agent...\tCtrl-Shift-C"),
                          _("Select the agent to launch for the current workspace"),
                          wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();

    item = new wxMenuItem(menu, XRCID("launch_claude_code"), _("Launch Claude Code"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    item = new wxMenuItem(menu, XRCID("launch_kiro_cli"), _("Launch Kiro Cli"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("launch_open_code"), _("Launch OpenCode"), _("Launch OpenCode"), wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("agent_host_settings"), _("Settings..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Agent Host"), menu);
    menu->Bind(wxEVT_MENU, &AgentHost::OnSettings, this, XRCID("agent_host_settings"));
    menu->Bind(wxEVT_MENU, &AgentHost::OnLaunchAgent, this, XRCID(kLaunchAgentAction));
    menu->Bind(wxEVT_MENU, &AgentHost::OnShowClaudeCode, this, XRCID("launch_claude_code"));
    menu->Bind(wxEVT_MENU, &AgentHost::OnShowKiroCli, this, XRCID("launch_kiro_cli"));
    menu->Bind(wxEVT_MENU, &AgentHost::OnShowOpenCode, this, XRCID("launch_open_code"));

    clKeyboardManager::Get()->AddAccelerator(_("Agent Host"),
                                             {
                                                 {kLaunchAgentAction, _("Launch an AI Agent..."), "Ctrl-Shift-C"},
                                                 {"launch_claude_code", _("Launch Claude Code"), ""},
                                                 {"launch_kiro_cli", _("Launch Kiro Cli"), ""},
                                                 {"launch_open_code", _("Launch OpenCode"), ""},
                                                 {"agent_host_settings", _("Options...")},
                                             });

    // The button was created before the shortcut was known
    if (auto sidebar = m_mgr->GetLeftSideBarCtrl()) {
        sidebar->UpdateActionButtonTooltip(kLaunchAgentAction);
    }
}

void AgentHost::UnPlug() {}

void AgentHost::ShowAgentsDialog()
{
    auto* workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr) {
        wxMessageBox(_("Launching an AI agent requires a workspace"), "CodeLite", wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }

    std::vector<AgentType> agents{
        AgentType::kClaudeCode,
        AgentType::kKiroCli,
        AgentType::kOpenCode,
    };

    // Suggest the agent that the user launched last (if it is installed and not running). If there is none, suggest the
    // first agent that is installed and not running.
    const int lastAgentUsed = workspace->GetConfigValue(kAgentHostLastAgent, kCategoryAgentHost, wxNOT_FOUND);
    int suggested{wxNOT_FOUND};
    wxArrayString choices;
    for (size_t i = 0; i < agents.size(); ++i) {
        wxString label = GetNameFromType(agents[i]);
        if (FindByType(agents[i]) != nullptr) {
            label << " (" << _("running") << ")";
        } else if (!ResolveAgentExecutable(agents[i], false)) {
            label << " (" << _("not found") << ")";
        } else if (suggested == wxNOT_FOUND || lastAgentUsed == static_cast<int>(agents[i])) {
            suggested = static_cast<int>(i);
        }
        choices.Add(label);
    }

    wxString workingDirectory =
        workspace->GetConfigValue(kAgentWorkingDirectory, kCategoryAgentHost, workspace->GetDir());
    LaunchAIAgentDlg dlg(EventNotifier::Get()->TopFrame(), choices, suggested, workingDirectory);
    if (dlg.ShowModal() != wxID_OK || dlg.GetAgent() == wxNOT_FOUND) {
        return;
    }
    const auto type = agents[dlg.GetAgent()];

    // Store the workspace settings.
    workspace->SetConfigValue(kAgentWorkingDirectory, kCategoryAgentHost, dlg.GetWorkingDirectory());
    workspace->SetConfigValue(kAgentHostLastAgent, kCategoryAgentHost, static_cast<int>(type));
    auto result = workspace->SaveConfig();
    if (!result.ok()) {
        clWARNING() << "Failed to save workspace config file." << result.message() << endl;
    }

    ShowAgentTerminal(type, dlg.GetWorkingDirectory());
}

void AgentHost::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    AgentHostSettgingsDlg dlg{EventNotifier::Get()->TopFrame()};
    dlg.ShowModal();
}

std::optional<AgentExecutable> ResolveAgentExecutable(AgentType agent_type, bool warn)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr) {
        return std::nullopt;
    }
    std::optional<wxString> tool_executable{std::nullopt};
    std::optional<SSHAccountInfo> sshAccount{std::nullopt};

    wxString defaultExec;
    wxString configParam;
    wxString argsParam;
    switch (agent_type) {
    case AgentType::kClaudeCode:
        defaultExec = "claude";
        configParam = kAgentHostClaudeCodeExecutable;
        argsParam = kAgentHostClaudeCodeArgs;
        break;
    case AgentType::kKiroCli:
        defaultExec = "kiro-cli";
        configParam = kAgentHostKiroCliExecutable;
        argsParam = kAgentHostKiroCliArgs;
        break;
    case AgentType::kOpenCode:
        defaultExec = "opencode";
        configParam = kAgentHostOpenCodeExecutable;
        argsParam = kAgentHostOpenCodeArgs;
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
            if (warn) {
                wxMessageBox(wxString::Format(_("Could not locate '%s' executable"), defaultExec),
                             "CodeLite",
                             wxICON_WARNING | wxOK | wxOK_DEFAULT);
            }
            return std::nullopt;
        }
    }

    wxString args = clConfig::Get().Read(argsParam, wxString{});
    args.Trim().Trim(false);

    return AgentExecutable{
        .executable = tool_executable.value(),
        .args = args,
        .sshAccount = sshAccount,
    };
}

void AgentHost::ShowAgentTerminal(AgentType agent_type, const wxString& workingDirectory)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr) {
        wxMessageBox(wxString::Format(_("Launching %s requires a workspace"), GetNameFromType(agent_type)),
                     "CodeLite",
                     wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }

    auto* agent_page = FindByType(agent_type);
    if (agent_page) {
        if (!clGetManager()->SelectPage(agent_page)) {
            clWARNING() << "Could not select agent page!" << endl;
        }
        return;
    }

    auto result = ResolveAgentExecutable(agent_type);
    if (!result) {
        // ResolveAgentExecutable already prompts with an error message.
        return;
    }

    // Define the working directory & the ssh account (if a remote workspace)
    auto agentPage = new AgentHostPage(clGetManager()->GetMainNotebook());
    clGetManager()->GetMainNotebook()->AddPage(agentPage, GetNameFromType(agent_type), true);
    agentPage->StartAgentHost(AgentInfo{
        .agent_type = agent_type,
        .executable = result->executable,
        .args = result->args,
        .workingDirectory =
            workingDirectory.empty() ? clWorkspaceManager::Get().GetWorkspace()->GetDir() : workingDirectory,
        .sshAccount = result->sshAccount,
    });
}

void AgentHost::OnShortcutsUpdated(clCommandEvent& event)
{
    event.Skip();
    auto sidebar = m_mgr->GetLeftSideBarCtrl();
    CHECK_PTR_RET(sidebar);
    sidebar->UpdateActionButtonTooltip(kLaunchAgentAction);
}

void AgentHost::OnLaunchAgent(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ShowAgentsDialog();
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

void AgentHost::OnShowOpenCode(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ShowAgentTerminal(AgentType::kOpenCode);
}

AgentHostPage* AgentHost::FindByType(AgentType type) const
{
    auto* book = clGetManager()->GetMainNotebook();
    for (size_t i = 0; i < book->GetPageCount(); ++i) {
        if (auto* page = dynamic_cast<AgentHostPage*>(book->GetPage(i));
            page != nullptr && page->GetInfo().agent_type == type) {
            return page;
        }
    }
    return nullptr;
}
