#include "Config.hpp"

#include "FileManager.hpp"
#include "Prompts.cpp"
#include "file_logger.h"
#include "fileutils.h"

#include <algorithm>

namespace llm
{
constexpr size_t kMaxHistorySize = 50;

template <typename T>
T ReadValue(const nlohmann::json& j, const std::string& name, const T& d = {})
{
    if (!j.contains(name)) {
        return d;
    }
    try {
        return j[name].get<T>();
    } catch ([[maybe_unused]] const std::exception& e) {
        return d;
    }
}

const std::map<std::string, std::string> kDefaultPromptTable = {
    {PromptKindToString(PromptKind::kCommentGeneration), PROMPT_DOCSTRING_GEN},
    {PromptKindToString(PromptKind::kReleaseNotesGenerate), PROMPT_GIT_RELEASE_NOTES},
    {PromptKindToString(PromptKind::kGitChangesCodeReview), PROMPT_GIT_CODE_REVIEW},
    {PromptKindToString(PromptKind::kGitCommitMessage), PROMPT_GIT_COMMIT_MSG}};

Config::Config() {}

Config::~Config() {}

void Config::Load()
{
    std::scoped_lock lk{m_mutex};

    // defaults
    m_prompts = kDefaultPromptTable;
    wxString content;
    if (!FileUtils::ReadFileContent(GetFullPath(), content, wxConvUTF8)) {
        return;
    }
    std::string cstr_content = content.ToStdString(wxConvUTF8);

    try {
        auto json = nlohmann::json::parse(cstr_content);
        m_prompts = ReadValue(json, "prompts", kDefaultPromptTable);
        m_cachingPolicy = ReadValue<std::string>(json, "caching_policy", llm::kCacheAuto.ToStdString(wxConvUTF8));
        m_persistingTrustedTools = ReadValue(json, "trusted_tools", decltype(m_persistingTrustedTools){});
        m_enableTools = ReadValue<bool>(json, "enable_tools", true);

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
    j["enable_tools"] = m_enableTools.load();
    j["caching_policy"] = m_cachingPolicy.ToStdString(wxConvUTF8);
    j["trusted_tools"] = m_persistingTrustedTools;

    wxString content = wxString::FromUTF8(j.dump(2));
    FileUtils::WriteFileContent(GetFullPath(), content, wxConvUTF8);
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

void Config::DeletePrompt(const wxString& label)
{
    std::scoped_lock lk{m_mutex};
    m_prompts.erase(label.ToStdString(wxConvUTF8));
}

void Config::AddPrompt(const wxString& label, const wxString& prompt)
{
    std::scoped_lock lk{m_mutex};
    std::string utf8 = label.ToStdString(wxConvUTF8);
    m_prompts.erase(utf8);
    m_prompts.insert({utf8, prompt.ToStdString(wxConvUTF8)});
}

std::vector<std::pair<wxString, wxString>> Config::GetAllPrompts() const
{
    std::scoped_lock lk{m_mutex};
    std::vector<std::pair<wxString, wxString>> result;
    result.reserve(m_prompts.size());
    for (const auto& [label, prompt] : m_prompts) {
        result.push_back({wxString::FromUTF8(label), wxString::FromUTF8(prompt)});
    }
    return result;
}

void Config::SetPrompt(PromptKind kind, const wxString& prompt)
{
    switch (kind) {
    case PromptKind::kMax:
        break;
    default: {
        auto label = PromptKindToString(kind);
        AddPrompt(wxString::FromUTF8(label), prompt);
    } break;
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

bool Config::IsBuiltInPrompt(const wxString& prompt) const
{
    std::string label = prompt.ToStdString(wxConvUTF8);
    return kDefaultPromptTable.contains(label);
}

void Config::AddTrustedTool(const wxString& toolname, const wxString& pattern, bool persist)
{
    CHECK_COND_RET(!toolname.empty());

    std::scoped_lock lk{m_mutex};
    std::string tool = toolname.ToStdString(wxConvUTF8);
    auto& patterns = persist ? m_persistingTrustedTools[tool] : m_transientTrustedTools[tool];
    if (pattern.empty()) {
        // Just add an entry with empty string
        patterns.push_back(std::string{});

    } else {
        std::string pat = pattern.ToStdString(wxConvUTF8);

        // Pick the data structure to use.
        auto it = std::ranges::find_if(patterns, [&pat](const std::string& p) { return p == pat; });
        if (it == patterns.end()) {
            patterns.push_back(pat);
        }
        *it = pat;
    }
}

void Config::DeleteTrustedTool(const wxString& toolname)
{
    std::scoped_lock lk{m_mutex};
    m_persistingTrustedTools.erase(toolname.ToStdString(wxConvUTF8));
    m_transientTrustedTools.erase(toolname.ToStdString(wxConvUTF8));
}

bool Config::IsToolTrustedFor(const wxString& toolname, std::function<bool(const wxString&)> check_pattern_cb) const
{
    std::scoped_lock lk{m_mutex};
    auto check_list =
        [&toolname](
            const std::map<std::string, std::vector<std::string>>& trusted_tools) -> const std::vector<std::string>* {
        auto it = trusted_tools.find(toolname.ToStdString(wxConvUTF8));
        if (it == trusted_tools.end())
            return nullptr;
        return &it->second;
    };

    auto pattens = check_list(m_persistingTrustedTools);
    if (pattens == nullptr) {
        pattens = check_list(m_transientTrustedTools);
    }

    if (pattens == nullptr) {
        return false;
    }
    if (check_pattern_cb == nullptr) {
        return true;
    }
    for (const auto& pat : *pattens) {
        if (check_pattern_cb(pat.empty() ? wxString{} : wxString::FromUTF8(pat)))
            return true;
    }
    return false;
}

} // namespace llm
