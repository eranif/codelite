#pragma once

#include "AgentHostUI.hpp"
#include "cl_command_event.h"
#include "ssh_account_info.h"

#include <chrono>
#include <memory>
#include <optional>
#include <wx/infobar.h>
#include <wx/splitter.h>

class ReviewBuddy;
class wxTerminalViewCtrl;

enum class AgentType {
    kClaudeCode = 0,
    kKiroCli = 1,
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

class AgentHostPage : public AgentHostPageBase
{
public:
    AgentHostPage(wxBookCtrlBase* parent);
    ~AgentHostPage() override;

    /// The main agent's terminal
    wxTerminalViewCtrl* GetTerminal() { return m_terminal; }
    void StartAgentHost(const AgentInfo& info);

protected:
    void OnFocus(wxFocusEvent& event) override;
    void OnBookPageChanged(wxBookCtrlEvent& event);
    void OnThemeChanged(clCommandEvent& event);
    void OnTerminalLink(clCommandEvent& event);
    void OnTerminalBell(clCommandEvent& event);
    void OnTerminalTerminated(clCommandEvent& event);
    void OnTerminalTitleChanged(clCommandEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    /// Open `text` as a URL, folder, file or symbol (same as clicking a link in the terminal)
    void OpenText(const wxString& text);
    void RestartAgentHost();
    void OnShowTerminal(wxCommandEvent& event);
    void OnGrepWorkspace(wxCommandEvent& event);
    void OnGrepWorkspaceUI(wxUpdateUIEvent& event);
    /// First line of the terminal selection (empty if nothing is selected)
    wxString GetSelectedLine(wxTerminalViewCtrl* terminal) const;
    /// The agent terminal (main or reviewer) that has the keyboard focus, or nullptr
    wxTerminalViewCtrl* GetFocusedTerminal() const;
    void SearchInWorkspace(const wxString& text);
    std::optional<wxString> BuildSystemPrompt(const AgentInfo& info);

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
    void AppendReviewBuddyMenu(wxMenu& menu, wxTerminalViewCtrl* terminal);

private:
    wxBookCtrlBase* m_book{nullptr};
    wxSplitterWindow* m_splitter{nullptr};
    wxInfoBar* m_infoBar{nullptr};
    wxTerminalViewCtrl* m_terminal{nullptr};
    wxTerminalViewCtrl* m_reviewTerminal{nullptr};
    std::unique_ptr<ReviewBuddy> m_review;
    AgentInfo m_agentInfo;
    // The result of the last "is this a git repository?" check of a remote working directory (cached, it is slow)
    bool m_hasGit{false};
    std::optional<std::chrono::steady_clock::time_point> m_lastGitCheck;
};
