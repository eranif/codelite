#pragma once

#include "ReviewLoop.hpp"
#include "ssh_account_info.h"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/timer.h>

class wxTerminalViewCtrl;

/// Runs a ReviewLoop for one AgentHostPage: the main agent's terminal plus the
/// reviewer's terminal next to it. It writes the request files and checks every
/// second (every few seconds over SSH) for the marker the loop waits for. The
/// first request goes to the reviewer on its command line, when the owner starts
/// it (see LaunchFn); later requests, and those for the main agent, are typed
/// into the running terminals. It never owns the terminals.
///
/// Remote files are read and written with clSFTPManager on the UI thread: that
/// class is not safe to use from another thread. Its Await...() functions wait
/// with std::future::get(): they do not run an event loop, so no event (timer,
/// menu, CallAfter) is handled while one of them runs. Still, the owner should
/// not destroy this object while IsBusy() is true: see AgentHostPage::CloseReviewBuddy().
class ReviewBuddy : public wxEvtHandler
{
public:
    struct Target {
        wxString workingDir;
        std::optional<SSHAccountInfo> sshAccount; // No value: the files are local
        wxString sessionName;                     // For the notifications
    };

    /// Starts the reviewer's agent with `prompt` as its first message in `folder`, relative to the working
    /// directory. The separate folder keeps its history isolated from the main agent's history.
    using LaunchFn = std::function<wxTerminalViewCtrl*(const wxString& prompt, const wxString& folder)>;

    /// Shows `message` in the main agent's page until the user closes it.
    /// `problem` is true when the loop needs the user, false when it is finished.
    using NoticeFn = std::function<void(const wxString& message, bool problem)>;

    /// Gives the keyboard focus to a terminal, if the user is working in this page. Called when a request is typed
    /// into a terminal: that agent is the active one now, and it may need an answer (a permission prompt).
    using FocusFn = std::function<void(wxTerminalViewCtrl* terminal)>;

    /// `isShown` tells whether the user is looking at the main agent's page right now; it decides whether a system
    /// notification is worth showing.
    ReviewBuddy(const Target& target,
                wxTerminalViewCtrl* main,
                LaunchFn launchReviewer,
                std::function<bool()> isShown,
                NoticeFn showNotice,
                FocusFn focusTerminal);
    ~ReviewBuddy() override;

    /// Writes the first request and starts the reviewer with it.
    void Begin();

    /// True while one of the functions of this class is running (Begin, Resend and the polling).
    bool IsBusy() const { return m_busyDepth > 0; }

    /// "<dir>/<name>", with the one separator that the remote hosts use too.
    static wxString JoinPath(const wxString& dir, const wxString& name);
    /// Is the working directory of `target` inside a git working tree (also a sub-folder of one, or a worktree)?
    /// It asks git, on the target host. It can block (a process, or an SSH round trip).
    static bool IsInsideGitRepo(const Target& target);

    /// The user can act on the loop only when it has started.
    bool IsRunning() const;
    bool HasStalled() const;
    wxString Describe() const;
    void Resend();
    void Stop();
    /// The comments file of the current round, relative to the working dir; empty before the loop has started.
    wxString CommentsPath() const;
    const Target& GetTarget() const { return m_target; }

private:
    using Actions = std::vector<ReviewLoop::Action>;

    /// Marks this object as busy for the life of the scope.
    struct BusyScope {
        explicit BusyScope(ReviewBuddy& owner)
            : m_owner(owner)
        {
            ++m_owner.m_busyDepth;
        }
        ~BusyScope() { --m_owner.m_busyDepth; }
        BusyScope(const BusyScope&) = delete;
        BusyScope& operator=(const BusyScope&) = delete;
        ReviewBuddy& m_owner;
    };

    void OnPoll(wxTimerEvent&);
    void OnEnterTimer(wxTimerEvent&);

    void Execute(Actions actions);
    void PasteLine(wxTerminalViewCtrl* terminal, const wxString& line);
    void Finished();
    /// Should NotifyUser() also show a modal dialog?
    enum class ShowDialog { No, Yes };
    /// Tells the user about the loop: a notice in the page (it stays until closed), the status bar, a system
    /// notification when they are not looking at this page, and, with ShowDialog::Yes, a modal dialog. The dialog is
    /// shown later, not inside this call: the caller may be a timer handler or the action loop of Execute(), and
    /// a modal dialog runs an event loop in which this object can be destroyed.
    void NotifyUser(const wxString& title, const wxString& message, bool problem, ShowDialog dialog = ShowDialog::No);

    bool WriteFile(const wxString& relPath, const wxString& text);
    bool FileExists(const wxString& relPath) const;
    wxString ReadFile(const wxString& relPath) const;
    wxString FullPath(const wxString& relPath) const;
    void AddIgnoreRule() const;
    void RemoveFolder(const wxString& relPath);
    void RemoveLocalFolder(const wxString& relPath);
    void RemoveRemoteFolder(const wxString& relPath);

    Target m_target;
    bool m_remote;
    wxTerminalViewCtrl* m_main;
    wxTerminalViewCtrl* m_reviewer{nullptr}; // Null until the reviewer started
    LaunchFn m_launchReviewer;
    std::function<bool()> m_isShown;
    FocusFn m_focusTerminal;
    NoticeFn m_showNotice;

    std::unique_ptr<ReviewLoop> m_loop;
    wxTimer m_pollTimer;
    wxTimer m_enterTimer;
    wxTerminalViewCtrl* m_enterTarget{nullptr};
    std::chrono::steady_clock::time_point m_lastProgress;
    int m_busyDepth{0};
    // The clean review folder is removed when this object is destroyed, after the reviewer pane has closed.
    wxString m_removeOnClose;
};
