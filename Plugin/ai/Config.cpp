#include "Config.hpp"

#include "FileManager.hpp"
#include "Prompts.cpp"
#include "file_logger.h"
#include "fileutils.h"

namespace llm
{
constexpr size_t kMaxHistorySize = 50;

template <typename T>
T ReadValue(const nlohmann::json& j, const std::string& name, const T& d = {})
{
    try {
        return j[name].get<T>();
    } catch ([[maybe_unused]] const std::exception& e) {
        return d;
    }
}

const std::map<std::string, std::string> kDefaultPromptTable = {
    {PromptKindToString(PromptKind::kCommentGeneration), PROMPT_DOCSTRING_GEN},
    {PromptKindToString(PromptKind::kReleaseNotesGenerate), PROMPT_GIT_RELEASE_NOTES},
    {PromptKindToString(PromptKind::kReleaseNotesMerge), PROMPT_GIT_RELEASE_NOTES_MERGE},
    {PromptKindToString(PromptKind::kGitChangesCodeReview), PROMPT_GIT_CODE_REVIEW},
    {PromptKindToString(PromptKind::kGitCommitMessage), PROMPT_GIT_COMMIT_MSG}};

Config::Config() {}

Config::~Config() {}

void Config::Load()
{
    std::scoped_lock lk{m_mutex};

    // defaults
    m_prompts = kDefaultPromptTable;
    m_history = {};

    wxString content;
    if (!FileUtils::ReadFileContent(GetFullPath(), content, wxConvUTF8)) {
        return;
    }
    std::string cstr_content = content.ToStdString(wxConvUTF8);

    try {
        auto json = nlohmann::json::parse(cstr_content);
        m_prompts = ReadValue(json, "prompts", kDefaultPromptTable);
        m_cachingPolicy = ReadValue<std::string>(json, "caching_policy", llm::kCacheAuto.ToStdString(wxConvUTF8));
        m_enableTools = ReadValue<bool>(json, "enable_tools", true);

        if (json.contains("history") && json["history"].is_object()) {
            for (const auto& kv : json["history"].items()) {
                wxString endpoint_name = wxString::FromUTF8(kv.key());
                auto history = ChatHistory::from_json(kv.value());
                if (!history.has_value()) {
                    continue;
                }
                m_history.insert({endpoint_name, history.value()});
            }
        }

    } catch (const std::exception& e) {
        clERROR() << "Failed to parse JSON file:" << GetFullPath() << "." << e.what() << endl;
        return;
    }
}

void Config::Save()
{
    std::scoped_lock lk{m_mutex};
    nlohmann::json j;

    // Merge new entries from the "kDefaultPromptTable" into the stored ones.
    for (const auto& [k, v] : kDefaultPromptTable) {
        if (m_prompts.count(k)) {
            continue;
        }
        m_prompts.insert({k, v});
    }

    j["prompts"] = m_prompts;
    j["history"] = json::object();

    auto& history = j["history"];
    for (const auto& [endpoint, chat_history] : m_history) {
        history[endpoint] = chat_history.to_json();
    }

    j["enable_tools"] = m_enableTools.load();
    j["caching_policy"] = m_cachingPolicy.ToStdString(wxConvUTF8);

    wxString content = wxString::FromUTF8(j.dump(2));
    FileUtils::WriteFileContent(GetFullPath(), content, wxConvUTF8);
}

void Config::AddConversation(const wxString& endpoint, const Conversation& conversation)
{
    std::scoped_lock lk{m_mutex};
    auto& history = GetOrAddHistory(endpoint);
    history.AddConversation(conversation);
}

wxString Config::GetPrompt(PromptKind kind) const
{
    if (kind == PromptKind::kMax) {
        return wxEmptyString;
    }

    std::scoped_lock lk{m_mutex};
    auto key = PromptKindToString(kind);
    auto prompt_iter = m_prompts.find(key);
    if (prompt_iter != m_prompts.end() && !prompt_iter->second.empty()) {
        return wxString::FromUTF8(prompt_iter->second);
    }

    // Return the default prompt
    prompt_iter = kDefaultPromptTable.find(key);
    if (prompt_iter != m_prompts.end()) {
        return prompt_iter->second;
    }
    clWARNING() << "No prompt found for key: '" << key << "'" << endl;
    return wxEmptyString;
}

void Config::SetPrompt(PromptKind kind, const wxString& prompt)
{
    auto label = PromptKindToString(kind);
    std::scoped_lock lk{m_mutex};
    switch (kind) {
    case PromptKind::kMax:
        break;
    default:
        m_prompts.erase(label);
        m_prompts.insert({label, prompt.ToStdString(wxConvUTF8)});
        break;
    }
}

void Config::ResetPrompts()
{
    std::scoped_lock lk{m_mutex};
    m_prompts.clear();
    m_prompts = kDefaultPromptTable;
}

ChatHistory& Config::GetOrAddHistory(const wxString& endpoint) { return m_history[endpoint]; }

wxString Config::GetFullPath()
{
    return FileManager::GetSettingFileFullPath(
        "assistant-global-settings.json", WriteOptions{.ignore_workspace = true});
}

} // namespace llm
