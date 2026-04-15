#include "ai/HistoryManager.hpp"

#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fileutils.h"

#include <algorithm>
#include <vector>
#include <wx/dir.h>
#include <wx/filename.h>

namespace
{
/**
 * @brief Normalizes a label string into an identifier-friendly form.
 *
 * Replaces every non-alphanumeric character with an underscore while preserving
 * alphanumeric characters.
 *
 * @param label The input label to normalize.
 * @return wxString The normalized label.
 */
inline wxString NormaliseLabel(const wxString& label)
{
    wxString normalised;
    normalised.reserve(label.size());

    for (const auto ch : label) {
        if (wxIsalnum(ch)) {
            normalised += ch;
        } else {
            normalised += '_';
        }
    }
    return normalised;
}
} // namespace

namespace llm
{
constexpr size_t kMaxFilesPerEndpoint = 50;

std::optional<Conversation> Conversation::from_json(assistant::json j)
{
    try {
        Conversation c;
        for (auto& message_json : j["messages"]) {
            assistant::message msg;
            assistant::json& jmsg = dynamic_cast<assistant::json&>(msg);
            jmsg = message_json;
            c.messages_.push_back(std::move(msg));
        }
        c.content_ = wxString::FromUTF8(j["content"].get<std::string>());
        c.label_ = wxString::FromUTF8(j["label"].get<std::string>());
        return c;
    } catch (const std::exception& e) {
        clWARNING() << "An error occurred while constructing Conversation from JSON." << e.what() << endl;
        return std::nullopt;
    }
}

std::optional<Conversation> Conversation::from_string(std::string s)
{
    try {
        assistant::json j = assistant::json::parse(s);
        return from_json(j);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<Conversation> Conversation::from_file(std::string path)
{
    try {
        wxFileName fn(wxString::FromUTF8(path));
        wxString content;
        if (!FileUtils::ReadFileContent(fn, content) || content.empty()) {
            return std::nullopt;
        }
        return from_string(content.ToStdString(wxConvUTF8));
    } catch (...) {
        return std::nullopt;
    }
}

wxFileName HistoryStore::MakeFilePath(const wxString& endpoint, const wxString& label) const
{
    wxFileName fn{GetEndpointDir(endpoint), NormaliseLabel(label)};
    fn.SetExt("json");
    return fn;
}

bool HistoryStore::Put(const wxString& endpoint, const Conversation& conversation) const
{
    auto conversation_file = MakeFilePath(endpoint, conversation.GetLabel());
    wxString content = wxString::FromUTF8(conversation.to_json().dump(2));
    if (!FileUtils::WriteFileContent(conversation_file, content)) {
        clWARNING() << "Failed to write conversation to disk." << conversation_file.GetFullPath() << endl;
        return false;
    }

    // Make sure that the number of files in the endpoint directory does not exceed the limit.
    Evict(endpoint);
    return true;
}

std::optional<Conversation> HistoryStore::Get(const wxString& endpoint, const HistoryEntry& entry) const
{
    if (!wxFileName::FileExists(entry.filepath)) {
        clWARNING() << "Failed to read conversation from disk:" << entry.filepath << ". No such file" << endl;
        return std::nullopt;
    }

    wxString content;
    if (!FileUtils::ReadFileContent(entry.filepath, content)) {
        clWARNING() << "Failed to read conversation content from disk:" << entry.filepath << endl;
        return std::nullopt;
    }

    std::string str = content.ToStdString(wxConvUTF8);
    auto c = Conversation::from_string(std::move(str));
    if (c.has_value()) {
        clDEBUG() << "Successfully restored conversation from disk" << endl;
    } else {
        clWARNING() << "Failed to construct conversation content from content:" << str << endl;
    }
    return c;
}

size_t HistoryStore::DeleteMulti(const wxString& endpoint, const std::vector<HistoryEntry>& entries) const
{
    size_t deleted{0};
    for (const HistoryEntry& entry : entries) {
        if (Delete(endpoint, entry)) {
            ++deleted;
        }
    }
    return deleted;
}

bool HistoryStore::Delete(const wxString& endpoint, const HistoryEntry& entry) const
{
    if (!wxFileName::FileExists(entry.filepath)) {
        return false;
    }

    if (!wxRemoveFile(entry.filepath)) {
        clWARNING() << "Failed to delete conversation from disk." << entry.filepath << endl;
        return false;
    }
    return true;
}

size_t HistoryStore::DeleteAll(const wxString& endpoint) const
{
    wxString endpoint_dir = GetEndpointDir(endpoint);
    if (!wxDirExists(endpoint_dir)) {
        return 0;
    }

    wxArrayString json_files;
    wxDir::GetAllFiles(endpoint_dir, &json_files, "*.json", wxDIR_FILES);

    size_t deleted = 0;
    for (const wxString& file : json_files) {
        if (wxRemoveFile(file)) {
            ++deleted;
        } else {
            clWARNING() << "Failed to delete conversation from disk." << file << endl;
        }
    }

    return deleted;
}

std::vector<HistoryEntry> HistoryStore::List(const wxString& endpoint) const
{
    std::vector<HistoryEntry> entries;

    wxString endpoint_dir = GetEndpointDir(endpoint);
    if (!wxDirExists(endpoint_dir)) {
        return entries;
    }

    wxArrayString json_files;
    wxDir::GetAllFiles(endpoint_dir, &json_files, "*.json", wxDIR_FILES);

    for (const wxString& file : json_files) {
        auto conv = Conversation::from_file(file.ToStdString(wxConvUTF8));
        if (conv.has_value()) {
            entries.push_back(HistoryEntry{
                .filepath = file,
                .label = conv.value().GetLabel(),
            });
        }
    }
    return entries;
}

size_t HistoryStore::Count(const wxString& endpoint) const
{
    wxString endpoint_dir = GetEndpointDir(endpoint);
    if (!wxDirExists(endpoint_dir)) {
        return 0;
    }

    wxArrayString json_files;
    wxDir::GetAllFiles(endpoint_dir, &json_files, "*.json", wxDIR_FILES);
    return json_files.size();
}

wxString HistoryStore::GetEndpointDir(const wxString& endpoint) const
{
    wxFileName store_path(clStandardPaths::Get().GetUserDataDir(), wxEmptyString);
    store_path.AppendDir("db");
    store_path.AppendDir("assistant");
    store_path.AppendDir(NormaliseLabel(endpoint));

    wxString path = store_path.GetPath();
    if (!store_path.DirExists()) {
        store_path.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    return path;
}

void HistoryStore::Evict(const wxString& endpoint) const
{
    wxString endpoint_dir = GetEndpointDir(endpoint);

    wxArrayString json_files;
    wxDir::GetAllFiles(endpoint_dir, &json_files, "*.json", wxDIR_FILES);

    if (json_files.size() <= kMaxFilesPerEndpoint) {
        return;
    }

    std::vector<wxFileName> files;
    files.reserve(json_files.size());
    for (const wxString& file : json_files) {
        files.emplace_back(file);
    }

    std::sort(files.begin(), files.end(), [](const wxFileName& lhs, const wxFileName& rhs) {
        const wxDateTime lhsTime = lhs.GetModificationTime();
        const wxDateTime rhsTime = rhs.GetModificationTime();
        if (lhsTime.IsEarlierThan(rhsTime)) {
            return true;
        }
        if (rhsTime.IsEarlierThan(lhsTime)) {
            return false;
        }
        return lhs.GetFullPath() < rhs.GetFullPath();
    });

    const size_t files_to_delete = files.size() - kMaxFilesPerEndpoint;
    for (size_t i = 0; i < files_to_delete; ++i) {
        if (!FileUtils::RemoveFile(files[i].GetFullPath())) {
            clWARNING() << "Failed to evict history file:" << files[i].GetFullPath() << endl;
        }
    }
}
} // namespace llm
