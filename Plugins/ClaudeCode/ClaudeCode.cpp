#include "ClaudeCode.hpp"

#include "ClaudeCodeSettgingsDlg.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "Platform/Platform.hpp"
#include "clSideBarCtrl.hpp"
#include "globals.h"
#include "open_resource_dialog.h"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

namespace
{
/// While Claude Code waits for the user, the tab label alternates between these two markers.
const wxString kAttentionMarkerOn = wxT("● ");  // black circle
const wxString kAttentionMarkerOff = wxT("○ "); // white circle
constexpr int kBlinkIntervalMs = 500;
} // namespace

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager) { return new ClaudeCode(manager); }

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("ClaudeCode"));
    info.SetDescription(_("Claude Code Integration"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

ClaudeCode::ClaudeCode(IManager* manager)
    : IPlugin(manager)
{
    m_showClaudeCode =
        std::make_shared<std::function<void()>>([this]() { CallAfter(&ClaudeCode::ShowClaudeTerminal); });
    m_mgr->GetLeftSideBarCtrl()->AddActionButton(
        "claude-code", _("Launch Claude Code for the Current Workspace"), m_showClaudeCode);
    EventNotifier::Get()->Bind(wxEVT_NOTIFY_PAGE_CLOSING, &ClaudeCode::OnPageClosing, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &ClaudeCode::OnAllPagesClosed, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &ClaudeCode::OnPageChanged, this);
}

ClaudeCode::~ClaudeCode()
{
    EventNotifier::Get()->Unbind(wxEVT_NOTIFY_PAGE_CLOSING, &ClaudeCode::OnPageClosing, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &ClaudeCode::OnAllPagesClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &ClaudeCode::OnPageChanged, this);
}

void ClaudeCode::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }
void ClaudeCode::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu,
                          XRCID("launch_claude_code"),
                          _("Launch Claude Code\tCtrl-Shift-I"),
                          _("Launch Claude Code"),
                          wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("claude_code_options"), _("Options..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Claude Code"), menu);
    menu->Bind(wxEVT_MENU, &ClaudeCode::OnSettings, this, XRCID("claude_code_options"));
    menu->Bind(wxEVT_MENU, &ClaudeCode::OnShowClaudeCode, this, XRCID("launch_claude_code"));

    clKeyboardManager::Get()->AddAccelerator(
        _("Claude Code"),
        {{"launch_claude_code", _("Launch Claude Code"), "Ctrl-Shift-I"}, {"claude_code_options", _("Options...")}});
}

void ClaudeCode::UnPlug() {}
void ClaudeCode::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ClaudeCodeSettgingsDlg dlg{EventNotifier::Get()->TopFrame()};
    if (dlg.ShowModal() == wxID_OK) {
        auto claude_exec = dlg.GetClaudeCode();
        clConfig::Get().Write(kClaudeCodeExecutable, claude_exec);
    }
}

void ClaudeCode::ShowClaudeTerminal()
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr) {
        wxMessageBox(_("Launching Claude Code requires a workspace"), "CodeLite", wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }

    if (m_claudeCodePage) {
        if (!clGetManager()->SelectPage(m_claudeCodePage)) {
            clWARNING() << "Could not select claude tab window" << endl;
        }
        return;
    }

    std::optional<wxString> claude_exec{std::nullopt};
    if (workspace->IsRemote())
        // On remote machines, always use the claude executable defined by the PATH
        claude_exec = "claude";
    else {
        // On local executions, use the configured claude executable first if one is not set, locate using
        // the environment variables.
        auto configured_claude_exec = clConfig::Get().Read(kClaudeCodeExecutable, wxString{});
        if (configured_claude_exec.empty())
            claude_exec = ThePlatform->Which("claude");
        else
            claude_exec = configured_claude_exec;

        if (!claude_exec) {
            wxMessageBox(_("Could not locate claude executable"), "CodeLite", wxICON_WARNING | wxOK | wxOK_DEFAULT);
            return;
        }
    }

    // Define the working directory & the ssh account (if a remote workspace)
    std::optional<SSHAccountInfo> sshAccount{std::nullopt};
    m_claudeCodePage = new ClaudeCodePage(clGetManager()->GetMainNotebook(), sshAccount);
    clGetManager()->GetMainNotebook()->AddPage(m_claudeCodePage, _("Claude Code"), true);
    CHECK_PTR_RET(m_claudeCodePage);

    wxString workdingDirectory;
    workdingDirectory = workspace->GetDir();
    if (workspace->IsRemote()) {
        sshAccount = SSHAccountInfo::FindAccount(workspace->GetSshAccount());
    }
    m_claudeCodePage->StartClaudeCode(*claude_exec, workdingDirectory);
}

void ClaudeCode::OnPageClosing(wxNotifyEvent& event)
{
    const wxWindow* win = reinterpret_cast<wxWindow*>(event.GetClientData());
    if (win && win == m_claudeCodePage) {
        m_claudeCodePage = nullptr;
        return;
    }
    event.Skip();
}

void ClaudeCode::OnAllPagesClosed(wxCommandEvent& event)
{
    m_claudeCodePage = nullptr;
    event.Skip();
}

void ClaudeCode::OnShowClaudeCode(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ShowClaudeTerminal();
}

void ClaudeCode::OnPageChanged(wxCommandEvent& event) { event.Skip(); }

bool ClaudeCode::IsClaudeTerminalVisible() const
{
    if (!m_claudeCodePage) {
        return false;
    }

    auto book = clGetManager()->GetMainNotebook();
    int index = book->FindPage(m_claudeCodePage);
    if (index == wxNOT_FOUND || index != book->GetSelection()) {
        return false;
    }

    auto* frame = EventNotifier::Get()->TopFrame();
    return frame && frame->IsActive();
}
