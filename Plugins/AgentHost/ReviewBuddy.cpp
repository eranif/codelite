#include "ReviewBuddy.hpp"

#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include "procutils.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

#include <functional>
#include <utility>
#include <wx/app.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/notifmsg.h>
#include <wx/toplevel.h>
#include <wx/utils.h>

namespace
{
constexpr int kLocalPollMs = 1000;
constexpr int kRemotePollMs = 4000;
// The delay between typing a line and pressing Enter: some TUIs take text and Enter that arrive together as one
// paste, and do not submit it.
constexpr int kEnterDelayMs = 400;
constexpr auto kStallTimeout = std::chrono::minutes(20);
// On Windows the reviewer's process may need a moment to let go of its working directory
constexpr int kRemoveRetryMs = 3000;
// Keeps our files out of `git status`. It has no leading slash: the agents may run in a sub-folder of the repository.
const wxString kExcludeRule = "**/.agents/reviews/";

/// A timer that runs a function once and then deletes itself. wxWidgets (3.2 and 3.3) has no `CallLater` for a
/// wxEvtHandler, and the owner of the function (ReviewBuddy) is gone when it runs.
class OneShotTimer : public wxTimer
{
public:
    explicit OneShotTimer(std::function<void()> func)
        : m_func(std::move(func))
    {
    }

    void Notify() override
    {
        // The application is shutting down: do nothing. The timer object is lost, but so is the whole process.
        if (wxTheApp == nullptr) {
            return;
        }
        m_func();
        // Not "delete this": we are inside the timer's own callback
        wxTheApp->CallAfter([this]() { delete this; });
    }

private:
    std::function<void()> m_func;
};

/// Runs `func` once, after `ms` milliseconds, if the application still runs by then.
void RunLater(int ms, std::function<void()> func) { (new OneShotTimer(std::move(func)))->StartOnce(ms); }

/// Removes the review folder `relFolder` of the project in `projectDir`, then the `.agents/reviews` and `.agents`
/// folders if they are empty. A symbolic link is never followed: if the folder, or one of its two parents, is a link,
/// nothing is removed. Returns false when the folder could not be removed and it is worth trying again later (the
/// reviewer's process may still use it).
bool RemoveLocalReviewFolder(const wxString& projectDir, const wxString& relFolder)
{
    const wxString folder = ReviewBuddy::JoinPath(projectDir, relFolder);
    const wxString reviews = ReviewBuddy::JoinPath(projectDir, ".agents/reviews");
    const wxString agents = ReviewBuddy::JoinPath(projectDir, ".agents");
    for (const wxString& path : {agents, reviews, folder}) {
        if (wxFileName::Exists(path, wxFILE_EXISTS_SYMLINK | wxFILE_EXISTS_NO_FOLLOW)) {
            clWARNING() << "Review buddy: refusing to remove, a symbolic link is in the way:" << path << endl;
            return true; // Nothing to retry
        }
    }

    if (wxDir::Exists(folder) && !wxFileName::Rmdir(folder, wxPATH_RMDIR_RECURSIVE)) {
        return false;
    }

    // Remove the parents only when they are empty, leaving other review folders untouched
    for (const wxString& parent : {reviews, agents}) {
        wxDir dir(parent);
        if (dir.IsOpened() && !dir.HasFiles() && !dir.HasSubDirs()) {
            dir.Close();
            wxFileName::Rmdir(parent);
        }
    }
    return true;
}

#ifdef __WXOSX__
/// `text` as an AppleScript string literal (in double quotes).
wxString AppleScriptString(const wxString& text)
{
    wxString quoted = "\"";
    for (const wxUniChar c : text) {
        if (c == '"' || c == '\\') {
            quoted << '\\';
            quoted << c;
        } else if (c == '\n' || c == '\r') {
            quoted << ' ';
        } else {
            quoted << c;
        }
    }
    quoted << '"';
    return quoted;
}

/// wxNotificationMessage uses NSUserNotification here, which macOS has deprecated for years, and the notification
/// does not show up. This one goes through osascript. The arguments are passed as a list, not through a shell.
void ShowMacNotification(const wxString& title, const wxString& text)
{
    const std::string script =
        ("display notification " + AppleScriptString(text) + " with title " + AppleScriptString(title)).utf8_string();
    const char* argv[] = {"osascript", "-e", script.c_str(), nullptr};
    wxExecute(argv, wxEXEC_ASYNC | wxEXEC_NODISABLE);
}
#endif
} // namespace

wxString ReviewBuddy::JoinPath(const wxString& dir, const wxString& name)
{
    return dir.EndsWith("/") || dir.EndsWith("\\") ? dir + name : dir + "/" + name;
}

bool ReviewBuddy::IsInsideGitRepo(const Target& target)
{
    if (target.workingDir.empty()) {
        return false;
    }
    const wxString command = "git rev-parse --is-inside-work-tree";
    if (target.sshAccount.has_value()) {
#if USE_SFTP
        // When the connection fails, the result is empty with exit code 0: so the output is checked too
        const auto result =
            clSFTPManager::Get().AwaitExecute(target.sshAccount->GetAccountName(), command, target.workingDir);
        return std::get<2>(result) == 0 && wxString::FromUTF8(std::get<0>(result)).Trim().Trim(false) == "true";
#else
        return false;
#endif
    }

    wxArrayString output;
    if (ProcUtils::SafeExecuteShellCommand(command, target.workingDir, output) != 0 || output.empty()) {
        return false;
    }
    return output[0].Trim().Trim(false) == "true";
}

ReviewBuddy::ReviewBuddy(const Target& target,
                         wxTerminalViewCtrl* main,
                         LaunchFn launchReviewer,
                         std::function<bool()> isShown,
                         NoticeFn showNotice,
                         FocusFn focusTerminal)
    : m_target(target)
    , m_remote(target.sshAccount.has_value())
    , m_main(main)
    , m_launchReviewer(std::move(launchReviewer))
    , m_isShown(std::move(isShown))
    , m_focusTerminal(std::move(focusTerminal))
    , m_showNotice(std::move(showNotice))
    , m_pollTimer(this)
    , m_enterTimer(this)
{
    Bind(wxEVT_TIMER, &ReviewBuddy::OnPoll, this, m_pollTimer.GetId());
    Bind(wxEVT_TIMER, &ReviewBuddy::OnEnterTimer, this, m_enterTimer.GetId());
}

ReviewBuddy::~ReviewBuddy()
{
    m_pollTimer.Stop();
    m_enterTimer.Stop();
    if (!m_removeOnClose.empty()) {
        RemoveFolder(m_removeOnClose);
    }
}

bool ReviewBuddy::IsRunning() const { return m_loop && m_loop->IsActive(); }

bool ReviewBuddy::HasStalled() const { return m_loop && m_loop->GetState() == ReviewLoop::State::Stalled; }

wxString ReviewBuddy::Describe() const { return m_loop ? m_loop->Describe() : _("Starting"); }

wxString ReviewBuddy::CommentsPath() const { return m_loop ? m_loop->CommentsPath() : wxString{}; }

void ReviewBuddy::Begin()
{
    if (m_loop) {
        return;
    }
    BusyScope busy(*this);
    m_loop = std::make_unique<ReviewLoop>(ReviewLoop::NewId());
    clDEBUG() << "Review buddy" << m_loop->Id() << "starts in" << m_target.workingDir << (m_remote ? "(remote)" : "")
              << endl;
    AddIgnoreRule();
    m_pollTimer.Start(m_remote ? kRemotePollMs : kLocalPollMs);
    m_lastProgress = std::chrono::steady_clock::now();
    Execute(m_loop->Start());
}

void ReviewBuddy::Resend()
{
    if (!m_loop) {
        return;
    }
    BusyScope busy(*this);
    const bool wasStalled = HasStalled();
    Execute(m_loop->Resend());
    if (wasStalled && IsRunning()) {
        m_pollTimer.Start(m_remote ? kRemotePollMs : kLocalPollMs);
    }
}

void ReviewBuddy::Stop()
{
    if (m_loop) {
        m_loop->Stop();
    }
    m_pollTimer.Stop();
    m_enterTimer.Stop();
    m_enterTarget = nullptr;
}

// ---------------------------------------------------------------------------
// Polling
// ---------------------------------------------------------------------------

void ReviewBuddy::OnPoll(wxTimerEvent&)
{
    if (!IsRunning()) {
        return;
    }
    BusyScope busy(*this);
    if (std::chrono::steady_clock::now() - m_lastProgress > kStallTimeout) {
        Execute(m_loop->OnSlow());
        return;
    }

    const wxString marker = m_loop->WatchedMarker();
    if (marker.empty() || !FileExists(marker)) {
        return;
    }
    const wxString fileToRead = m_loop->FileToRead();
    const wxString content = fileToRead.empty() ? wxString{} : ReadFile(fileToRead);
    Execute(m_loop->OnMarkerFound(content));
}

// ---------------------------------------------------------------------------
// Doing what the loop asks for
// ---------------------------------------------------------------------------

void ReviewBuddy::Execute(Actions actions)
{
    m_lastProgress = std::chrono::steady_clock::now();
    for (const auto& action : actions) {
        switch (action.kind) {
        case ReviewLoop::Action::Kind::WriteFile:
            if (!WriteFile(action.path, action.text)) {
                Execute(m_loop->Fail(_("Could not write ") + action.path));
                return;
            }
            break;
        case ReviewLoop::Action::Kind::PasteToReviewer:
            if (m_reviewer == nullptr) {
                // The first request: the reviewer starts with it as its first message, so there is nothing to wait
                // for. The request file is written already (the actions run in order).
                m_reviewer = m_launchReviewer ? m_launchReviewer(action.text, m_loop->Folder()) : nullptr;
                if (m_reviewer == nullptr) {
                    Execute(m_loop->Fail(_("Could not start the reviewer")));
                    return;
                }
            } else {
                PasteLine(m_reviewer, action.text);
            }
            break;
        case ReviewLoop::Action::Kind::PasteToMain:
            PasteLine(m_main, action.text);
            break;
        case ReviewLoop::Action::Kind::RemoveFolder:
            if (!ReviewLoop::IsReviewFolder(action.path)) {
                clWARNING() << "Review buddy: refusing to remove unexpected folder" << action.path << endl;
                break;
            }
            // The reviewer runs in this folder, so leave it until its pane is closed.
            m_removeOnClose = action.path;
            break;
        case ReviewLoop::Action::Kind::Notify:
            clDEBUG() << "Review buddy:" << action.text << endl;
            NotifyUser(_("Review needs your attention"), action.text, true);
            break;
        case ReviewLoop::Action::Kind::Finished:
            Finished();
            break;
        }
    }
    clDEBUG() << "Review buddy:" << m_loop->Describe() << endl;
}

void ReviewBuddy::PasteLine(wxTerminalViewCtrl* terminal, const wxString& line)
{
    if (terminal == nullptr) {
        return;
    }
    terminal->SendInput(line.ToStdString(wxConvUTF8));
    m_enterTarget = terminal;
    m_enterTimer.StartOnce(kEnterDelayMs);
    // This agent is the active one now.
    if (m_focusTerminal) {
        m_focusTerminal(terminal);
    }
}

void ReviewBuddy::OnEnterTimer(wxTimerEvent&)
{
    if (m_enterTarget != nullptr) {
        m_enterTarget->SendEnter();
        m_enterTarget = nullptr;
    }
}

void ReviewBuddy::Finished()
{
    m_pollTimer.Stop();
    clDEBUG() << "Review buddy ended:" << m_loop->Message() << endl;
    const bool done = m_loop->GetState() == ReviewLoop::State::Done;
    NotifyUser(done ? _("Review finished") : _("Review needs your attention"), m_loop->Message(), !done);
}

void ReviewBuddy::NotifyUser(const wxString& title, const wxString& message, bool problem)
{
    if (wxTheApp == nullptr) {
        return;
    }
    const wxString text = m_target.sessionName.empty() ? message : m_target.sessionName + ": " + message;

    // In the page, until the user closes it. It leaves out the session name (in `text`) on purpose: it is shown
    // inside that page.
    if (m_showNotice) {
        m_showNotice(title + " - " + message, problem);
    }
    // The status bar. Other activity may overwrite it soon.
    clGetManager()->SetStatusMessage(title + " - " + text, 10);

    // Outside the page the user is looking at: a system notification.
    const bool looking = wxTheApp->IsActive() && m_isShown && m_isShown();
    if (!looking) {
#ifdef __WXOSX__
        ShowMacNotification(title, text);
#else
        wxNotificationMessage notification(
            title, text, wxTheApp->GetTopWindow(), problem ? wxICON_WARNING : wxICON_INFORMATION);
        notification.Show();
#endif
    }

    // CodeLite is in the background: the Dock icon bounces on macOS, the taskbar button flashes on Windows.
    if (!wxTheApp->IsActive()) {
        if (auto* frame = dynamic_cast<wxTopLevelWindow*>(wxTheApp->GetTopWindow())) {
            frame->RequestUserAttention(wxUSER_ATTENTION_INFO);
        }
    }
}

// ---------------------------------------------------------------------------
// Files
// ---------------------------------------------------------------------------

wxString ReviewBuddy::FullPath(const wxString& relPath) const
{
    // The owner makes sure that the working directory is not empty
    return JoinPath(m_target.workingDir, relPath);
}

bool ReviewBuddy::WriteFile(const wxString& relPath, const wxString& text)
{
    const wxString path = FullPath(relPath);
    if (m_remote) {
#if USE_SFTP
        const wxString account = m_target.sshAccount->GetAccountName();
        // The SFTP write does not create folders
        const wxString dir = path.BeforeLast('/');
        const auto result = clSFTPManager::Get().AwaitExecute(
            account, "mkdir -p " + StringUtils::WrapWithDoubleQuotes(dir), wxEmptyString);
        if (std::get<2>(result) != 0) {
            clWARNING() << "Review buddy: mkdir failed:" << std::get<1>(result) << endl;
        }
        return clSFTPManager::Get().AwaitWriteFile(text, path, account);
#else
        return false;
#endif
    }

    wxFileName fn(path);
    // Mkdir() of a wxFileName makes its folder part (GetPath()), not the file.
    if (!fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
        return false;
    }
    wxFFile file(fn.GetFullPath(), "wb");
    return file.IsOpened() && file.Write(text, wxConvUTF8);
}

bool ReviewBuddy::FileExists(const wxString& relPath) const
{
    const wxString path = FullPath(relPath);
    if (m_remote) {
#if USE_SFTP
        return clSFTPManager::Get().IsFileExists(path, m_target.sshAccount->GetAccountName());
#else
        return false;
#endif
    }
    return wxFileName::FileExists(path);
}

wxString ReviewBuddy::ReadFile(const wxString& relPath) const
{
    const wxString path = FullPath(relPath);
    if (m_remote) {
#if USE_SFTP
        wxMemoryBuffer buffer;
        if (!clSFTPManager::Get().AwaitReadFile(path, m_target.sshAccount->GetAccountName(), &buffer)) {
            return wxString{};
        }
        return wxString::FromUTF8(static_cast<const char*>(buffer.GetData()), buffer.GetDataLen());
#else
        return wxString{};
#endif
    }

    wxFFile file(path, "rb");
    wxString content;
    if (file.IsOpened()) {
        file.ReadAll(&content, wxConvUTF8);
    }
    return content;
}

void ReviewBuddy::RemoveFolder(const wxString& relPath)
{
    if (m_remote) {
        RemoveRemoteFolder(relPath);
    } else {
        RemoveLocalFolder(relPath);
    }
}

void ReviewBuddy::RemoveLocalFolder(const wxString& relPath)
{
    if (RemoveLocalReviewFolder(m_target.workingDir, relPath)) {
        return;
    }

    // Try once more later. This object is gone by then, so the retry uses copies of what it needs. If it fails again,
    // or the application ends first, the folder stays: it is safe to delete.
    clWARNING() << "Review buddy: could not remove" << relPath << ", will try again" << endl;
    RunLater(kRemoveRetryMs, [workingDir = m_target.workingDir, relPath]() {
        if (!RemoveLocalReviewFolder(workingDir, relPath)) {
            clWARNING() << "Review buddy: could not remove" << relPath << ". It is safe to delete it" << endl;
        }
    });
}

void ReviewBuddy::RemoveRemoteFolder(const wxString& relPath)
{
#if USE_SFTP
    // This waits for one SSH command on the UI thread, in the destructor. It is accepted: the folder can be removed
    // only after the reviewer's pane is closed, and the destructor is the last place that knows about the folder.
    // Same rules as for a local folder: no symbolic links in the way, and the parents go only when they are empty.
    // The command is for a POSIX shell: the remote host is Unix. When a link is in the way, the checks fail and
    // nothing is removed; the warning below is expected then.
    const wxString folder = StringUtils::WrapWithDoubleQuotes(relPath);
    const wxString command = "[ ! -L .agents ] && [ ! -L .agents/reviews ] && [ ! -L " + folder + " ] && rm -rf " +
                             folder + " && { rmdir .agents/reviews .agents 2>/dev/null || true; }";
    const auto result =
        clSFTPManager::Get().AwaitExecute(m_target.sshAccount->GetAccountName(), command, m_target.workingDir);
    if (std::get<2>(result) != 0) {
        clWARNING() << "Review buddy: could not remove remote folder" << relPath << endl;
    }
#else
    wxUnusedVar(relPath);
#endif
}

void ReviewBuddy::AddIgnoreRule() const
{
    // Keep our files out of `git status`, without touching the repository's own .gitignore. Best effort.
    // `git rev-parse --git-path` finds the file for a sub-folder of the repository and for a worktree too.
    if (m_remote) {
#if USE_SFTP
        const wxString command = "f=$(git rev-parse --git-path info/exclude 2>/dev/null) && [ -n \"$f\" ] && "
                                 "mkdir -p \"$(dirname \"$f\")\" && { grep -qxF '" +
                                 kExcludeRule + "' \"$f\" 2>/dev/null || echo '" + kExcludeRule + "' >> \"$f\"; }";
        clSFTPManager::Get().AwaitExecute(m_target.sshAccount->GetAccountName(), command, m_target.workingDir);
#endif
        return;
    }

    wxArrayString output;
    if (ProcUtils::SafeExecuteShellCommand("git rev-parse --git-path info/exclude", m_target.workingDir, output) != 0 ||
        output.empty()) {
        return;
    }
    wxString exclude = output[0].Trim().Trim(false);
    if (exclude.empty()) {
        return;
    }
    // git prints a path relative to the working directory
    if (wxFileName(exclude).IsRelative()) {
        exclude = FullPath(exclude);
    }

    wxString content;
    if (wxFileName::FileExists(exclude)) {
        wxFFile in(exclude, "rb");
        if (in.IsOpened()) {
            in.ReadAll(&content, wxConvUTF8);
        }
    }
    if (content.Contains(kExcludeRule)) {
        return;
    }
    wxFileName::Mkdir(wxFileName(exclude).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    wxFFile out(exclude, "ab");
    if (out.IsOpened()) {
        out.Write(wxString(content.empty() || content.EndsWith("\n") ? "" : "\n") + kExcludeRule + "\n", wxConvUTF8);
    }
}
