#include "Config.hpp"

#include "FileManager.hpp"
#include "Prompts.cpp"
#include "cl_config.h"
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
        m_history = ReadValue(json, "history", std::vector<std::string>{});
    } catch (const std::exception& e) {
        clERROR() << "Failed to parse JSON file:" << GetFullPath() << "." << e.what() << endl;
        return;
    }
}

void Config::Save()
{
    std::scoped_lock lk{m_mutex};
    nlohmann::json j;
    if (m_history.size() > kMaxHistorySize) {
        m_history.resize(kMaxHistorySize);
    }

    // Merge new entries from the "kDefaultPromptTable" into the stored ones.
    for (const auto& [k, v] : kDefaultPromptTable) {
        if (m_prompts.count(k)) {
            continue;
        }
        m_prompts.insert({k, v});
    }

    j["prompts"] = m_prompts;
    j["history"] = m_history;

    wxString content = wxString::FromUTF8(j.dump(2));
    FileUtils::WriteFileContent(GetFullPath(), content, wxConvUTF8);
}

void Config::AddHistory(const wxString& prompt)
{
    std::scoped_lock lk{m_mutex};
    std::string new_prompt = prompt.ToStdString(wxConvUTF8);
    auto iter = std::find_if(
        m_history.begin(), m_history.end(), [&new_prompt](const std::string& p) { return p == new_prompt; });
    if (iter != m_history.end()) {
        m_history.erase(iter);
    }

    // Insert at the top
    m_history.insert(m_history.begin(), new_prompt);
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

wxString Config::GetFullPath()
{
    return FileManager::GetSettingFileFullPath(
        "assistant-global-settings.json", WriteOptions{.ignore_workspace = true});
}

} // namespace llm
