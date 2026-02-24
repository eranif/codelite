#pragma once

// clang-format off
#include <wx/app.h>
// clang-format on

#include "assistant/attributes.hpp"
#include "assistant/client/client_base.hpp"
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

// Cache Policy
inline static wxString kCacheAuto = "Auto";
inline static wxString kCacheNone = "None";
inline static wxString kCacheStaticContent = "Static Content";

using CachePolicy = assistant::CachePolicy;

inline wxString CachePolicyToString(CachePolicy cache_policy)
{
    switch (cache_policy) {
    case assistant::CachePolicy::kNone:
        return llm::kCacheNone;
    case assistant::CachePolicy::kStatic:
        return llm::kCacheStaticContent;
    case assistant::CachePolicy::kAuto:
    default:
        return llm::kCacheAuto;
    }
}

inline CachePolicy CachePolicyFromString(const wxString& policy)
{
    if (policy == llm::kCacheNone) {
        return CachePolicy::kNone;
    } else if (policy == llm::kCacheStaticContent) {
        return CachePolicy::kStatic;
    } else {
        return CachePolicy::kAuto;
    }
}

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

using nlohmann::json;
struct WXDLLIMPEXP_SDK Conversation {
    std::vector<assistant::Message> messages;
    Conversation() = default;
    ~Conversation() = default;
    Conversation(const std::vector<assistant::Message>& msgs)
        : messages(msgs)
    {
    }

    json to_json() const
    {
        auto messages_json = json::array();
        for (const auto& msg : messages) {
            json m = json::object();
            m["role"] = msg.role;
            m["content"] = msg.text;
            messages_json.push_back(m);
        }
        return messages_json;
    }

    static std::optional<Conversation> from_json(json j)
    {
        try {
            Conversation c;
            for (const auto& entry : j) {
                assistant::Message msg;
                msg.role = entry["role"].get<std::string>();
                msg.text = entry["content"].get<std::string>();
                c.messages.push_back(msg);
            }
            return c;
        } catch (...) {
            return std::nullopt;
        }
    }

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
    std::optional<std::string> GetConversationLabel() const
    {
        if (messages.empty()) {
            return std::nullopt;
        }
        wxString first_prompt = wxString::FromUTF8(messages[0].text);
        first_prompt.Trim().Trim(false);
        first_prompt = first_prompt.BeforeFirst('\n').Trim().Trim(false);
        return first_prompt.ToStdString(wxConvUTF8);
    }
};

struct ChatHistory {
    std::vector<Conversation> conversations;
    json to_json() const
    {

        // {
        //      "conv1" : [{role, text}...],
        //      ...
        //      "convN" : [{role, text}...]
        //  }

        nlohmann::json conv_json = nlohmann::json::object();
        for (const auto& conv : conversations) {
            auto label = conv.GetConversationLabel();
            if (!label.has_value()) {
                continue;
            }
            conv_json[label.value()] = conv.to_json();
        }
        return conv_json;
    }

    static std::optional<ChatHistory> from_json(json j)
    {
        try {
            ChatHistory h;
            if (!j.is_object()) {
                return std::nullopt;
            }
            for (const auto& kv : j.items()) {
                auto res = Conversation::from_json(kv.value());
                if (!res.has_value()) {
                    return std::nullopt;
                }
                h.conversations.push_back(res.value());
            }
            return h;
        } catch (...) {
            return std::nullopt;
        }
    }

    /**
     * @brief Finds a conversation by its label.
     *
     * Searches through the conversations collection for a conversation with a matching label.
     * The search performs an exact string comparison against each conversation's label.
     *
     * @param label The label string to search for in the conversations collection.
     *
     * @return std::optional<Conversation> Returns the matching Conversation if found,
     *         or std::nullopt if no conversation with the specified label exists.
     *
     * @note This function performs a linear search through all conversations.
     */
    std::optional<Conversation> FindConversation(const wxString& label)
    {
        auto iter = std::find_if(conversations.begin(), conversations.end(), [&label](const Conversation& h) {
            auto l = h.GetConversationLabel();
            return l.has_value() && l.value() == label;
        });
        if (iter == conversations.end()) {
            return std::nullopt;
        }
        return *iter;
    }

    /**
     * @brief Deletes a conversation from the collection by its label.
     *
     * Searches through the conversations collection for a conversation with a matching label
     * and removes it. If no conversation with the specified label exists, the behavior is undefined
     * (attempting to erase an invalid iterator).
     *
     * @param label The label of the conversation to delete.
     *
     * @return void This function does not return a value.
     *
     * @note If no conversation with the given label is found, this function will cause undefined
     *       behavior by attempting to erase an invalid iterator (conversations.end()).
     *
     * @see Conversation::GetConversationLabel()
     */
    void DeleteConversation(const wxString& label)
    {
        auto iter = std::find_if(conversations.begin(), conversations.end(), [&label](const Conversation& h) {
            auto l = h.GetConversationLabel();
            return l.has_value() && l.value() == label;
        });
        if (iter != conversations.end()) {
            conversations.erase(iter);
        }
    }

    /**
     * @brief Adds a conversation to the collection, replacing any existing conversation with the same label.
     *
     * This method first retrieves the conversation label; if the label is absent, the function returns early
     * without modifying the collection. If a label exists, any existing conversation with that label is removed
     * before inserting the new conversation at the beginning of the collection.
     *
     * @param conversation The Conversation object to be added. Must have a valid conversation label to be inserted.
     *
     * @return void This function does not return a value.
     *
     * @note If the conversation does not have a label (i.e., GetConversationLabel() returns std::nullopt),
     *       the function returns immediately without making any changes to the collection.
     * @note The new conversation is always inserted at the beginning of the conversations collection.
     *
     * @see DeleteConversation
     * @see Conversation::GetConversationLabel
     */
    void AddConversation(const Conversation& conversation)
    {
        auto label = conversation.GetConversationLabel();
        if (!label.has_value()) {
            return;
        }
        DeleteConversation(label.value());
        conversations.insert(conversations.begin(), conversation);
    }

    size_t size() const { return conversations.size(); }
    bool empty() const { return size() == 0; }
};

class WXDLLIMPEXP_SDK Config
{
public:
    Config();
    virtual ~Config();

    /**
     * @brief Retrieves the chat history for a given endpoint.
     *
     * This method performs a thread-safe lookup of the chat history associated with
     * the specified endpoint. If no history exists for the endpoint, an empty
     * ChatHistory object is returned.
     *
     * @param endpoint The endpoint identifier whose chat history is to be retrieved.
     *
     * @return A copy of the ChatHistory object associated with the endpoint, or an
     *         empty ChatHistory if the endpoint has no recorded history.
     *
     * @note This method is thread-safe and acquires a lock on the internal mutex
     *       during execution.
     */
    ChatHistory GetHistory(const wxString& endpoint) const
    {
        std::scoped_lock lk{m_mutex};
        if (!m_history.contains(endpoint)) {
            return {};
        }

        return m_history.find(endpoint)->second;
    }

    /**
     * @brief Adds a conversation to the history of a specific endpoint.
     *
     * This method is thread-safe and acquires a lock before modifying the endpoint's
     * conversation history. If no history exists for the given endpoint, a new one is
     * created automatically.
     *
     * @param endpoint The endpoint identifier (as a wxString) to which the conversation
     *                 should be added.
     * @param conversation The Conversation object to be added to the endpoint's history.
     *
     * @return void This function does not return a value.
     *
     * @note This method is thread-safe due to the internal use of m_mutex.
     */
    void AddConversation(const wxString& endpoint, const Conversation& conversation);

    /**
     * @brief Sets or replaces the chat history for a specific endpoint.
     *
     * This function removes any existing chat history associated with the given endpoint
     * and inserts the new history. The operation is performed in a thread-safe manner
     * using an internal mutex lock.
     *
     * @param endpoint The unique identifier string for the endpoint whose history is being set.
     * @param history The ChatHistory object containing the new chat history to associate with the endpoint.
     *
     * @return void This function does not return a value.
     *
     * @note This function is thread-safe and can be called from multiple threads concurrently.
     */
    void SetHistory(const wxString& endpoint, const ChatHistory& history)
    {
        std::scoped_lock lk{m_mutex};
        if (endpoint.empty()) {
            return;
        }
        m_history.erase(endpoint);
        m_history.insert({endpoint, history});
    }

    /**
     * @brief Clears the chat history.
     */
    void ClearHistory()
    {
        std::scoped_lock lk{m_mutex};
        m_history.clear();
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

    /**
     * @brief Sets the caching policy for this object.
     *
     * This method is thread-safe and uses a scoped lock to ensure atomic updates
     * to the internal caching policy string.
     */
    inline void SetCachePolicy(llm::CachePolicy cp)
    {
        std::scoped_lock lock{m_mutex};
        m_cachingPolicy = CachePolicyToString(cp);
    }

    /**
     * @brief Gets the current cache policy.
     */
    inline llm::CachePolicy GetCachePolicy() const
    {
        std::scoped_lock lock{m_mutex};
        return CachePolicyFromString(m_cachingPolicy);
    }

    /**
     * @brief Gets the current cache policy as string.
     */
    inline wxString GetCachePolicyString() const { return CachePolicyToString(GetCachePolicy()); }

    /**
     * @brief Set the current cache policy as string.
     */
    inline void SetCachePolicy(const wxString& cp) { SetCachePolicy(CachePolicyFromString(cp)); }

    inline bool AreToolsEnabled() const { return m_enableTools.load(); }
    inline void SetToolsEnabled(bool b) { m_enableTools.store(b); }

private:
    wxString GetFullPath();
    ChatHistory& GetOrAddHistory(const wxString& endpoint) CALLER_MUST_LOCK(m_mutex);

    mutable std::mutex m_mutex;
    std::map<std::string, std::string> m_prompts GUARDED_BY(m_mutex);
    mutable std::map<wxString, ChatHistory> m_history GUARDED_BY(m_mutex);
    std::atomic_bool m_enableTools;
    wxString m_cachingPolicy GUARDED_BY(m_mutex){llm::kCacheAuto};
};
} // namespace llm
