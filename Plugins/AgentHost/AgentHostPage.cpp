#include "AgentHostPage.hpp"

#include "AgentHostSettgingsDlg.hpp"
#include "AgentTerminal.hpp"
#include "ColoursAndFontsManager.h"
#include "FileManager.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "ReviewBuddy.hpp"
#include "TabAttentionIcon.hpp"
#include "ai/LLMManager.hpp"
#include "aui/clAuiToolBarArt.h"
#include "aui/cl_aui_tool_stickness.h"
#include "bitmap_loader.h"
#include "clFileSystemEvent.h"
#include "clStrings.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "environmentconfig.h"
#include "globals.h"
#include "open_resource_dialog.h"
#include "search_thread.h"
#include "terminal_view.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

#include <utility>
#include <wx/app.h>
#include <wx/sizer.h>
#include <wx/toplevel.h>
#include <wx/xrc/xmlres.h>

namespace
{
#ifdef __WXMSW__
const wxString kShellCommand = "CMD";
#else
const wxString kShellCommand = "/bin/bash --login -i";
#endif

// The toolbar buttons (the names are turned into IDs with XRCID)
constexpr const char* kToolRestart = "agent_host_restart";
constexpr const char* kToolReviewBuddy = "agent_host_review_buddy";
constexpr const char* kToolFindInFiles = "agent_host_find_in_files";
constexpr const char* kToolOpenResource = "agent_host_open_resource";
constexpr const char* kToolCopy = "agent_host_copy";
constexpr const char* kToolPaste = "agent_host_paste";

/// `text` followed by the keyboard shortcut that is assigned to the command `command` in CodeLite, on its own row.
/// No shortcut is added when `command` is nullptr (the button has no command) or no shortcut is assigned to it.
wxString WithShortcut(const wxString& text, const char* command)
{
    if (command == nullptr) {
        return text;
    }
    const auto shortcut = clKeyboardManager::Get()->GetShortcutForCommand(wxString(command));
    return shortcut.IsOk() ? text + "\n" + shortcut.DisplayString() : text;
}
} // namespace

#define CHECK_CAN_HANDLE_EVENT(event)              \
    if (event.GetEventObject() != GetTerminal()) { \
        event.Skip();                              \
        return;                                    \
    }

AgentHostPage::AgentHostPage(wxBookCtrlBase* parent)
    : AgentHostPageBase(parent)
    , m_book(parent)
{
#ifndef __WXMSW__
    // Clicking on the tab label with the mouse does not move the focus to the page (GTK keeps it on the tab),
    // so we set the focus on the terminal when our page becomes the selected one.
    m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &AgentHostPage::OnBookPageChanged, this);
#endif

    // The toolbar. The tools are created by BuildToolbar() (again on a theme change), the events are bound once.
    m_auibar->Bind(wxEVT_MENU, &AgentHostPage::OnToolRestart, this, XRCID(kToolRestart));
    m_auibar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &AgentHostPage::OnToolReviewBuddy, this, XRCID(kToolReviewBuddy));
    m_auibar->Bind(wxEVT_MENU, &AgentHostPage::OnToolCopy, this, XRCID(kToolCopy));
    m_auibar->Bind(wxEVT_MENU, &AgentHostPage::OnToolPaste, this, XRCID(kToolPaste));
    m_auibar->Bind(wxEVT_UPDATE_UI, &AgentHostPage::OnToolCopyUI, this, XRCID(kToolCopy));
    m_auibar->Bind(wxEVT_UPDATE_UI, &AgentHostPage::OnToolHasTerminalUI, this, XRCID(kToolPaste));
    m_auibar->Bind(wxEVT_MENU, &AgentHostPage::OnToolFindInFiles, this, XRCID(kToolFindInFiles));
    m_auibar->Bind(wxEVT_MENU, &AgentHostPage::OnToolOpenResource, this, XRCID(kToolOpenResource));
    m_auibar->Bind(wxEVT_UPDATE_UI, &AgentHostPage::OnToolHasTerminalUI, this, XRCID(kToolRestart));
    m_auibar->Bind(wxEVT_UPDATE_UI, &AgentHostPage::OnToolHasTerminalUI, this, XRCID(kToolReviewBuddy));
    m_auibar->Bind(wxEVT_UPDATE_UI, &AgentHostPage::OnToolSelectionUI, this, XRCID(kToolFindInFiles));
    m_auibar->Bind(wxEVT_UPDATE_UI, &AgentHostPage::OnToolSelectionUI, this, XRCID(kToolOpenResource));

    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, &AgentHostPage::OnBitmapsUpdated, this);
    BuildToolbar();

    // A notice from the review buddy (hidden until there is something to say)
    m_infoBar = new wxInfoBar(this);
    GetSizer()->Add(m_infoBar, wxSizerFlags().Expand());

    // The main agent fills the page. The reviewer, when there is one, opens in a pane next to it.
    m_splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_3DSASH);
    m_splitter->SetMinimumPaneSize(FromDIP(150));
    m_splitter->SetSashGravity(0.5);
    GetSizer()->Add(m_splitter, wxSizerFlags(1).Expand());

    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TEXT_LINK_CLICKED, &AgentHostPage::OnTerminalLink, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TERMINATED, &AgentHostPage::OnTerminalTerminated, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_TITLE_CHANGED, &AgentHostPage::OnTerminalTitleChanged, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_BELL, &AgentHostPage::OnTerminalBell, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_PROGRAM_STATUS, &AgentHostPage::OnTerminalProgramStatus, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &AgentHostPage::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_FINDINFILES_DLG_SHOWING, &AgentHostPage::OnFindInFilesShowing, this);
    EventNotifier::Get()->Bind(wxEVT_OPEN_RESOURCE_SHOWING, &AgentHostPage::OnOpenResourceShowing, this);
    if (auto frame = EventNotifier::Get()->TopFrame()) {
        frame->Bind(wxEVT_MENU, &AgentHostPage::OnShowTerminal, this, XRCID("show_terminal_pane"));
    }
}

AgentHostPage::~AgentHostPage()
{
    // The review folder is deleted with ReviewBuddy only after its terminal is asked to close. On Windows the
    // process may still hold the directory briefly; ReviewBuddy logs and leaves it if removal then fails.
    DestroyReviewPane();
    m_review.reset();
#ifndef __WXMSW__
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &AgentHostPage::OnBookPageChanged, this);
#endif
    EventNotifier::Get()->Unbind(wxEVT_BITMAPS_UPDATED, &AgentHostPage::OnBitmapsUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &AgentHostPage::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TEXT_LINK_CLICKED, &AgentHostPage::OnTerminalLink, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TERMINATED, &AgentHostPage::OnTerminalTerminated, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TITLE_CHANGED, &AgentHostPage::OnTerminalTitleChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_BELL, &AgentHostPage::OnTerminalBell, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_PROGRAM_STATUS, &AgentHostPage::OnTerminalProgramStatus, this);
    EventNotifier::Get()->Unbind(wxEVT_FINDINFILES_DLG_SHOWING, &AgentHostPage::OnFindInFilesShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_OPEN_RESOURCE_SHOWING, &AgentHostPage::OnOpenResourceShowing, this);

    if (auto frame = EventNotifier::Get()->TopFrame()) {
        frame->Unbind(wxEVT_MENU, &AgentHostPage::OnShowTerminal, this, XRCID("show_terminal_pane"));
    }
}

void AgentHostPage::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(GetTerminal());

    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    CHECK_COND_RET(lexer);
    auto font = lexer->GetFontForStyle(0, this);
    for (auto* terminal : {GetTerminal(), GetReviewTerminal()}) {
        if (terminal == nullptr) {
            continue;
        }
        auto theme = terminal->GetTheme();
        theme.font = font;
        terminal->SetTheme(theme);
    }
}

void AgentHostPage::OnTerminalTitleChanged(clCommandEvent& event)
{
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(GetTerminal());
    wxString new_title = event.GetString();
    new_title.Trim().Trim(false);
    if (new_title.empty()) {
        new_title = _("Claude Code");
    }

    auto book = clGetManager()->GetMainNotebook();
    int where = book->FindPage(this);
    if (where != wxNOT_FOUND) {
        book->SetPageText(where, new_title);
    }
}

void AgentHostPage::UpdateWorkingIcon()
{
    // The spinner is on our tab while the main agent or the reviewer works. Removing it restores the icon of the tab.
    const bool working =
        m_lastAgentState == wxTerminalProgramState::Working || m_lastReviewState == wxTerminalProgramState::Working;
    if (!working) {
        m_workingIcon.reset();
        return;
    }

    if (m_workingIcon) {
        return; // already spinning
    }

    // An icon that asks for the user is out of date now
    m_attentionIcon.reset();
    auto* icons = clGetManager()->GetStdIcons();
    std::vector<wxBitmap> frames;
    for (int i = 0; i < 8; ++i) {
        frames.push_back(icons->LoadBitmap(wxString::Format("agent-spinner-%d", i)));
    }
    m_workingIcon = std::make_unique<TabAttentionIcon>(clGetManager()->GetMainNotebook(), this, frames, false);
}

void AgentHostPage::OnTerminalProgramStatus(clCommandEvent& event)
{
    // The reviewer: only its spinner is used (its other states are for the reviewer's pane)
    if (GetReviewTerminal() != nullptr && event.GetEventObject() == GetReviewTerminal()) {
        event.Skip();
        const auto reviewState = static_cast<wxTerminalProgramState>(event.GetInt());
        if (std::exchange(m_lastReviewState, reviewState) != reviewState) {
            UpdateWorkingIcon();
        }
        return;
    }

    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(GetTerminal());

    const auto state = static_cast<wxTerminalProgramState>(event.GetInt());
    const auto previous = std::exchange(m_lastAgentState, state);

    // Only a change is reported (a working agent reports its progress again and again).
    if (state == previous) {
        return;
    }

    // The agent is done and may have changed files: let the others (the git view, for example) refresh.
    // This does not depend on the notification settings.
    if (state == wxTerminalProgramState::Done) {
        clFileSystemEvent modifiedEvent{wxEVT_FILE_MODIFIED_EXTERNALLY};
        EventNotifier::Get()->AddPendingEvent(modifiedEvent);
    }

    // A spinner on the tab while the agent works, also when the tab is selected. It only shows the state, so it does
    // not depend on the notification settings.
    UpdateWorkingIcon();

    // During a review the main agent stops on purpose, to hand the control to the reviewer: do not bother the user.
    if (m_review) {
        return;
    }

    // The user turned the notifications off (Terminal settings)
    if (!TerminalSettings::Load().m_notifyWhenDone) {
        return;
    }

    // Only these states need the user. Idle, Working and Clear: nothing to do.
    if (state != wxTerminalProgramState::Blocked && state != wxTerminalProgramState::Done &&
        state != wxTerminalProgramState::Error) {
        return;
    }

    // The tab is not the selected one: mark it until the user comes back to the page.
    // A waving hand: the agent is waiting for the user. A checkmark: the agent is done. (No icon for an error.)
    // Defensive: never save the spinner as the original icon of the tab (it is on the tab while the reviewer works)
    if (m_book->GetCurrentPage() != this && state != wxTerminalProgramState::Error && !m_workingIcon) {
        const char* iconName =
            state == wxTerminalProgramState::Blocked ? "agent-attention-blocked" : "agent-attention-done";
        // Replacing the icon first restores the original tab icon
        m_attentionIcon.reset();
        m_attentionIcon = std::make_unique<TabAttentionIcon>(
            clGetManager()->GetMainNotebook(),
            this,
            std::vector<wxBitmap>{clGetManager()->GetStdIcons()->LoadBitmap(iconName)});
    }

    // The agent is waiting for the user and CodeLite is in the background: the Dock icon bounces on macOS, the
    // taskbar button flashes on Windows. A finished task can wait.
    if (state == wxTerminalProgramState::Blocked && !wxTheApp->IsActive()) {
        if (auto* frame = dynamic_cast<wxTopLevelWindow*>(wxTheApp->GetTopWindow())) {
            frame->RequestUserAttention(wxUSER_ATTENTION_INFO);
        }
    }
}

void AgentHostPage::OnTerminalTerminated(clCommandEvent& event)
{
    if (GetReviewTerminal() != nullptr && event.GetEventObject() == GetReviewTerminal()) {
        // The reviewer quit: close its pane, as the page closes when the main agent exits.
        event.Skip();
        CallAfter(&AgentHostPage::CloseReviewBuddy);
        return;
    }
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(GetTerminal());

    CallAfter([this]() {
        auto book = clGetManager()->GetMainNotebook();
        int where = book->FindPage(this);
        if (where != wxNOT_FOUND) {
            book->DeletePage(where);
        }
    });
}

namespace
{
/// The first line of `text`, trimmed.
wxString FirstLine(const wxString& text) { return text.BeforeFirst('\n').BeforeFirst('\r').Trim().Trim(false); }

/// The text to search for, taken from the selection of `terminal`: first line of the terminal selection. If it is
/// empty, the first line of the text the application copied (e.g. tmux, TUI apps like claude). Empty if there is none.
wxString GetFindWhat(const wxTerminalViewCtrl* terminal)
{
    if (terminal == nullptr) {
        return wxEmptyString;
    }

    wxString text;
    if (auto selection = terminal->GetMouseSelection()) {
        text = FirstLine(*selection);
    }
    if (text.empty()) {
        if (auto selection = terminal->GetAppSelection()) {
            text = FirstLine(*selection);
        }
    }
    return text;
}

template <typename Event>
void UpdateFindWhat(Event& event, const wxTerminalViewCtrl* terminal)
{
    event.Skip();
    const wxString text = GetFindWhat(terminal);
    if (!text.empty()) {
        event.SetFindWhat(text);
    }
}
} // namespace

// Normally, only a terminal that has the focus is used (the focus tells where the user is working). When a toolbar
// button runs the command, use the terminal that has a selection instead.
void AgentHostPage::OnOpenResourceShowing(clOpenResourceEvent& event)
{
    UpdateFindWhat(event, m_fillFromSelection ? GetSelectionTerminal() : GetFocusedTerminal());
}

void AgentHostPage::OnFindInFilesShowing(clFindInFilesEvent& event)
{
    UpdateFindWhat(event, m_fillFromSelection ? GetSelectionTerminal() : GetFocusedTerminal());
}

void AgentHostPage::OnTerminalBell(clCommandEvent& event)
{
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(GetTerminal());
    clDEBUG() << "Got a bell!" << endl;
}

void AgentHostPage::OnTerminalLink(clCommandEvent& event)
{
    if (event.GetEventObject() != GetTerminal() && event.GetEventObject() != GetReviewTerminal()) {
        event.Skip();
        return;
    }
    OpenText(event.GetString());
}

void AgentHostPage::OpenText(const wxString& text)
{
    wxString trimmed_text = text;
    while (trimmed_text.EndsWith(".") || trimmed_text.EndsWith(":"))
        trimmed_text.RemoveLast();

    if (trimmed_text.StartsWith("http://") || trimmed_text.StartsWith("https://")) {
        ::wxLaunchDefaultBrowser(trimmed_text);
        return;
    }

    wxString saved_trimmed_text = trimmed_text;
    if (trimmed_text.StartsWith("~/"))
        trimmed_text = wxGetHomeDir() + trimmed_text.Mid(1);

    if (wxFileName::DirExists(trimmed_text)) {
        CallAfter([trimmed_text]() { FileUtils::OpenFileExplorer(trimmed_text); });
        return;
    }

    if (FileUtils::IsBinaryExecutable(trimmed_text)) {
        ::wxLaunchDefaultApplication(trimmed_text);
        return;
    }

    if (auto editor = clGetManager()->OpenFile(trimmed_text); editor != nullptr) {
        editor->SetActive();
        return;
    }

#if USE_SFTP
    // Try a remote file.
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace && workspace->IsRemote() &&
        (clSFTPManager::Get().OpenFile(saved_trimmed_text, workspace->GetSshAccount()) != nullptr))
        return;
#endif

    // Could not resolve it, try the "open resource dialog"
    OpenResourceDialog dlg(EventNotifier::Get()->TopFrame(), clGetManager(), trimmed_text);

    if (dlg.ShowModal() == wxID_OK && !dlg.GetSelections().empty()) {
        std::vector<OpenResourceDialogItemData*> items = dlg.GetSelections();
        for (const auto item : items) {

            // try the plugins first
            clCommandEvent open_resource_event(wxEVT_OPEN_RESOURCE_FILE_SELECTED);
            open_resource_event.SetFileName(item->m_file);
            open_resource_event.SetLineNumber(item->m_line);
            open_resource_event.SetInt(item->m_column); // use the int field for the column

            if (EventNotifier::Get()->ProcessEvent(open_resource_event)) {
                continue;
            }

            // default behaviour
            OpenResourceDialog::OpenSelection(*item, clGetManager());
        }
    }
}

void AgentHostPage::StartAgentHost(const AgentInfo& info)
{
    m_agentInfo = info;
    RestartAgentHost();
}

std::optional<SystemPromptCli> AgentHostPage::BuildSystemPrompt(const AgentInfo& info)
{
    wxBusyCursor bc{};
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr)
        return std::nullopt;

    wxString systemPromptFilePath;
    SystemPromptCli cli;
    wxString& systemPromptArgs = cli.args;
    wxString systemPrompt = llm::Manager::GetInstance().GetConfig().GetSystemPrompt();
    if (systemPrompt.empty())
        return std::nullopt;

    switch (info.agent_type) {
    case AgentType::kClaudeCode: {
        systemPromptFilePath = FileManager::GetSettingFileFullPath("SYSTEM_PROMPT.md");
        if (!FileManager::WriteContent(systemPromptFilePath, systemPrompt, true)) {
            clWARNING() << "Failed to write system prompt file:" << systemPromptFilePath << endl;
            return std::nullopt;
        }
        systemPromptArgs << "--append-system-prompt-file " << StringUtils::WrapWithDoubleQuotes(systemPromptFilePath);
    } break;
    case AgentType::kKiroCli: {
        // Kiro does not support using custom files, we need to use .kiro/steering/SYSTEM_PROMPT.md file.
        systemPromptFilePath = FileManager::GetFullPath(".kiro/steering/SYSTEM_PROMPT.md");
        wxString dirpath = systemPromptFilePath.BeforeLast('/');
        clDEBUG() << "Creating rmeote dir:" << dirpath << endl;
        FileManager::CreateDir(dirpath);
        systemPromptArgs.clear();
    } break;
    case AgentType::kOpenCode: {
        // OpenCode has no command line option for this. Point it to a config file that lists the system prompt file
        // under "instructions", using the OPENCODE_CONFIG environment variable (the file is merged with the other
        // config files). The prompt file is written below.
        systemPromptFilePath = FileManager::GetSettingFileFullPath("SYSTEM_PROMPT.md");
        wxString configFilePath = FileManager::GetSettingFileFullPath("opencode.json");

        // Use forward slashes: it works on all the platforms and needs no escaping inside the JSON string
        wxString promptPathInJson = systemPromptFilePath;
        promptPathInJson.Replace("\\", "/");
        wxString configPathInEnv = configFilePath;
        configPathInEnv.Replace("\\", "/");
        cli.env.insert({"OPENCODE_CONFIG", configPathInEnv});

        const wxString configContent = wxString::Format(
            R"({ "$schema": "https://opencode.ai/config.json", "instructions": ["%s"]})", promptPathInJson);
        if (!FileManager::WriteContent(configFilePath, configContent, true)) {
            clWARNING() << "Failed to write OpenCode config file:" << configFilePath << endl;
            return std::nullopt;
        }
    } break;
    }

    if (!FileManager::WriteContent(systemPromptFilePath, systemPrompt, true)) {
        clWARNING() << "Failed to write system prompt file:" << systemPromptFilePath << endl;
        return std::nullopt;
    }
    clDEBUG() << "Successfully written system prompt file:" << systemPromptFilePath << endl;
    clDEBUG() << "Will add the following args to agent CLI:" << systemPromptArgs << endl;
    return cli;
}

void AgentHostPage::SetFocus()
{
    // wxPanel::SetFocus() gives the focus to the last focused child, which is not always the terminal. This is called,
    // for example, when the output pane is hidden and the focus returns to the current page of the main book.
    if (GetTerminal() != nullptr) {
        GetTerminal()->SetFocus();
    } else {
        AgentHostPageBase::SetFocus();
    }
}

void AgentHostPage::OnFocus(wxFocusEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(GetTerminal());
    GetTerminal()->SetFocus();
}

void AgentHostPage::OnBookPageChanged(wxBookCtrlEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(GetTerminal());
    if (m_book->GetCurrentPage() != this) {
        return;
    }

    // The focus is moved to the tab label after the page is changed, so set it once we are done handling the event
    CallAfter([this]() {
        if (GetTerminal()) {
            GetTerminal()->SetFocus();
        }
    });
}

void AgentHostPage::OnContextMenu(wxContextMenuEvent& event)
{
    // There is no context menu: all the actions are in the toolbar. The event is not skipped, to also hide the default
    // menu of the terminal (this handler is bound after the terminal's own handler, so it runs first).
    wxUnusedVar(event);
}

void AgentHostPage::RestartAgentHost()
{
    wxWindowUpdateLocker locker{this};
    CloseReviewBuddy();
    if (m_agentTerminal) {
        wxDELETE(m_agentTerminal);
    }

    // Remember the label given to the tab, the blink code needs it.
    wxString baseCommand = m_agentInfo.executable;
    baseCommand.Prepend("\"").Append("\"");
    wxString command_to_run;

    // Append the system prompt args to the base command.
    auto systemPromptCli = BuildSystemPrompt(m_agentInfo);
    if (systemPromptCli && !systemPromptCli->args.empty())
        baseCommand << " " << systemPromptCli->args;

    switch (m_agentInfo.agent_type) {
    case AgentType::kClaudeCode:
    case AgentType::kOpenCode:
        command_to_run = wxString::Format("%s --continue || %s", baseCommand, baseCommand);
        break;
    case AgentType::kKiroCli:
        command_to_run = wxString::Format("%s --resume || %s", baseCommand, baseCommand);
        break;
    }
    if (!m_agentInfo.workingDirectory.empty()) {
        wxString cd_command;
        cd_command << "cd \"" << m_agentInfo.workingDirectory << "\" && ";
        command_to_run.Prepend(cd_command);
    }
    // The terminal inherits the environment of CodeLite, set the agent variables until it is launched
    wxStringMap_t envMap;
    std::unique_ptr<EnvSetter> env;
    if (systemPromptCli && !systemPromptCli->env.empty()) {
        envMap = systemPromptCli->env;
        env = std::make_unique<EnvSetter>(&envMap);
    }

    m_agentTerminal = new AgentTerminal(m_splitter, m_agentInfo.sshAccount, kShellCommand);
    if (GetTerminal() == nullptr) {
        wxDELETE(m_agentTerminal);
        return;
    }

    m_splitter->Initialize(m_agentTerminal);
    GetTerminal()->RegisterTextClickedFilter([](const std::string&) -> bool { return true; });

    GetSizer()->Layout();
    // Hook a custom context menu
    GetTerminal()->Bind(wxEVT_CONTEXT_MENU, &AgentHostPage::OnContextMenu, this);
    GetTerminal()->SendCommand(command_to_run);
}

wxTerminalViewCtrl* AgentHostPage::GetTerminal() const
{
    return m_agentTerminal != nullptr ? m_agentTerminal->GetTerminal() : nullptr;
}

wxTerminalViewCtrl* AgentHostPage::GetReviewTerminal() const
{
    return m_reviewAgentTerminal != nullptr ? m_reviewAgentTerminal->GetTerminal() : nullptr;
}

wxString AgentHostPage::GetSelectedLine(wxTerminalViewCtrl* terminal) const
{
    if (terminal == nullptr || !terminal->CanCopy()) {
        return wxEmptyString;
    }
    // Use the first line of the selection only
    return FirstLine(terminal->GetMouseSelection().value_or(wxEmptyString));
}

void AgentHostPage::SearchInWorkspace(const wxString& text)
{
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    auto owner = clGetManager()->BookGetPage(PaneId::BOTTOM_BAR, FIND_IN_FILES_WIN);
    if (workspace == nullptr || owner == nullptr || text.empty()) {
        return;
    }

    wxArrayString files;
    workspace->GetWorkspaceFiles(files);
    SearchThreadST::Get()->GrepWord(owner, files, text);

    if (GetTerminal())
        GetTerminal()->CallAfter(&wxWindow::SetFocus);
}

void AgentHostPage::OnShowTerminal(wxCommandEvent& event) { event.Skip(); }

void AgentHostPage::OnGrepWorkspace(wxCommandEvent& event)
{
    // Handle it only when the focus is in our terminal and we have a selection, otherwise let the frame handle it
    const wxString selection = GetSelectedLine(GetFocusedTerminal());
    if (selection.empty()) {
        event.Skip();
        return;
    }
    SearchInWorkspace(selection);
}

void AgentHostPage::OnGrepWorkspaceUI(wxUpdateUIEvent& event)
{
    // Enable the item when our terminal has the focus and a selection, otherwise let the frame decide
    if (!GetSelectedLine(GetFocusedTerminal()).empty() && clWorkspaceManager::Get().GetWorkspace() != nullptr) {
        event.Enable(true);
        return;
    }
    event.Skip();
}

wxTerminalViewCtrl* AgentHostPage::GetFocusedTerminal() const
{
    auto* focus = wxWindow::FindFocus();
    if (focus == nullptr) {
        return nullptr;
    }
    if (focus == GetTerminal() || focus == GetReviewTerminal()) {
        return static_cast<wxTerminalViewCtrl*>(focus);
    }
    return nullptr;
}

wxTerminalViewCtrl* AgentHostPage::GetSelectionTerminal() const
{
    for (auto* terminal : {GetFocusedTerminal(), GetTerminal(), GetReviewTerminal()}) {
        if (terminal != nullptr && !GetFindWhat(terminal).empty()) {
            return terminal;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// The toolbar
// ---------------------------------------------------------------------------

void AgentHostPage::BuildToolbar()
{
    auto* icons = clGetManager()->GetStdIcons();
    m_auibar->Clear();
    // The tooltip of a button shows its keyboard shortcut. Restart and Review Buddy have no command in CodeLite, so
    // they have no shortcut.
    clAuiToolBarArt::AddTool(
        m_auibar, XRCID(kToolRestart), _("Restart"), icons->LoadBitmap("file_reload"), _("Restart the agent"));
    // A drop-down button: both the button and its arrow open the menu to pick the agent of the review
    clAuiToolBarArt::AddTool(m_auibar,
                             XRCID(kToolReviewBuddy),
                             _("Review Buddy"),
                             icons->LoadBitmap("agent-host-code-review"),
                             _("Run Review Buddy"),
                             wxITEM_DROPDOWN);
    m_auibar->AddSeparator();
    clAuiToolBarArt::AddTool(m_auibar,
                             XRCID(kToolCopy),
                             _("Copy"),
                             icons->LoadBitmap("copy"),
                             WithShortcut(_("Copy the selected text"), "wxID_COPY"));
    clAuiToolBarArt::AddTool(m_auibar,
                             XRCID(kToolPaste),
                             _("Paste"),
                             icons->LoadBitmap("paste"),
                             WithShortcut(_("Paste the clipboard text"), "wxID_PASTE"));
    m_auibar->AddSeparator();
    clAuiToolBarArt::AddTool(m_auibar,
                             XRCID(kToolFindInFiles),
                             _("Find In Files"),
                             icons->LoadBitmap("agent-host-find-in-files"),
                             WithShortcut(_("Find In Files: search for the selected text"), "find_in_files"));
    clAuiToolBarArt::AddTool(
        m_auibar,
        XRCID(kToolOpenResource),
        _("Open Resource"),
        icons->LoadBitmap("agent-host-open-resource"),
        WithShortcut(_("Open Resource: find a file or a symbol with the selected text"), "find_resource"));
    clAuiToolBarArt::Finalise(m_auibar);
    m_auibar->Realize();
    // The new tools can have another size (a new theme has other icons): tell the sizer, or the toolbar keeps its old
    // size until the window is resized
    m_auibar->InvalidateBestSize();
    Layout();
    m_auibar->Refresh();
}

void AgentHostPage::OnBitmapsUpdated(clCommandEvent& event)
{
    event.Skip();
    BuildToolbar();
}

void AgentHostPage::OnToolRestart(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // Deferred: leave the toolbar's callback first, the restart destroys the terminal
    CallAfter(&AgentHostPage::RestartAgentHost);
}

void AgentHostPage::OnToolReviewBuddy(wxAuiToolBarEvent& event)
{
    // Not skipped: the button and its arrow both open the menu (and no wxEVT_MENU is sent)
    wxUnusedVar(event);
    wxMenu menu;
    if (m_review) {
        // There is a review: the menu to manage it
        AppendReviewControls(menu);
    } else if (wxString whyNot; !CanHaveReviewBuddy(whyNot)) {
        menu.Append(wxID_ANY, _("Review Buddy is not available") + " (" + whyNot + ")")->Enable(false);
    } else {
        // Pick the agent of the review
        const int claude_id = wxWindow::NewControlId();
        const int kiro_id = wxWindow::NewControlId();
        menu.Append(claude_id, _("Review with Claude Code"));
        menu.Append(kiro_id, _("Review with Kiro CLI"));
        // Deferred: leave the menu's callback first
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent&) { CallAfter(&AgentHostPage::LaunchReviewBuddy, AgentType::kClaudeCode); },
            claude_id);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent&) { CallAfter(&AgentHostPage::LaunchReviewBuddy, AgentType::kKiroCli); },
            kiro_id);
    }

    const int tool_id = XRCID(kToolReviewBuddy);
    // Keep the button pressed while the menu is shown
    clAuiToolStickness stickness{m_auibar, tool_id};
    // Line up the menu with the button
    const wxRect tool_rect = m_auibar->GetToolRect(tool_id);
    m_auibar->PopupMenu(&menu, tool_rect.GetBottomLeft());
}

void AgentHostPage::OnToolCopy(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if (auto* terminal = GetCopyTerminal()) {
        if (terminal->CanCopy()) {
            // The text the user selected comes first
            terminal->Copy();
        } else if (auto selection = terminal->GetAppSelection()) {
            // Otherwise, the text the application copied (e.g. tmux, TUI apps like claude): all of it, not only the
            // first line
            ::CopyToClipboard(*selection);
        }
        // The toolbar click may have moved the focus
        terminal->CallAfter(&wxWindow::SetFocus);
    }
}

void AgentHostPage::OnToolPaste(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // Paste into the terminal that has the focus (the agent's or the reviewer's), the agent's by default
    auto* terminal = GetFocusedTerminal();
    if (terminal == nullptr) {
        terminal = GetTerminal();
    }
    if (terminal != nullptr) {
        terminal->Paste();
        terminal->CallAfter(&wxWindow::SetFocus);
    }
}

void AgentHostPage::OnToolCopyUI(wxUpdateUIEvent& event) { event.Enable(GetCopyTerminal() != nullptr); }

wxTerminalViewCtrl* AgentHostPage::GetCopyTerminal() const
{
    for (auto* terminal : {GetFocusedTerminal(), GetTerminal(), GetReviewTerminal()}) {
        if (terminal == nullptr) {
            continue;
        }
        const auto app_selection = terminal->GetAppSelection();
        if (terminal->CanCopy() || (app_selection.has_value() && !app_selection->empty())) {
            return terminal;
        }
    }
    return nullptr;
}

void AgentHostPage::OnToolFindInFiles(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CallAfter(&AgentHostPage::RunFrameCommand, "find_in_files");
}

void AgentHostPage::OnToolOpenResource(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CallAfter(&AgentHostPage::RunFrameCommand, "find_resource");
}

void AgentHostPage::OnToolHasTerminalUI(wxUpdateUIEvent& event) { event.Enable(GetTerminal() != nullptr); }

void AgentHostPage::OnToolSelectionUI(wxUpdateUIEvent& event) { event.Enable(GetSelectionTerminal() != nullptr); }

void AgentHostPage::RunFrameCommand(const char* name)
{
    auto* frame = EventNotifier::Get()->TopFrame();
    CHECK_PTR_RET(frame);

    // The command sends the "dialog is showing" event, our handlers fill the dialog from the selection
    m_fillFromSelection = true;
    wxCommandEvent event(wxEVT_MENU, XRCID(name));
    event.SetEventObject(frame);
    frame->GetEventHandler()->ProcessEvent(event);
    m_fillFromSelection = false;
}

// ---------------------------------------------------------------------------
// Review Buddy
// ---------------------------------------------------------------------------

void AgentHostPage::AppendReviewControls(wxMenu& menu)
{
    CHECK_PTR_RET(m_review);

    menu.Append(wxID_ANY, _("Review Buddy: ") + m_review->Describe())->Enable(false);
    if (m_review->IsRunning() || m_review->HasStalled()) {
        const int resend_id = wxWindow::NewControlId();
        const int stop_id = wxWindow::NewControlId();
        menu.Append(resend_id, _("Send the Request Again"));
        menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) { m_review->Resend(); }, resend_id);
        menu.Append(stop_id, _("Stop the Review"));
        menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) { m_review->Stop(); }, stop_id);
    }
    const int open_id = wxWindow::NewControlId();
    const int close_id = wxWindow::NewControlId();
    menu.Append(open_id, _("Open the Latest Review"))->Enable(!m_review->CommentsPath().empty());
    menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) { OpenLatestReview(); }, open_id);
    menu.Append(close_id, _("Close Review Buddy"));
    menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) { CallAfter(&AgentHostPage::CloseReviewBuddy); }, close_id);
}

bool AgentHostPage::CanHaveReviewBuddy(wxString& whyNot)
{
    if (m_agentInfo.workingDirectory.empty()) {
        whyNot = _("no working directory");
        return false;
    }
    if (!HasGitRepo()) {
        whyNot = _("not a git repository");
        return false;
    }
    return true;
}

bool AgentHostPage::HasGitRepo()
{
    // Ask git: it also knows a sub-folder of a repository, and a worktree
    const ReviewBuddy::Target target{m_agentInfo.workingDirectory, m_agentInfo.sshAccount, wxString{}};
    if (!m_agentInfo.sshAccount.has_value()) {
        return ReviewBuddy::IsInsideGitRepo(target);
    }

    // A remote check opens an SSH connection, so it is repeated at most every 30 seconds.
    constexpr auto kRecheckAfter = std::chrono::seconds(30);
    const auto now = std::chrono::steady_clock::now();
    if (m_lastGitCheck.has_value() && now - *m_lastGitCheck < kRecheckAfter) {
        return m_hasGit;
    }
    m_lastGitCheck = now;
    m_hasGit = ReviewBuddy::IsInsideGitRepo(target);
    return m_hasGit;
}

void AgentHostPage::LaunchReviewBuddy(AgentType reviewer)
{
    if (m_review || GetReviewTerminal() != nullptr || GetTerminal() == nullptr) {
        return;
    }
    if (m_agentInfo.workingDirectory.empty()) {
        m_infoBar->ShowMessage(_("Cannot find the working directory of the agent"), wxICON_WARNING);
        return;
    }
    clDEBUG() << "Launching review buddy for" << m_agentInfo.workingDirectory << endl;

    // The name of the page, for the notifications
    wxString pageName;
    auto book = clGetManager()->GetMainNotebook();
    if (int where = book->FindPage(this); where != wxNOT_FOUND) {
        pageName = book->GetPageText(where);
    }

    // The reviewer's pane opens when the first request is ready (it is written first, possibly over SSH): the agent
    // reads it as soon as it starts.
    DismissNotice(); // The message about an earlier review
    m_review = std::make_unique<ReviewBuddy>(
        ReviewBuddy::Target{m_agentInfo.workingDirectory, m_agentInfo.sshAccount, pageName},
        GetTerminal(),
        [this, reviewer](
            const wxString& prompt, const wxString& folder) { return StartReviewer(reviewer, prompt, folder); },
        // Whether the user is looking at this page right now.
        [this]() { return IsShownOnScreen() && m_book->GetCurrentPage() == this; },
        [this](const wxString& message, bool problem) {
            m_infoBar->ShowMessage(message, problem ? wxICON_WARNING : wxICON_INFORMATION);
        },
        [this](wxTerminalViewCtrl* terminal) { FocusTerminal(terminal); });
    m_review->Bind(wxEVT_REVIEW_BUDDY_CLOSE, [this](wxCommandEvent&) { CallAfter(&AgentHostPage::CloseReviewBuddy); });
    m_review->Begin();
}

wxTerminalViewCtrl* AgentHostPage::StartReviewer(AgentType reviewer, const wxString& prompt, const wxString& folder)
{
    if (GetReviewTerminal() != nullptr || GetTerminal() == nullptr) {
        return GetReviewTerminal();
    }

    // The reviewer runs in its review folder: agent resume history is keyed by working directory.
    const wxString reviewerDir = ReviewBuddy::JoinPath(m_agentInfo.workingDirectory, folder);

    // The reviewer runs on the same host as the main agent, so both see the same project.
    auto executable = ResolveAgentExecutable(reviewer);
    if (!executable.has_value()) {
        return nullptr;
    }

    // A new conversation (no --continue / --resume), started with the request as its first message
    wxString command = StringUtils::WrapWithDoubleQuotes(executable->executable);
    if (reviewer == AgentType::kKiroCli) {
        command << " chat";
    }
    // The user's extra arguments, as typed in the settings (for example, a custom agent). They are shell text: the
    // user does the quoting.
    wxString extraArgsKey;
    switch (reviewer) {
    case AgentType::kClaudeCode:
        extraArgsKey = kAgentHostClaudeCodeReviewArgs;
        break;
    case AgentType::kKiroCli:
        extraArgsKey = kAgentHostKiroCliReviewArgs;
        break;
    case AgentType::kOpenCode:
        extraArgsKey = kAgentHostOpenCodeReviewArgs;
        break;
    }
    wxString extraArgs = clConfig::Get().Read(extraArgsKey, wxString{});
    extraArgs.Trim().Trim(false);
    if (!extraArgs.empty()) {
        command << " " << extraArgs;
    }
    command << " " << StringUtils::WrapWithDoubleQuotes(prompt);
    command.Prepend("cd " + StringUtils::WrapWithDoubleQuotes(reviewerDir) + " && ");

    m_reviewAgentTerminal = new AgentTerminal(m_splitter, m_agentInfo.sshAccount, kShellCommand);
    if (GetReviewTerminal() == nullptr) {
        wxDELETE(m_reviewAgentTerminal);
        return nullptr;
    }
    GetReviewTerminal()->Bind(wxEVT_CONTEXT_MENU, &AgentHostPage::OnContextMenu, this);
    GetReviewTerminal()->SendCommand(command);

    m_splitter->SplitVertically(m_agentTerminal, m_reviewAgentTerminal);
    // The reviewer is the active agent now; it may ask for a permission.
    FocusTerminal(GetReviewTerminal());
    return GetReviewTerminal();
}

void AgentHostPage::DismissNotice()
{
    if (m_infoBar != nullptr && m_infoBar->IsShown()) {
        m_infoBar->Dismiss();
    }
}

void AgentHostPage::DestroyReviewPane()
{
    if (m_reviewAgentTerminal != nullptr) {
        m_reviewAgentTerminal->Destroy(); // also ends the reviewer's process
    }
    m_reviewAgentTerminal = nullptr;

    // The reviewer is gone: it does not work any more
    m_lastReviewState = wxTerminalProgramState::Clear;
    UpdateWorkingIcon();
}

void AgentHostPage::CloseReviewBuddy()
{
    if (m_review && m_review->IsBusy()) {
        // Never destroy the loop while one of its functions is running
        CallAfter(&AgentHostPage::CloseReviewBuddy);
        return;
    }
    DismissNotice();
    if (m_reviewAgentTerminal != nullptr) {
        wxWindowUpdateLocker locker{this};
        if (m_splitter->IsSplit()) {
            m_splitter->Unsplit(m_reviewAgentTerminal);
        }
        DestroyReviewPane();
    }
    // Destroy after the reviewer terminal: cleanup may remove its working directory.
    m_review.reset();
    if (GetTerminal() != nullptr) {
        GetTerminal()->SetFocus();
    }
}

void AgentHostPage::OpenLatestReview()
{
    CHECK_PTR_RET(m_review);
    const wxString relative = m_review->CommentsPath();
    CHECK_COND_RET(!relative.empty());

    const wxString path = ReviewBuddy::JoinPath(m_agentInfo.workingDirectory, relative);
    bool opened = false;
    if (m_agentInfo.sshAccount.has_value()) {
#if USE_SFTP
        opened = clSFTPManager::Get().OpenFile(path, *m_agentInfo.sshAccount) != nullptr;
#endif
    } else if (wxFileName::FileExists(path)) {
        opened = clGetManager()->OpenFile(path) != nullptr;
    }
    if (!opened) {
        m_infoBar->ShowMessage(_("The review is not ready yet"), wxICON_INFORMATION);
    }
}

void AgentHostPage::FocusTerminal(wxTerminalViewCtrl* terminal)
{
    // Only if the user is working in this page: do not take the focus away from another one.
    if (terminal != nullptr && wxTheApp->IsActive() && IsShownOnScreen()) {
        terminal->SetFocus();
    }
}
