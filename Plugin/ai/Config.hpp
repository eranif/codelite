#pragma once

// clang-format off
#include <wx/app.h>
// clang-format on

#include "assistant/attributes.hpp"
#include "assistant/client_base.hpp"
#include "assistant/common/json.hpp"
#include "codelite_exports.h"

#include <algorithm>
#include <map>
#include <mutex>
#include <optional>
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

inline std::string PromptKindToString(PromptKind kind)
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

using Conversation = std::vector<assistant::Message>;
using ChatHistory = std::vector<Conversation>;

class WXDLLIMPEXP_SDK Config
{
public:
    Config();
    virtual ~Config();

    /**
     * @brief Extracts a label for a conversation based on the first message's text.
     *
     * @details This function derives a conversation label by taking the first line of the
     * first message in the conversation history, trimming any leading and trailing whitespace.
     * The label can be used for display purposes such as conversation list titles or tabs.
     *
     * @param history The conversation history containing messages to extract the label from.
     *
     * @return An optional string containing the trimmed first line of the first message's text,
     *         or std::nullopt if the conversation history is empty.
     *
     * @see Conversation
     */
    static std::optional<std::string> GetConversationLabel(const Conversation& history)
    {
        if (history.empty()) {
            return std::nullopt;
        }
        wxString first_prompt = wxString::FromUTF8(history[0].text);
        first_prompt.Trim().Trim(false);
        first_prompt = first_prompt.BeforeFirst('\n').Trim().Trim(false);
        return first_prompt.ToStdString(wxConvUTF8);
    }

    /**
     * @brief Retrieves a copy of the current chat history.
     *
     * This method provides thread-safe access to the chat history by acquiring
     * a lock on the internal mutex before copying the history data.
     *
     * @return ChatHistory A copy of the current chat history stored in this object.
     *
     * @note This method is thread-safe and can be called concurrently from multiple threads.
     *
     * @see ChatHistory
     */
    ChatHistory GetHistory() const
    {
        std::scoped_lock lk{m_mutex};
        return m_history;
    }

    /**
     * @brief Adds a conversation to the configuration's history, replacing any existing conversation with the same
     * label.
     *
     * @details This method is thread-safe and acquires a lock before modifying the history.
     * If a conversation with the same label already exists in the history, it is removed before
     * inserting the new conversation at the beginning of the history list. If the conversation
     * has no valid label, the operation is silently aborted.
     *
     * @param conversation The conversation to add to the history. Must have a valid label
     *                     obtainable via GetConversationLabel() for the operation to proceed.
     *
     * @return void This function does not return a value.
     *
     * @note The conversation is always inserted at the front of the history list, effectively
     *       making it the most recent entry.
     *
     * @see GetConversationLabel() For obtaining the label used to identify duplicate conversations.
     */
    void AddConversation(const Conversation& conversation);

    /**
     * @brief Sets the chat history to the specified value.
     *
     * Replaces the current chat history with a copy of the provided history.
     * This operation is thread-safe and acquires an exclusive lock on the internal mutex.
     *
     * @param history The chat history to set. The contents are copied into the internal storage.
     *
     * @return void This function does not return a value.
     *
     * @note This function is thread-safe.
     *
     * @see ChatHistory
     * @see GetHistory
     */
    void SetHistory(const ChatHistory& history)
    {
        std::scoped_lock lk{m_mutex};
        m_history = history;
    }

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
    mutable ChatHistory m_history GUARDED_BY(m_mutex);
};
} // namespace llm
