#pragma once

#include "AgentHostUI.hpp"
#include "cl_command_event.h"
#include "macros.h"
#include "ssh_account_info.h"
#include "terminal_event.h"

#include <chrono>
#include <memory>
#include <optional>
#include <wx/infobar.h>
#include <wx/splitter.h>

class AgentTerminal;
class ReviewBuddy;
class TabAttentionIcon;
class wxTerminalViewCtrl;

enum class AgentType {
    kClaudeCode,
    kKiroCli,
    kOpenCode,
};

/// The executable of an agent and, for a remote workspace, the SSH account it runs on
struct AgentExecutable {
    wxString executable;
    std::optional<SSHAccountInfo> sshAccount;
};

/// Locate the executable of `agent_type` for the current workspace. In a remote workspace this is the name to find on
/// the remote PATH. Returns `std::nullopt` if there is no workspace or the executable is not found (in which case a
/// message box is shown when `warn` is true).
std::optional<AgentExecutable> ResolveAgentExecutable(AgentType agent_type, bool warn = true);

struct AgentInfo {
    AgentType agent_type;
    wxString executable;
    wxString workingDirectory;
    std::optional<SSHAccountInfo> sshAccount;
};

/// How to pass the system prompt to an agent
struct SystemPromptCli {
    /// Arguments to append to the agent command line
    wxString args;
    /// Environment variables to set for the agent process
    wxStringMap_t env;
};

class AgentHostPage : public AgentHostPageBase
{
public:
    AgentHostPage(wxBookCtrlBase* parent);
    ~AgentHostPage() override;

    /// The main agent's terminal
    wxTerminalViewCtrl* GetTerminal() const;
    void StartAgentHost(const AgentInfo& info);

    /// Pass the focus to the main agent's terminal (if it was created)
    void SetFocus() override;

protected:
    void OnFocus(wxFocusEvent& event) override;
    void OnBookPageChanged(wxBookCtrlEvent& event);
    void OnThemeChanged(clCommandEvent& event);
    void OnTerminalLink(clCommandEvent& event);
    void OnTerminalBell(clCommandEvent& event);
    void OnTerminalTerminated(clCommandEvent& event);
    void OnTerminalTitleChanged(clCommandEvent& event);
    /// The main agent reported its status. Tell the user when the agent needs them (a tab icon, and the taskbar or Dock
    /// icon) when they are not looking at this page. Nothing is done during a review: the main agent stops on purpose,
    /// to hand the control to the reviewer. The status of the reviewer is used for the spinner only.
    void OnTerminalProgramStatus(clCommandEvent& event);
    /// Show the spinner on our tab while the main agent or the reviewer works, remove it otherwise
    void UpdateWorkingIcon();
    void OnContextMenu(wxContextMenuEvent& event);
    void OnFindInFilesShowing(clFindInFilesEvent& event);
    void OnOpenResourceShowing(clOpenResourceEvent& event);
    /// Open `text` as a URL, folder, file or symbol (same as clicking a link in the terminal)
    void OpenText(const wxString& text);
    void RestartAgentHost();
    void OnShowTerminal(wxCommandEvent& event);
    void OnGrepWorkspace(wxCommandEvent& event);
    void OnGrepWorkspaceUI(wxUpdateUIEvent& event);
    /// First line of the terminal selection (empty if nothing is selected)
    wxString GetSelectedLine(wxTerminalViewCtrl* terminal) const;
    /// The terminal of the reviewer, or nullptr
    wxTerminalViewCtrl* GetReviewTerminal() const;
    /// The agent terminal (main or reviewer) that has the keyboard focus, or nullptr
    wxTerminalViewCtrl* GetFocusedTerminal() const;
    /// The agent terminal (main or reviewer) that has a selection (the focused one first), or nullptr. The selection
    /// is the mouse selection of the terminal or, if there is none, the text the application copied (OSC 52).
    wxTerminalViewCtrl* GetSelectionTerminal() const;

    // The toolbar
    /// (Re)create the tools. Called again when the theme changes, to load the icons of the new theme.
    void BuildToolbar();
    void OnBitmapsUpdated(clCommandEvent& event);
    void OnToolRestart(wxCommandEvent& event);
    /// The button and its drop-down arrow: a menu to pick the agent of the review (or to manage the running review)
    void OnToolReviewBuddy(wxAuiToolBarEvent& event);
    void OnToolCopy(wxCommandEvent& event);
    void OnToolPaste(wxCommandEvent& event);
    void OnToolCopyUI(wxUpdateUIEvent& event);
    /// The agent terminal (main or reviewer) that has something to copy (the focused one first), or nullptr: a mouse
    /// selection, or the text the application copied (OSC 52)
    wxTerminalViewCtrl* GetCopyTerminal() const;
    void OnToolFindInFiles(wxCommandEvent& event);
    void OnToolOpenResource(wxCommandEvent& event);
    void OnToolHasTerminalUI(wxUpdateUIEvent& event);
    void OnToolSelectionUI(wxUpdateUIEvent& event);
    /// Run the main frame command `name` (a menu item), with the dialog filled from our selection
    void RunFrameCommand(const char* name);
    void SearchInWorkspace(const wxString& text);
    std::optional<SystemPromptCli> BuildSystemPrompt(const AgentInfo& info);

    // Review Buddy: a second agent that reviews the unpushed work of the main one, in a pane next to it
    /// Can a review buddy be launched from this page? If not, `whyNot` says why.
    bool CanHaveReviewBuddy(wxString& whyNot);
    bool HasGitRepo();
    void LaunchReviewBuddy(AgentType reviewer);
    /// Starts the reviewer with `prompt` as its first message in `folder`, relative to the working directory.
    wxTerminalViewCtrl* StartReviewer(AgentType reviewer, const wxString& prompt, const wxString& folder);
    /// Destroys the reviewer's pane and terminal without changing focus or splitter state.
    void DestroyReviewPane();
    void CloseReviewBuddy();
    void OpenLatestReview();
    void DismissNotice();
    void FocusTerminal(wxTerminalViewCtrl* terminal);
    /// Append the items to manage the current review (resend, stop, open the latest review, close) to `menu`
    void AppendReviewControls(wxMenu& menu);

private:
    wxBookCtrlBase* m_book{nullptr};
    wxSplitterWindow* m_splitter{nullptr};
    wxInfoBar* m_infoBar{nullptr};
    // The panes of the splitter: each holds a terminal
    AgentTerminal* m_agentTerminal{nullptr};
    AgentTerminal* m_reviewAgentTerminal{nullptr};
    std::unique_ptr<ReviewBuddy> m_review;
    // The icon on our tab while the agent waits for the user. Removed when the page gets the focus.
    std::unique_ptr<TabAttentionIcon> m_attentionIcon;
    // The spinner on our tab while the agent works. It stays when the page gets the focus.
    std::unique_ptr<TabAttentionIcon> m_workingIcon;
    // True while a toolbar button runs a main frame command: the toolbar click does not need the terminal to have the
    // focus, so the dialog is filled from the terminal that has a selection
    bool m_fillFromSelection{false};
    AgentInfo m_agentInfo;
    // The last state that the main agent reported: the user is told only when the state changes
    wxTerminalProgramState m_lastAgentState{wxTerminalProgramState::Clear};
    // The same for the reviewer (Clear when there is no reviewer)
    wxTerminalProgramState m_lastReviewState{wxTerminalProgramState::Clear};
    // The result of the last "is this a git repository?" check of a remote working directory (cached, it is slow)
    bool m_hasGit{false};
    std::optional<std::chrono::steady_clock::time_point> m_lastGitCheck;
};
