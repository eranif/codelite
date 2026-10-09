#include "ReviewLoop.hpp"

#include <cstdint>
#include <random>
#include <utility>
#include <wx/intl.h>

namespace
{
constexpr const char* kReviewsDir = ".agents/reviews";

// The one line typed into a terminal: it points at a file with the details, so
// the line itself stays short and the same for every agent.
wxString FollowLine(const wxString& file) { return "Follow the instructions in " + file; }

// "<folder>/<stem>-<n>.<ext>", e.g. ".agents/reviews/<id>/review-request-2.md".
wxString NumberedPath(const wxString& folder, const char* stem, int n, const char* ext)
{
    return wxString::Format("%s/%s-%d.%s", folder, stem, n, ext);
}

// `line` without the decoration around it that Markdown adds, e.g. "**STATUS: CLEAN**", "## STATUS: CLEAN" or
// "`STATUS: CLEAN`." gives "STATUS: CLEAN".
wxString StripDecoration(const wxString& line)
{
    static const wxString kDecoration = "*`#._ \t";
    const size_t first = line.find_first_not_of(kDecoration);
    if (first == wxString::npos) {
        return wxString{};
    }
    const size_t last = line.find_last_not_of(kDecoration);
    return line.Mid(first, last - first + 1);
}
} // namespace

ReviewLoop::ReviewLoop(wxString id, int maxRounds)
    : m_id(std::move(id))
    , m_maxRounds(maxRounds)
{
}

wxString ReviewLoop::Folder() const { return wxString::Format("%s/%s", kReviewsDir, m_id); }

wxString ReviewLoop::CommentsPath() const { return NumberedPath(Folder(), "review-comments", m_dir, "md"); }

wxString ReviewLoop::Describe() const
{
    switch (m_state) {
    case State::Idle:
        return _("Not started");
    case State::Reviewing:
        return wxString::Format(_("Round %d of %d: waiting for the review%s"),
                                m_round,
                                m_maxRounds,
                                m_slow ? _(" (for a long time)") : wxString{});
    case State::Fixing:
        return wxString::Format(_("Round %d of %d: waiting for the fixes%s"),
                                m_round,
                                m_maxRounds,
                                m_slow ? _(" (for a long time)") : wxString{});
    case State::Done:
    case State::LimitReached:
    case State::Stalled:
        return m_message;
    case State::Stopped:
        return _("Stopped");
    }
    return wxEmptyString;
}

wxString ReviewLoop::WatchedMarker() const
{
    switch (m_state) {
    case State::Reviewing:
        return NumberedPath(Folder(), "review-completed", m_dir, "marker");
    case State::Fixing:
        return NumberedPath(Folder(), "comments-addressed", m_dir, "marker");
    default:
        return wxEmptyString;
    }
}

wxString ReviewLoop::FileToRead() const { return m_state == State::Reviewing ? CommentsPath() : wxString{}; }

std::vector<ReviewLoop::Action> ReviewLoop::Start()
{
    m_round = 1;
    m_dir = 1;
    m_message.clear();
    m_state = State::Reviewing;
    return AskForReview();
}

std::vector<ReviewLoop::Action> ReviewLoop::AskForReview()
{
    const wxString file = NumberedPath(Folder(), "review-request", m_dir, "md");
    return {
        {Action::Kind::WriteFile, file, BuildReviewRequest(Folder(), m_dir, m_round)},
        {Action::Kind::PasteToReviewer, wxEmptyString, FollowLine(file)},
    };
}

std::vector<ReviewLoop::Action> ReviewLoop::AskForFix()
{
    const wxString file = NumberedPath(Folder(), "fix-request", m_dir, "md");
    return {
        {Action::Kind::WriteFile, file, BuildFixRequest(Folder(), m_dir, m_round)},
        {Action::Kind::PasteToMain, wxEmptyString, FollowLine(file)},
    };
}

std::vector<ReviewLoop::Action> ReviewLoop::Finish(State state, const wxString& message)
{
    m_state = state;
    m_message = message;
    return {{Action::Kind::Finished, wxEmptyString, message}};
}

std::vector<ReviewLoop::Action> ReviewLoop::OnMarkerFound(const wxString& content)
{
    m_slow = false;
    switch (m_state) {
    case State::Reviewing: {
        if (wxString(content).Trim().empty()) {
            return Fail(_("The review marker exists but review-comments.md is "
                          "missing or empty"));
        }
        if (ParseVerdict(content) == Verdict::Clean) {
            return Finish(State::Done, wxString::Format(_("Review is clean after %d round(s)"), m_round));
        }
        // "Findings" and "Unknown" both go to the main agent: a review that forgot
        // its STATUS line still has something to say, and the round limit stops a
        // pointless cycle.
        m_state = State::Fixing;
        return AskForFix();
    }
    case State::Fixing:
        if (m_round >= m_maxRounds) {
            // Not "Done": the user must not think that the code is clean
            return Finish(
                State::LimitReached,
                wxString::Format(_("Round limit (%d) reached. The last fixes were not reviewed"), m_maxRounds));
        }
        ++m_round;
        ++m_dir;
        m_state = State::Reviewing;
        return AskForReview();
    default:
        return {};
    }
}

std::vector<ReviewLoop::Action> ReviewLoop::OnSlow()
{
    if (!IsActive()) {
        return {};
    }
    m_slow = true;
    return {{Action::Kind::Notify,
             wxEmptyString,
             m_state == State::Reviewing ? _("No review for a long time. Is the reviewer waiting for "
                                             "you?")
                                         : _("No fixes for a long time. Is the main agent waiting for "
                                             "you?")}};
}

std::vector<ReviewLoop::Action> ReviewLoop::Fail(const wxString& why)
{
    // A late failure (a remote write that ends after the user pressed Stop) must
    // not bring an ended loop back.
    if (m_state == State::Stopped || m_state == State::Done || m_state == State::LimitReached) {
        return {};
    }
    if (IsActive()) {
        m_stalledFrom = m_state;
    }
    return Finish(State::Stalled, why);
}

std::vector<ReviewLoop::Action> ReviewLoop::Resend()
{
    State state = m_state == State::Stalled ? m_stalledFrom : m_state;
    if (state != State::Reviewing && state != State::Fixing) {
        return {};
    }
    m_message.clear();
    m_slow = false;
    m_state = state;
    if (state == State::Fixing) {
        return AskForFix();
    }
    // New file names, same round: the old ones may include a marker already.
    ++m_dir;
    return AskForReview();
}

void ReviewLoop::Stop()
{
    m_state = State::Stopped;
    m_message.clear();
}

ReviewLoop::Verdict ReviewLoop::ParseVerdict(const wxString& comments)
{
    // The first line that has text must be "STATUS: CLEAN" or "STATUS: FINDINGS". Markdown decoration around it is
    // ignored: "**STATUS: CLEAN**" and "STATUS: CLEAN." are fine.
    size_t pos = 0;
    while (pos < comments.length()) {
        size_t end = comments.find('\n', pos);
        if (end == wxString::npos) {
            end = comments.length();
        }
        wxString line = StripDecoration(comments.Mid(pos, end - pos).Trim().Trim(false));
        pos = end + 1;
        if (line.empty()) {
            continue;
        }
        line.MakeUpper();
        if (line == "STATUS: CLEAN") {
            return Verdict::Clean;
        }
        if (line == "STATUS: FINDINGS") {
            return Verdict::Findings;
        }
        return Verdict::Unknown;
    }
    return Verdict::Unknown;
}

wxString ReviewLoop::BuildReviewRequest(const wxString& folder, int n, int round)
{
    wxString text;
    text << "# Code review request (round " << round << ")\n\n"
         << "You are a code reviewer. Another agent wrote the code in this "
            "folder. Review its work that is **not pushed yet**:\n\n"
         << "- uncommitted changes in the working tree, staged and unstaged, and "
            "new untracked files (`git status`, `git diff HEAD`)\n"
         << "- commits that are not on the upstream branch "
            "(`git log @{upstream}..HEAD`, `git diff @{upstream}...HEAD`). If "
            "the branch has no upstream, compare with the default branch of the "
            "remote (for example `origin/main` or `origin/master`).\n\n"
         << "Ignore the `.agents/` folder: it holds the files of this review.\n";
    if (round > 1) {
        text << "\nThis is not the first round. First read the comments of the "
                "earlier rounds (`"
             << folder << "/review-comments-*.md`) and the answers of the other agent (`" << folder
             << "/response-*.md`, if any), and check that each finding was really "
                "addressed.\n";
    }
    text << "\n## Rules\n\n"
         << "- Do **not** change any file except the ones in `" << folder << "/`.\n"
         << "- Do not run git commands that change anything (no add, commit, "
            "push, checkout, stash, reset).\n\n"
         << "## What to write\n\n"
         << "1. Write your review to `" << NumberedPath(folder, "review-comments", n, "md") << "`.\n"
         << "   - The first line must be exactly `STATUS: FINDINGS` or "
            "`STATUS: CLEAN`.\n"
         << "   - If there are findings, list them: file and line, severity "
            "(blocker, major, minor, nit), the problem, and a suggested fix.\n"
         << "   - Use `STATUS: CLEAN` only if nothing needs to change anymore.\n"
         << "2. **Only after** the file is completely written, create the "
            "empty file `"
         << NumberedPath(folder, "review-completed", n, "marker") << "`.\n\n"
         << "Do these two steps in this order. Then stop and wait.\n";
    return text;
}

wxString ReviewLoop::BuildFixRequest(const wxString& folder, int n, int round)
{
    wxString text;
    text << "# Address the review comments (round " << round << ")\n\n"
         << "Another agent reviewed your work. Read `" << NumberedPath(folder, "review-comments", n, "md") << "`.\n\n"
         << "- Address every finding. If you disagree with one, do not change "
            "the code for it; explain why in `"
         << NumberedPath(folder, "response", n, "md") << "`.\n"
         << "- Do not edit anything under `.agents/reviews/` except that "
            "response file.\n"
         << "- When you are done, create the empty file `" << NumberedPath(folder, "comments-addressed", n, "marker")
         << "`.\n\n"
         << "Then stop and wait.\n";
    return text;
}

wxString ReviewLoop::NewId()
{
    // A random (version 4) UUID, RFC 4122 section 4.4: the version digit is 4 and
    // the two top bits of the variant digit are 10.
    std::random_device rd;
    std::mt19937_64 rng{(static_cast<uint64_t>(rd()) << 32) ^ rd()};
    const uint64_t a = rng();
    const uint64_t b = rng();
    return wxString::Format("%08x-%04x-4%03x-%04x-%012llx",
                            static_cast<unsigned>(a >> 32),
                            static_cast<unsigned>((a >> 16) & 0xffff),
                            static_cast<unsigned>(a & 0xfff),
                            static_cast<unsigned>(0x8000 | ((b >> 48) & 0x3fff)),
                            static_cast<unsigned long long>(b & 0xffffffffffffULL));
}
