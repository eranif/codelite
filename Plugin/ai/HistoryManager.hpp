#pragma once

// clang-format off
#include <wx/utils.h>
// clang-format on

#include "assistant/assistant.hpp"
#include "codelite_exports.h"
#include "wx/filename.h"

#include <map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/wxcrt.h>

namespace llm
{
struct WXDLLIMPEXP_SDK Conversation {
    assistant::messages messages_;
    wxString content_;
    wxString label_;

    Conversation() = default;
    ~Conversation() = default;
    /**
     * @brief Constructs a conversation from messages, rendered content, and a label.
     *
     * @param msgs The assistant message sequence to store.
     * @param content The conversation content as plain text.
     * @param label The human-readable label for this conversation.
     */
    explicit Conversation(const assistant::messages& msgs, const wxString& content, const wxString& label)
        : messages_(msgs)
        , content_(content)
        , label_(label)
    {
    }

    assistant::json to_json() const
    {
        auto j = assistant::json::object();
        j["messages"] = messages_.to_json();
        j["content"] = content_.ToStdString(wxConvUTF8);
        j["label"] = label_.ToStdString(wxConvUTF8);
        return j;
    }

    /**
     * @brief Creates a conversation from a JSON value.
     *
     * @param j JSON object containing messages, content, and label fields.
     * @return std::optional<Conversation> Parsed conversation, or std::nullopt on failure.
     */
    static std::optional<Conversation> from_json(assistant::json j);

    /**
     * @brief Creates a conversation from a JSON string.
     *
     * @param s UTF-8 encoded JSON string representation of a conversation.
     * @return std::optional<Conversation> Parsed conversation, or std::nullopt on failure.
     */
    static std::optional<Conversation> from_string(std::string s);

    /**
     * @brief Loads a conversation from a JSON file.
     *
     * @param path UTF-8 encoded file system path to a conversation file.
     * @return std::optional<Conversation> Parsed conversation, or std::nullopt on failure.
     */
    static std::optional<Conversation> from_file(std::string path);

    /**
     * @brief Returns the conversation label.
     *
     * @return const wxString& The stored label.
     */
    inline const wxString& GetLabel() const { return label_; }
};

/**
 * @brief File-system entry for a stored conversation.
 */
struct WXDLLIMPEXP_SDK HistoryEntry {
    /**
     * @brief Full path to the conversation JSON file.
     */
    wxString filepath;
    /**
     * @brief Display label for the stored conversation.
     */
    wxString label;
};

/**
 * @brief Helper utilities for working with collections of history entries.
 */
struct WXDLLIMPEXP_SDK HistoryEntryHelper {
    /**
     * @brief Extracts labels from a sequence of history entries.
     *
     * @param entries History entries to process.
     * @return wxArrayString Labels in the same order as the input entries.
     */
    static inline wxArrayString GetLabels(const std::vector<HistoryEntry>& entries)
    {
        wxArrayString labels;
        labels.reserve(entries.size());
        for (const auto& e : entries) {
            labels.Add(e.label);
        }
        return labels;
    }

    /**
     * @brief Indexes history entries by label.
     *
     * @param entries History entries to process.
     * @return std::map<wxString, HistoryEntry> Map keyed by entry label.
     */
    static inline std::map<wxString, HistoryEntry> GetAsMap(const std::vector<HistoryEntry>& entries)
    {
        std::map<wxString, HistoryEntry> m;
        for (const auto& e : entries) {
            m.emplace(e.label, e);
        }
        return m;
    }
};

class WXDLLIMPEXP_SDK HistoryStore
{
public:
    /**
     * @brief Stores a conversation as a JSON file for the given endpoint.
     *
     * Serializes the provided conversation, writes it to the endpoint-specific
     * storage directory using a normalized label-based file name, and then evicts
     * old entries to keep the number of files within the configured limit.
     *
     * @param endpoint The endpoint whose conversation store directory will receive the file.
     * @param conversation The conversation to persist to disk.
     *
     * @return bool True if the conversation was written successfully; false otherwise.
     */
    bool Put(const wxString& endpoint, const Conversation& conversation) const;

    /**
     * @brief Retrieves a stored conversation for the given endpoint and label.
     *
     * Looks up the conversation file in the history directory, reads its contents,
     * and parses the file data into a Conversation object. If the file does not
     * exist, cannot be read, or cannot be parsed, the function returns an empty
     * optional.
     *
     * @param endpoint The endpoint whose history directory should be searched.
     * @param entry The history entry used to locate the stored file.
     *
     * @return std::optional<Conversation> The parsed Conversation if found and successfully loaded; otherwise
     * std::nullopt.
     */
    std::optional<Conversation> Get(const wxString& endpoint, const HistoryEntry& entry) const;

    /**
     * @brief Deletes a stored conversation file for the given endpoint and label.
     *
     * This member of HistoryStore builds the conversation file path from the
     * endpoint directory and normalized label, then removes the corresponding JSON
     * file from disk if it exists.
     *
     * @param endpoint The endpoint whose conversation directory is searched.
     * @param entry The history entry identifying the file to delete.
     *
     * @return bool True if the file existed and was successfully removed; false otherwise.
     */
    bool Delete(const wxString& endpoint, const HistoryEntry& entry) const;

    /**
     * @brief Deletes multiple history entries for a given endpoint.
     *
     * This HistoryStore method iterates over the provided entries and attempts to delete
     * each one for the specified endpoint, counting only the successful deletions.
     *
     * @param endpoint The endpoint whose history entries should be removed.
     * @param entries The history entries to delete.
     *
     * @return size_t The number of entries that were successfully deleted.
     */
    size_t DeleteMulti(const wxString& endpoint, const std::vector<HistoryEntry>& entries) const;

    /**
     * @brief Deletes all stored conversation files for a given endpoint.
     *
     * Searches the endpoint-specific storage directory for JSON files and removes
     * each one from disk. If the directory does not exist, nothing is deleted and
     * zero is returned.
     *
     * @param endpoint The endpoint whose conversation directory is searched.
     *
     * @return size_t The number of JSON files successfully deleted.
     */
    size_t DeleteAll(const wxString& endpoint) const;

    /**
     * @brief Lists the labels of all stored conversations for a given endpoint.
     *
     * Searches the endpoint-specific storage directory for JSON files, loads each
     * conversation from disk, and collects the label from every successfully parsed
     * conversation. If the endpoint directory does not exist, an empty array is
     * returned.
     *
     * @param endpoint The endpoint identifier whose stored conversation labels should be listed.
     *
     * @return std::vector<HistoryEntry> All conversations that could be loaded successfully.
     */
    std::vector<HistoryEntry> List(const wxString& endpoint) const;

    /**
     * @brief Counts the stored conversations for a given endpoint.
     *
     * Counts the number of JSON conversation files present in the endpoint-specific
     * storage directory. If the directory does not exist, zero is returned.
     *
     * @param endpoint The endpoint whose stored conversations should be counted.
     *
     * @return size_t The number of stored conversations found on disk.
     */
    size_t Count(const wxString& endpoint) const;

private:
    /**
     * @brief Returns the filesystem directory used to store data for a specific endpoint.
     *
     * Builds the path under the application's user data directory as "db/assistant/<normalized-endpoint>",
     * creates the directory tree if it does not already exist, and returns the resulting path string.
     *
     * @param endpoint const wxString& The endpoint name used to derive the final directory name.
     *
     * @return wxString The full path to the endpoint-specific storage directory.
     */
    wxString GetEndpointDir(const wxString& endpoint) const;

    /**
     * @brief Constructs the file path for a history store entry.
     *
     * Builds a wxFileName for the current HistoryStore by combining the directory
     * for the given endpoint with a normalized version of the supplied label, then
     * forces the file extension to "json".
     *
     * @param endpoint const wxString& The endpoint name used to determine the
     *        target directory.
     * @param label const wxString& The entry label used to derive the file name.
     *
     * @return wxFileName The completed file path for the history entry.
     */
    wxFileName MakeFilePath(const wxString& endpoint, const wxString& label) const;

    /**
     * Evicts the oldest history JSON files for a given endpoint until the per-endpoint
     * file limit is satisfied.
     *
     * This method scans the endpoint's history directory for "*.assistant::json" files, sorts them
     * by modification time from oldest to newest (breaking ties by full path), and deletes
     * the excess files beyond kMaxFilesPerEndpoint. If a file cannot be removed, a warning
     * is logged and processing continues.
     *
     * @param endpoint The endpoint whose history directory should be pruned.
     * @return void
     * @throws None. Failures to delete individual files are reported through logging only.
     */
    void Evict(const wxString& endpoint) const;
};
} // namespace llm
