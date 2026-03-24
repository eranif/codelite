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
    case llm::PromptKind::kReleaseNotesGenerate:
        return "Generate Release Notes";
    case llm::PromptKind::kCommentGeneration:
        return "Generate Comment";
    case llm::PromptKind::kGitCommitMessage:
        return "Generate Commit Message";
    case llm::PromptKind::kGitChangesCodeReview:
        return "Code Review";
    case llm::PromptKind::kMax:
        return "";
    }
    return "";
}

using nlohmann::json;
class WXDLLIMPEXP_SDK Config
{
public:
    Config();
    virtual ~Config();
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
     * Retrieves all configured prompts as a vector of label-prompt pairs.
     *
     * This method provides thread-safe access to all prompts stored in the configuration.
     * Each prompt is converted from UTF-8 encoding to wxString format. The method acquires
     * a mutex lock to ensure consistent read access and prevents concurrent modifications.
     *
     * Parameters:
     *   (none)
     *
     * Returns:
     *   A vector of pairs where each pair contains a prompt label (first) and its
     *   corresponding prompt text (second), both as wxString objects. Returns an empty
     *   vector if no prompts are configured.
     */
    std::vector<std::pair<wxString, wxString>> GetAllPrompts() const;

    /**
     * Adds or replaces a prompt entry in the configuration.
     *
     * This method thread-safely adds a new prompt to the internal prompts map,
     * using the provided label as the key. If an entry with the same label already
     * exists, it is first removed and then replaced with the new prompt value.
     * Both the label and prompt are converted from wxString to UTF-8 encoded
     * standard strings before storage.
     *
     * Parameters:
     *   label - A wxString containing the unique identifier for the prompt entry.
     *   prompt - A wxString containing the prompt text or content to be stored.
     *
     * Returns:
     *   Nothing (void).
     */
    void AddPrompt(const wxString& label, const wxString& prompt);
    /**
     * Deletes a prompt configuration entry by its label.
     *
     * This function removes the prompt associated with the given label from the
     * internal prompts collection. Thread-safe operation is ensured via an internal
     * mutex lock that is held for the duration of the erase operation.
     *
     * Parameters:
     *   label - A wxString containing the label of the prompt to delete. The label
     *           is converted to a UTF-8 encoded std::string for lookup and removal.
     *
     * Returns:
     *   void - No value is returned.
     */
    void DeletePrompt(const wxString& label);

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

    bool IsBuiltInPrompt(const wxString& prompt) const;
    bool IsPromptExists(const wxString& prompt) const
    {
        std::lock_guard lock{m_mutex};
        return m_prompts.contains(prompt.ToStdString(wxConvUTF8));
    }

private:
    wxString GetFullPath();

    mutable std::mutex m_mutex;
    std::map<std::string, std::string> m_prompts GUARDED_BY(m_mutex);
    std::atomic_bool m_enableTools{true};
    wxString m_cachingPolicy GUARDED_BY(m_mutex){llm::kCacheAuto};
};
} // namespace llm
