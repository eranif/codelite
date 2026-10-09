#include "AgentHostPage.hpp"

#include "ColoursAndFontsManager.h"
#include "FileManager.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "ReviewBuddy.hpp"
#include "ai/LLMManager.hpp"
#include "clStrings.h"
#include "globals.h"
#include "open_resource_dialog.h"
#include "search_thread.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>

namespace
{
#ifdef __WXMSW__
const wxString kShellCommand = "CMD";
#else
const wxString kShellCommand = "/bin/bash --login -i";
#endif
} // namespace

#define CHECK_CAN_HANDLE_EVENT(event)           \
    if (event.GetEventObject() != m_terminal) { \
        event.Skip();                           \
        return;                                 \
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
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &AgentHostPage::OnThemeChanged, this);
    if (auto frame = EventNotifier::Get()->TopFrame()) {
        frame->Bind(wxEVT_MENU, &AgentHostPage::OnGrepWorkspace, this, XRCID("grep_current_workspace"));
        frame->Bind(wxEVT_UPDATE_UI, &AgentHostPage::OnGrepWorkspaceUI, this, XRCID("grep_current_workspace"));
        frame->Bind(wxEVT_MENU, &AgentHostPage::OnShowTerminal, this, XRCID("show_terminal_pane"));
    }
}

AgentHostPage::~AgentHostPage()
{
    m_review.reset(); // stops its timers
#ifndef __WXMSW__
    m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &AgentHostPage::OnBookPageChanged, this);
#endif
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &AgentHostPage::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TEXT_LINK_CLICKED, &AgentHostPage::OnTerminalLink, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TERMINATED, &AgentHostPage::OnTerminalTerminated, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_TITLE_CHANGED, &AgentHostPage::OnTerminalTitleChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_BELL, &AgentHostPage::OnTerminalBell, this);
    if (auto frame = EventNotifier::Get()->TopFrame()) {
        frame->Unbind(wxEVT_MENU, &AgentHostPage::OnGrepWorkspace, this, XRCID("grep_current_workspace"));
        frame->Unbind(wxEVT_UPDATE_UI, &AgentHostPage::OnGrepWorkspaceUI, this, XRCID("grep_current_workspace"));
        frame->Unbind(wxEVT_MENU, &AgentHostPage::OnShowTerminal, this, XRCID("show_terminal_pane"));
    }
}

void AgentHostPage::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_terminal);

    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    CHECK_COND_RET(lexer);
    auto font = lexer->GetFontForStyle(0, this);
    for (auto* terminal : {m_terminal, m_reviewTerminal}) {
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
    CHECK_PTR_RET(m_terminal);
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

void AgentHostPage::OnTerminalTerminated(clCommandEvent& event)
{
    if (m_reviewTerminal != nullptr && event.GetEventObject() == m_reviewTerminal) {
        // The reviewer quit: close its pane, as the page closes when the main agent exits.
        event.Skip();
        CallAfter(&AgentHostPage::CloseReviewBuddy);
        return;
    }
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(m_terminal);

    CallAfter([this]() {
        auto book = clGetManager()->GetMainNotebook();
        int where = book->FindPage(this);
        if (where != wxNOT_FOUND) {
            book->DeletePage(where);
        }
    });
}

void AgentHostPage::OnTerminalBell(clCommandEvent& event)
{
    CHECK_CAN_HANDLE_EVENT(event);
    CHECK_PTR_RET(m_terminal);
    clDEBUG() << "Got a bell!" << endl;
}

void AgentHostPage::OnTerminalLink(clCommandEvent& event)
{
    if (event.GetEventObject() != m_terminal && event.GetEventObject() != m_reviewTerminal) {
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

std::optional<wxString> AgentHostPage::BuildSystemPrompt(const AgentInfo& info)
{
    wxBusyCursor bc{};
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if (workspace == nullptr)
        return std::nullopt;

    wxString systemPromptFilePath;
    wxString systemPromptArgs;
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
    default:
        return std::nullopt;
    }

    if (!FileManager::WriteContent(systemPromptFilePath, systemPrompt, true)) {
        clWARNING() << "Failed to write system prompt file:" << systemPromptFilePath << endl;
        return std::nullopt;
    }
    clDEBUG() << "Successfully written system prompt file:" << systemPromptFilePath << endl;
    clDEBUG() << "Will add the following args to agent CLI:" << systemPromptArgs << endl;
    return systemPromptArgs;
}

void AgentHostPage::OnFocus(wxFocusEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_terminal);
    m_terminal->SetFocus();
}

void AgentHostPage::OnBookPageChanged(wxBookCtrlEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_terminal);
    if (m_book->GetCurrentPage() != this) {
        return;
    }

    // The focus is moved to the tab label after the page is changed, so set it once we are done handling the event
    CallAfter([this]() {
        if (m_terminal) {
            m_terminal->SetFocus();
        }
    });
}

void AgentHostPage::OnContextMenu(wxContextMenuEvent& event)
{
    // The menu is shown for the terminal that was clicked: the agent's or the reviewer's
    auto* terminal = dynamic_cast<wxTerminalViewCtrl*>(event.GetEventObject());
    if (terminal == nullptr) {
        terminal = m_terminal;
    }
    wxMenu menu;

    const wxString selection = GetSelectedLine(terminal);
    if (terminal->CanCopy()) {
        menu.Append(wxID_COPY);
        menu.Bind(wxEVT_MENU, [terminal](wxCommandEvent&) { terminal->Copy(); }, wxID_COPY);
    }

    menu.Append(wxID_PASTE);
    menu.Bind(wxEVT_MENU, [terminal](wxCommandEvent&) { terminal->Paste(); }, wxID_PASTE);
    menu.AppendSeparator();

    if (!selection.empty()) {
        // Keep long selections readable in the menu
        wxString label = selection;
        if (label.length() > 40) {
            label = label.Left(37) + "...";
        }
        // Escape '&' so it is not used as a mnemonic
        label.Replace("&", "&&");

        const int symbol_id = wxWindow::NewControlId();
        const int search_id = XRCID("grep_current_workspace");
        wxString search_label = wxString::Format(_("Search '%s' in Workspace"), label);

        menu.Append(search_id, search_label);
        menu.Append(symbol_id, wxString::Format(_("Open Symbol '%s'"), label));
        menu.Bind(
            wxEVT_MENU,
            [this, selection](wxCommandEvent&) { CallAfter(&AgentHostPage::SearchInWorkspace, selection); },
            search_id);
        menu.Bind(
            wxEVT_MENU,
            [this, selection](wxCommandEvent&) { CallAfter(&AgentHostPage::OpenText, selection); },
            symbol_id);
        menu.AppendSeparator();
    }
    menu.Append(wxID_REFRESH);
    menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) { CallAfter(&AgentHostPage::RestartAgentHost); }, wxID_REFRESH);

    AppendReviewBuddyMenu(menu, terminal);

    clKeyboardManager::Get()->UpdateMenuShortcuts(menu);
    terminal->PopupMenu(&menu);
}

void AgentHostPage::RestartAgentHost()
{
    wxWindowUpdateLocker locker{this};
    CloseReviewBuddy();
    if (m_terminal) {
        wxDELETE(m_terminal);
    }

    // Remember the label given to the tab, the blink code needs it.
    wxString baseCommand = m_agentInfo.executable;
    baseCommand.Prepend("\"").Append("\"");
    wxString command_to_run;

    // Append the system prompt args to the base command.
    auto systemPromptArgs = BuildSystemPrompt(m_agentInfo);
    if (systemPromptArgs && !systemPromptArgs->empty())
        baseCommand << " " << systemPromptArgs.value();

    switch (m_agentInfo.agent_type) {
    case AgentType::kClaudeCode:
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
    m_terminal = clGetManager()->GetTerminalManager()->OpenNewTerminalTab(
        wxEmptyString, m_agentInfo.sshAccount, wxEmptyString, true, kShellCommand, m_splitter);
    m_splitter->Initialize(m_terminal);
    GetSizer()->Layout();
    // Hook a custom context menu
    m_terminal->Bind(wxEVT_CONTEXT_MENU, &AgentHostPage::OnContextMenu, this);
    m_terminal->SendCommand(command_to_run);
}

wxString AgentHostPage::GetSelectedLine(wxTerminalViewCtrl* terminal) const
{
    if (terminal == nullptr || !terminal->CanCopy()) {
        return wxEmptyString;
    }
    // Use the first line of the selection only
    return terminal->GetMouseSelection().value_or(wxEmptyString).BeforeFirst('\n').BeforeFirst('\r').Trim().Trim(false);
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

    if (m_terminal)
        m_terminal->CallAfter(&wxWindow::SetFocus);
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
    if (focus == m_terminal || focus == m_reviewTerminal) {
        return static_cast<wxTerminalViewCtrl*>(focus);
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Review Buddy
// ---------------------------------------------------------------------------

void AgentHostPage::AppendReviewBuddyMenu(wxMenu& menu, wxTerminalViewCtrl* terminal)
{
    if (m_review) {
        menu.AppendSeparator();
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
        return;
    }

    // Only the main agent can get a reviewer
    if (terminal != m_terminal) {
        return;
    }
    menu.AppendSeparator();

    wxString whyNot;
    if (!CanHaveReviewBuddy(whyNot)) {
        menu.Append(wxID_ANY, _("Launch Review Buddy") + " (" + whyNot + ")")->Enable(false);
        return;
    }

    auto* submenu = new wxMenu();
    const int claude_id = wxWindow::NewControlId();
    const int kiro_id = wxWindow::NewControlId();
    submenu->Append(claude_id, _("Claude Code"));
    submenu->Append(kiro_id, _("Kiro CLI"));
    // Bound on the submenu: its items send their events there first. Deferred: leave the menu's callback first.
    submenu->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) { CallAfter(&AgentHostPage::LaunchReviewBuddy, AgentType::kClaudeCode); },
        claude_id);
    submenu->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) { CallAfter(&AgentHostPage::LaunchReviewBuddy, AgentType::kKiroCli); },
        kiro_id);
    menu.AppendSubMenu(submenu, _("Launch Review Buddy"));
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
    if (m_review || m_reviewTerminal != nullptr || m_terminal == nullptr) {
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
        m_terminal,
        [this, reviewer](const wxString& prompt) { return StartReviewer(reviewer, prompt); },
        // Whether the user is looking at this page right now.
        [this]() { return IsShownOnScreen() && m_book->GetCurrentPage() == this; },
        [this](const wxString& message, bool problem) {
            m_infoBar->ShowMessage(message, problem ? wxICON_WARNING : wxICON_INFORMATION);
        },
        [this](wxTerminalViewCtrl* terminal) { FocusTerminal(terminal); });
    m_review->Begin();
}

wxTerminalViewCtrl* AgentHostPage::StartReviewer(AgentType reviewer, const wxString& prompt)
{
    if (m_reviewTerminal != nullptr || m_terminal == nullptr) {
        return m_reviewTerminal;
    }

    // The reviewer runs on the same host as the main agent, so both see the same folder.
    auto executable = ResolveAgentExecutable(reviewer);
    if (!executable.has_value()) {
        return nullptr;
    }

    // A new conversation (no --continue / --resume), started with the request as its first message
    wxString command = StringUtils::WrapWithDoubleQuotes(executable->executable);
    if (reviewer == AgentType::kKiroCli) {
        command << " chat";
    }
    command << " " << StringUtils::WrapWithDoubleQuotes(prompt);
    if (!m_agentInfo.workingDirectory.empty()) {
        command.Prepend("cd \"" + m_agentInfo.workingDirectory + "\" && ");
    }

    m_reviewTerminal = clGetManager()->GetTerminalManager()->OpenNewTerminalTab(
        wxEmptyString, m_agentInfo.sshAccount, wxEmptyString, true, kShellCommand, m_splitter);
    if (m_reviewTerminal == nullptr) {
        return nullptr;
    }
    m_reviewTerminal->Bind(wxEVT_CONTEXT_MENU, &AgentHostPage::OnContextMenu, this);
    m_reviewTerminal->SendCommand(command);

    m_splitter->SplitVertically(m_terminal, m_reviewTerminal);
    // The reviewer is the active agent now; it may ask for a permission.
    FocusTerminal(m_reviewTerminal);
    return m_reviewTerminal;
}

void AgentHostPage::DismissNotice()
{
    if (m_infoBar != nullptr && m_infoBar->IsShown()) {
        m_infoBar->Dismiss();
    }
}

void AgentHostPage::CloseReviewBuddy()
{
    if (m_review && m_review->IsBusy()) {
        // Never destroy the loop while one of its functions is running
        CallAfter(&AgentHostPage::CloseReviewBuddy);
        return;
    }
    m_review.reset(); // stops its timers
    DismissNotice();
    if (m_reviewTerminal == nullptr) {
        return;
    }
    wxWindowUpdateLocker locker{this};
    if (m_splitter->IsSplit()) {
        m_splitter->Unsplit(m_reviewTerminal);
    }
    m_reviewTerminal->Destroy(); // also ends the reviewer's process
    m_reviewTerminal = nullptr;
    if (m_terminal != nullptr) {
        m_terminal->SetFocus();
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
