#include "ClaudeCode.hpp"

#include "ClaudeCodeSettgingsDlg.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "Platform/Platform.hpp"
#include "clSideBarCtrl.hpp"
#include "globals.h"

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

    m_blinkTimer.SetOwner(this, XRCID("claude_code_blink_timer"));
    Bind(wxEVT_TIMER, &ClaudeCode::OnBlinkTimer, this, XRCID("claude_code_blink_timer"));
}

ClaudeCode::~ClaudeCode()
{
    m_blinkTimer.Stop();
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
    CHECK_PTR_RET(workspace);

    if (m_claudeTerminal) {
        if (!clGetManager()->SelectPage(m_claudeTerminal)) {
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
    std::optional<wxString> wd{std::nullopt};

    if (workspace) {
        wd = workspace->GetDir();
        if (workspace->IsRemote()) {
            sshAccount = SSHAccountInfo::FindAccount(workspace->GetSshAccount());
        }
    }

#ifdef __WXMSW__
    const wxString kShellCommand = "CMD";
    const wxString kShellTitle = wxT("🤖 Claude Code");
#else
    const wxString kShellCommand = "/bin/bash --login -i";
    const wxString kShellTitle = wxEmptyString;
#endif

    m_claudeTerminal = clGetManager()->GetTerminalManager()->OpenNewTerminalTab(
        wd.value(), sshAccount, kShellTitle, true, kShellCommand, clGetManager()->GetMainNotebook());

    CHECK_PTR_RET(m_claudeTerminal);

    // Remember the label given to the tab, the blink code needs it.
    auto book = clGetManager()->GetMainNotebook();
    int page_index = book->FindPage(m_claudeTerminal);
    m_tabTitle = page_index == wxNOT_FOUND ? _("Claude Code") : book->GetPageText(page_index);

    m_claudeTerminal->Bind(wxEVT_TERMINAL_TITLE_CHANGED, [this](wxTerminalEvent& event) {
        wxString new_title = event.GetTitle();
        new_title.Trim().Trim(false);
        if (new_title.empty()) {
            new_title = _("Terminal");
        }
        m_tabTitle = new_title;
        UpdateTabLabel();
    });

    m_claudeTerminal->Bind(wxEVT_TERMINAL_BELL, &ClaudeCode::OnTerminalBell, this);
    m_claudeTerminal->Bind(wxEVT_SET_FOCUS, &ClaudeCode::OnTerminalFocus, this);

    m_claudeTerminal->Bind(wxEVT_TERMINAL_TERMINATED, [this](wxTerminalEvent& event) {
        StopAttentionBlink();
        m_claudeTerminal = nullptr;
        wxUnusedVar(event);
    });

    m_claudeTerminal->Bind(wxEVT_TERMINAL_TEXT_LINK, &ClaudeCode::OnTerminalLink, this);
    claude_exec.value().Prepend("\"").Append("\"");
    wxString command_to_run = wxString::Format("%s --continue || %s", *claude_exec, *claude_exec);
    m_claudeTerminal->SendCommand(command_to_run);
}

void ClaudeCode::OnPageClosing(wxNotifyEvent& event)
{
    const wxWindow* win = reinterpret_cast<wxWindow*>(event.GetClientData());
    if (win && win == m_claudeTerminal) {
        StopAttentionBlink();
        m_claudeTerminal = nullptr;
        return;
    }
    event.Skip();
}

void ClaudeCode::OnAllPagesClosed(wxCommandEvent& event)
{
    StopAttentionBlink();
    m_claudeTerminal = nullptr;
    event.Skip();
}

void ClaudeCode::OnShowClaudeCode(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ShowClaudeTerminal();
}

void ClaudeCode::OnPageChanged(wxCommandEvent& event)
{
    event.Skip();
    const wxWindow* win = reinterpret_cast<wxWindow*>(event.GetClientData());
    if (win && win == m_claudeTerminal) {
        // The user switched to the Claude Code tab, the attention state is no longer needed.
        StopAttentionBlink();
    }
}

void ClaudeCode::OnTerminalFocus(wxFocusEvent& event)
{
    event.Skip();
    StopAttentionBlink();
}

void ClaudeCode::OnTerminalLink(wxTerminalEvent& event)
{
    clGetManager()->GetTerminalManager()->OpenLink(event.GetClickedText());
}

void ClaudeCode::OnTerminalBell(wxTerminalEvent& event)
{
    wxUnusedVar(event);
    if (IsClaudeTerminalVisible()) {
        // The user is already looking at the terminal, no need to blink.
        return;
    }
    StartAttentionBlink();
}

void ClaudeCode::OnBlinkTimer(wxTimerEvent& event)
{
    wxUnusedVar(event);
    if (!m_needsAttention || !m_claudeTerminal) {
        StopAttentionBlink();
        return;
    }
    m_blinkOn = !m_blinkOn;
    UpdateTabLabel();
}

void ClaudeCode::StartAttentionBlink()
{
    CHECK_PTR_RET(m_claudeTerminal);
    if (m_needsAttention) {
        // Already blinking.
        return;
    }
    m_needsAttention = true;
    m_blinkOn = true;
    UpdateTabLabel();
    m_blinkTimer.Start(kBlinkIntervalMs);
}

void ClaudeCode::StopAttentionBlink()
{
    m_blinkTimer.Stop();
    if (!m_needsAttention) {
        return;
    }
    m_needsAttention = false;
    m_blinkOn = false;
    UpdateTabLabel();
}

void ClaudeCode::UpdateTabLabel()
{
    CHECK_PTR_RET(m_claudeTerminal);
    auto book = clGetManager()->GetMainNotebook();
    int index = book->FindPage(m_claudeTerminal);
    if (index == wxNOT_FOUND) {
        return;
    }

    wxString label = m_tabTitle;
    if (m_needsAttention) {
        label.Prepend(m_blinkOn ? kAttentionMarkerOn : kAttentionMarkerOff);
    }
    book->SetPageText(index, label);
}

bool ClaudeCode::IsClaudeTerminalVisible() const
{
    if (!m_claudeTerminal) {
        return false;
    }

    auto book = clGetManager()->GetMainNotebook();
    int index = book->FindPage(m_claudeTerminal);
    if (index == wxNOT_FOUND || index != book->GetSelection()) {
        return false;
    }

    auto* frame = EventNotifier::Get()->TopFrame();
    return frame && frame->IsActive();
}
