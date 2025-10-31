#pragma once

#include "assistant/attributes.hpp"
#include "assistant/common/json.hpp"
#include "codelite_exports.h"

#include <algorithm>
#include <map>
#include <mutex>
#include <vector>
#include <wx/arrstr.h>

namespace llm
{
enum class PromptKind {
    kCommentGeneration,
    kGitCommitMessage,
    kReleaseNotesGenerate,
    kReleaseNotesMerge,
    kGitChangesCodeReview,
    kMax, // Must be last
};

inline std::string GetPromptString(PromptKind kind)
{
    switch (kind) {
    case llm::PromptKind::kReleaseNotesMerge:
        return "Git Release Notes: Merge";
    case llm::PromptKind::kReleaseNotesGenerate:
        return "Git Release Notes: Generate";
    case llm::PromptKind::kCommentGeneration:
        return "Comment Generation Prompt";
    case llm::PromptKind::kGitCommitMessage:
        return "Git Commit Message Prompt";
    case llm::PromptKind::kGitChangesCodeReview:
        return "Code Review Prompt";
    case llm::PromptKind::kMax:
        return "";
    }
    return "";
}

class WXDLLIMPEXP_SDK Config
{
public:
    Config();
    virtual ~Config();
    /**
     * @brief Retrieves the stored history as a {@code wxArrayString}.
     *
     * This method acquires a scoped lock on the internal mutex to ensure thread
     * safety, then copies each UTF‑8 string from {@code m_history} into a new
     * {@code wxArrayString}, converting each entry to a {@code wxString}.
     *
     * @return A {@code wxArrayString} containing all history entries. If the
     *         internal history container is empty, an empty {@code wxArrayString}
     *         is returned.
     */
    wxArrayString GetHistory() const
    {
        std::scoped_lock lk{m_mutex};
        if (m_history.empty()) {
            return {};
        }
        wxArrayString history;
        history.reserve(m_history.size());
        for (const auto& h : m_history) { history.Add(wxString::FromUTF8(h)); }
        return history;
    }
    /**
     * @brief Sets the history to the provided array of strings.
     *
     * This method replaces the current history with a new set of strings from the
     * provided wxArrayString. The operation is thread-safe and uses a scoped lock
     * to prevent race conditions. All strings are converted from wxString to
     * std::string using UTF-8 encoding.
     *
     * @param history A wxArrayString containing the new history entries to be stored.
     *                Each wxString element will be converted to std::string using UTF-8 encoding.
     *
     * @return void This function does not return a value.
     *
     * @note This method is thread-safe due to internal mutex locking.
     * @note The existing history is completely cleared before the new history is set.
     *
     * @par Example:
     * @code
     * wxArrayString newHistory;
     * newHistory.Add(wxT("First entry"));
     * newHistory.Add(wxT("Second entry"));
     * newHistory.Add(wxT("Third entry"));
     * myObject.SetHistory(newHistory);
     * @endcode
     *
     * @see GetHistory()
     * @see ClearHistory()
     */
    void SetHistory(const wxArrayString& history)
    {
        std::scoped_lock lk{m_mutex};
        m_history.clear();
        m_history.reserve(history.size());
        for (const auto& h : history) { m_history.push_back(h.ToStdString(wxConvUTF8)); }
    }

    /**
     * @brief Adds the given prompt to the configuration history.
     *
     * This method acquires a lock on the internal mutex, converts the provided
     * {@code wxString} prompt to a UTF‑8 encoded {@code std::string}, removes any
     * existing occurrence of the same prompt from the history, and inserts the new
     * prompt at the beginning of the {@code m_history} container.
     *
     * @param prompt The prompt string to be added to the history.
     */
    void AddHistory(const wxString& prompt);
    /**
     * @brief Loads the configuration from the associated file.
     *
     * This method acquires a lock on the internal mutex to ensure thread safety,
     * initializes the prompts and history to their default values, reads the
     * configuration file (UTF‑8), parses its JSON content, and populates the
     * `m_prompts` and `m_history` members.
     *
     * If the file cannot be read or the JSON is malformed, the method returns
     * early, leaving the defaults intact. Parsing errors are caught and logged
     * via `clERROR()`.
     *
     * @note The function does not propagate exceptions; any errors are handled
     * internally and reported through logging.
     */
    void Load();
    /**
     * @brief Saves the current configuration to disk.
     *
     * This method performs the following steps:
     * - Acquires an exclusive lock on the internal mutex to ensure thread‑safety.
     * - Truncates the history vector to {@code kMaxHistorySize} if it exceeds the limit.
     * - Merges any missing entries from {@code kDefaultPromptTable} into the stored prompts map.
     * - Serializes the prompts and history into a JSON object with pretty‑printing (indentation of 2 spaces).
     * - Writes the resulting JSON string to the configuration file using UTF‑8 encoding.
     *
     * The function does not take any parameters and does not return a value.
     *
     * @note The operation is atomic with respect to other threads accessing the configuration,
     *       thanks to the scoped lock on {@code m_mutex}.
     */
    void Save();
    /**
     * @brief Retrieves the prompt string associated with a given {@link PromptKind}.
     *
     * This method safely accesses the internal prompt storage (`m_prompts`) by
     * acquiring a scoped lock on {@code m_mutex}. It first checks whether the
     * requested {@code kind} is {@code PromptKind::kMax}, in which case an empty
     * string is returned. Otherwise it obtains the corresponding key via
     * {@link Config::GetPromptString}, looks up the key in the map, and returns
     * the stored UTF‑8 string converted to a {@code wxString}. If the key is not
     * present, an empty string is returned.
     *
     * @param kind The kind of prompt to retrieve.
     *
     * @return The prompt string for the requested {@code kind}, or an empty
     *         {@code wxString} if the kind is {@code PromptKind::kMax} or if no
     *         entry exists for the generated key.
     */
    wxString GetPrompt(PromptKind kind) const;
    /**
     * @brief Sets the prompt string for a specific {@code PromptKind}.
     *
     * Retrieves the label corresponding to {@code kind} via {@code GetPromptString},
     * then updates the internal prompt map with the provided {@code prompt}. The
     * update is performed while holding {@code m_mutex} to ensure thread safety.
     *
     * @param kind The type of prompt to set. {@code PromptKind::kMax} is ignored.
     * @param prompt The prompt text to associate with the specified {@code kind}.
     */
    void SetPrompt(PromptKind kind, const wxString& prompt);

    /**
     * @brief Resets the prompt collection to its default state.
     *
     * This method clears the current prompts stored in `m_prompts` and then
     * populates it with the predefined `kDefaultPromptTable`. It is used to
     * restore the default configuration of prompts.
     *
     * @note This function takes no parameters and returns no value.
     */
    void ResetPrompts();

private:
    wxString GetFullPath();

    mutable std::mutex m_mutex;
    std::map<std::string, std::string> m_prompts GUARDED_BY(m_mutex);
    mutable std::vector<std::string> m_history GUARDED_BY(m_mutex);
};
} // namespace llm
