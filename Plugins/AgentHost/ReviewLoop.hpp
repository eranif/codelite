#pragma once

#include <vector>
#include <wx/string.h>

// The "review buddy" cycle between two agents that work in the same folder:
//
//   reviewer: reviews the unpushed work, writes review-comments-N.md and then
//             review-completed-N.marker
//   main:     reads the comments, fixes them, then writes
//             comments-addressed-N.marker
//   ...and again, until the reviewer says "STATUS: CLEAN".
//
// All files of a loop are in one folder, .agents/reviews/<id>/, and a round is
// told apart by the number in the file name. One folder, because some tools ask
// for permission to write into each new folder.
//
// This class is the pure state machine: no I/O, no GUI. The owner polls for the
// file named by WatchedMarker(), reports it with OnMarkerFound(), and carries
// out the Actions it gets back (write a file, type a line into a terminal).
// All paths are relative to the agents' working directory and use '/'.
class ReviewLoop
{
public:
    enum class State {
        Idle,         // Not started
        Reviewing,    // Waiting for the reviewer to finish round N
        Fixing,       // Waiting for the main agent to address round N
        Done,         // The reviewer found nothing
        LimitReached, // The round limit was reached: the last fixes were never reviewed (not a success)
        Stalled,      // Something failed, see Message(); Resend() tries again
        Stopped,      // The user stopped it
    };

    enum class Verdict { Clean, Findings, Unknown };

    struct Action {
        enum class Kind {
            WriteFile,       // Write `text` to `path`, creating folders as needed
            PasteToReviewer, // Type the line `text` into the reviewer's terminal
            PasteToMain,     // Type the line `text` into the main agent's terminal
            RemoveFolder,    // Delete the completed loop's folder after the reviewer pane closes
            Notify,          // Still waiting after a long time: tell the user
            Finished,        // The loop ended (Done, LimitReached or Stalled): tell the user
        };
        Kind kind;
        wxString path;
        wxString text;
    };

    static constexpr int kDefaultMaxRounds = 5;

    explicit ReviewLoop(wxString id, int maxRounds = kDefaultMaxRounds);

    const wxString& Id() const { return m_id; }
    State GetState() const { return m_state; }
    int Round() const { return m_round; }
    int MaxRounds() const { return m_maxRounds; }
    // Reviewing or Fixing: a marker is expected.
    bool IsActive() const { return m_state == State::Reviewing || m_state == State::Fixing; }
    // Why the loop ended or stalled. Empty otherwise.
    const wxString& Message() const { return m_message; }
    // Nothing happened for a long time (cleared by the next marker).
    bool IsSlow() const { return m_slow; }
    // One line for the user, e.g. "Round 2 of 5: waiting for the review".
    wxString Describe() const;

    // Begins round 1.
    std::vector<Action> Start();
    // The marker the loop waits for now; empty when it waits for nothing.
    wxString WatchedMarker() const;
    // The file OnMarkerFound() wants the content of; empty when it wants none.
    wxString FileToRead() const;
    // The marker exists. `content` is the content of FileToRead() ("" if none).
    std::vector<Action> OnMarkerFound(const wxString& content);
    // Nothing happened for a long time. The loop keeps waiting: a late marker
    // still counts. Returns a Notify action, and again after each such time.
    std::vector<Action> OnSlow();
    // Something the owner needs failed (writing a file, ...).
    std::vector<Action> Fail(const wxString& why);
    // Asks again for what the loop waits for (also leaves Stalled). A review that
    // is asked for again gets new file names (same round), so an old marker
    // cannot answer it.
    std::vector<Action> Resend();
    void Stop();

    // The folder of the loop: ".agents/reviews/<id>".
    wxString Folder() const;
    // Where the reviewer writes the comments of the current round.
    wxString CommentsPath() const;
    // Whether `path` is a review folder created by NewId(). The owner checks this before deleting it.
    static bool IsReviewFolder(const wxString& path);

    static Verdict ParseVerdict(const wxString& comments);
    // `n` is the number in the file names, `round` the round shown to the user.
    // They differ after a review was asked for again: "Round 3 of 5" can be
    // review-request-4.md.
    static wxString BuildReviewRequest(const wxString& folder, int n, int round);
    static wxString BuildFixRequest(const wxString& folder, int n, int round);
    // A random UUID-like text, e.g. "3f2c1d9e-...".
    static wxString NewId();

private:
    std::vector<Action> AskForReview();
    std::vector<Action> AskForFix();
    std::vector<Action> Finish(State state, const wxString& message);

    wxString m_id;
    int m_maxRounds;
    State m_state{State::Idle};
    State m_stalledFrom{State::Idle};
    int m_round{0};
    int m_dir{0}; // The number in the current file names, see Build...Request()
    bool m_slow{false};
    wxString m_message;
};
