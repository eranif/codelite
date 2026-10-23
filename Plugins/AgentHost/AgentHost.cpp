#include "AgentHost.hpp"

#include "AgentHostSettgingsDlg.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "Platform/Platform.hpp"
#include "clSideBarCtrl.hpp"
#include "globals.h"
#include "open_resource_dialog.h"

#include <iterator>
#include <wx/choicdlg.h>

// The resource ID of the "Launch an AI Agent..." menu item: the tooltip of the side bar button shows its shortcut
static const char* kLaunchAgentAction = "launch_agent";

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
    EventNotifier::Get()->Bind(wxEVT_NOTIFY_PAGE_CLOSING, &AgentHost::OnPageClosing, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &AgentHost::OnAllPagesClosed, this);
    EventNotifier::Get()->Bind(wxEVT_KEYBOARD_SHORTCUTS_UPDATED, &AgentHost::OnShortcutsUpdated, this);
}

AgentHost::~AgentHost()
{
    EventNotifier::Get()->Unbind(wxEVT_KEYBOARD_SHORTCUTS_UPDATED, &AgentHost::OnShortcutsUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_NOTIFY_PAGE_CLOSING, &AgentHost::OnPageClosing, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &AgentHost::OnAllPagesClosed, this);
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
    const struct {
        wxString label;
        AgentType type;
    } agents[] = {
        {_("Claude Code"), AgentType::kClaudeCode},
        {_("Kiro CLI"), AgentType::kKiroCli},
        {_("OpenCode"), AgentType::kOpenCode},
    };

    // Suggest the agent that the user launched last. If there is none, suggest the first agent that is installed.
    const int lastAgent = clConfig::Get().Read(kAgentHostLastAgent, -1);
    int suggested = -1;
    wxArrayString choices;
    for (size_t i = 0; i < std::size(agents); ++i) {
        const auto& agent = agents[i];
        wxString label = agent.label;
        if (m_pages.count(agent.type) != 0) {
            label << " (" << _("running") << ")";
        } else if (!ResolveAgentExecutable(agent.type, false)) {
            label << " (" << _("not found") << ")";
        } else if (suggested == -1 || lastAgent == static_cast<int>(agent.type)) {
            suggested = static_cast<int>(i);
        }
        choices.Add(label);
    }

    wxSingleChoiceDialog dlg(EventNotifier::Get()->TopFrame(),
                             _("Select the agent to launch for the current workspace"),
                             _("Launch an AI Agent"),
                             choices);
    dlg.SetSelection(suggested == -1 ? 0 : suggested);
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    const auto type = agents[dlg.GetSelection()].type;
    clConfig::Get().Write(kAgentHostLastAgent, static_cast<int>(type));
    ShowAgentTerminal(type);
}

void AgentHost::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    AgentHostSettgingsDlg dlg{EventNotifier::Get()->TopFrame()};
    if (dlg.ShowModal() == wxID_OK) {
        clConfig::Get().Write(kAgentHostClaudeCodeExecutable, dlg.GetClaudeCodeExecutable());
        clConfig::Get().Write(kAgentHostKiroCliExecutable, dlg.GetKiroCliExecutable());
        clConfig::Get().Write(kAgentHostOpenCodeExecutable, dlg.GetOpenCodeExecutable());
        clConfig::Get().Write(kAgentHostClaudeCodeReviewArgs, dlg.GetClaudeCodeReviewArgs());
        clConfig::Get().Write(kAgentHostKiroCliReviewArgs, dlg.GetKiroCliReviewArgs());
        clConfig::Get().Write(kAgentHostOpenCodeReviewArgs, dlg.GetOpenCodeReviewArgs());
    }
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
    switch (agent_type) {
    case AgentType::kClaudeCode:
        defaultExec = "claude";
        configParam = kAgentHostClaudeCodeExecutable;
        break;
    case AgentType::kKiroCli:
        defaultExec = "kiro-cli";
        configParam = kAgentHostKiroCliExecutable;
        break;
    case AgentType::kOpenCode:
        defaultExec = "opencode";
        configParam = kAgentHostOpenCodeExecutable;
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

    return AgentExecutable{
        .executable = tool_executable.value(),
        .sshAccount = sshAccount,
    };
}

void AgentHost::ShowAgentTerminal(AgentType agent_type)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr) {
        wxString agent_name;
        switch (agent_type) {
        case AgentType::kClaudeCode:
            agent_name = "Claude Code";
            break;
        case AgentType::kKiroCli:
            agent_name = "Kiro CLI";
            break;
        case AgentType::kOpenCode:
            agent_name = "OpenCode";
            break;
        }
        wxMessageBox(wxString::Format(_("Launching %s requires a workspace"), agent_name),
                     "CodeLite",
                     wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }

    auto iter = m_pages.find(agent_type);
    if (iter != m_pages.end()) {
        if (!clGetManager()->SelectPage(iter->second)) {
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
    CHECK_PTR_RET(agentPage);

    m_pages[agent_type] = agentPage;
    switch (agent_type) {
    case AgentType::kClaudeCode:
        clGetManager()->GetMainNotebook()->AddPage(agentPage, _("Claude Code"), true);
        break;
    case AgentType::kKiroCli:
        clGetManager()->GetMainNotebook()->AddPage(agentPage, _("Kiro"), true);
        break;
    case AgentType::kOpenCode:
        clGetManager()->GetMainNotebook()->AddPage(agentPage, _("OpenCode"), true);
        break;
    }
    agentPage->StartAgentHost(AgentInfo{
        .agent_type = agent_type,
        .executable = result->executable,
        .workingDirectory = clWorkspaceManager::Get().GetWorkspace()->GetDir(),
        .sshAccount = result->sshAccount,
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
