#include "Config.hpp"

#include "FileManager.hpp"
#include "Prompts.cpp"
#include "cl_standard_paths.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"

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

static const std::unordered_set<std::string> builtin_prompts = {
    {kPromptGenerateComment},
    {kPromptGenerateReleaseNotes},
    {kPromptGenerateCommitMessage},
    {kPromptGenerateCodeReview},
};

static const std::map<std::string, std::string> kDefaultPromptTable = {
    {kPromptGenerateComment, PROMPT_DOCSTRING_GEN},
    {kPromptGenerateReleaseNotes, PROMPT_GIT_RELEASE_NOTES},
    {kPromptGenerateCommitMessage, PROMPT_GIT_COMMIT_MSG},
    {kPromptGenerateCodeReview, PROMPT_GIT_CODE_REVIEW},
};

Config::Config() {}

Config::~Config() {}

clStatus Config::Load()
{
    std::scoped_lock lk{m_mutex};

    wxString content;
    if (!FileUtils::ReadFileContent(GetFullPath(), content, wxConvUTF8)) {
        return StatusIOError("Read error");
    }
    std::string cstr_content = content.ToStdString(wxConvUTF8);

    // defaults
    m_prompts = kDefaultPromptTable;
    wxString doc_string_prompt_label = GetPromptFilePath(kPromptGenerateComment);
    if (!wxFileName::FileExists(doc_string_prompt_label)) {
        clSYSTEM() << "Initializing prompt store with default values" << endl;
        // First time, write default prompts
        for (const auto& [prompt_label, prompt_content] : m_prompts) {
            wxString label = wxString::FromUTF8(prompt_label);
            wxString content = wxString::FromUTF8(prompt_content);
            auto result = WritePromptToFile(label, content);
            if (!result.ok()) {
                clWARNING() << "Failed to write default prompt file:" << GetPromptFilePath(label) << "."
                            << result.message() << endl;
            }
            clSYSTEM() << "Default prompt:" << label << "written to file:" << GetPromptFilePath(label) << endl;
        }
    }

    try {
        auto json = nlohmann::json::parse(cstr_content);
        m_prompts.clear();
        if (json.contains("prompts") && json["prompts"].is_object()) {
            for (const auto& kv : json["prompts"].items()) {
                std::string prompt_label = kv.key();
                const auto st = ReadPromptFromFile(wxString::FromUTF8(prompt_label));
                if (!st.ok()) {
                    clWARNING() << "Failed to read prompt file:" << GetPromptFilePath(prompt_label) << "."
                                << st.error_message() << endl;
                    continue;
                }
                m_prompts[prompt_label] = st.value().ToStdString(wxConvUTF8);
            }
        }

        m_cachingPolicy = ReadValue<std::string>(json, "caching_policy", llm::kCacheAuto.ToStdString(wxConvUTF8));
        m_persistingTrustedTools = ReadValue(json, "trusted_tools", decltype(m_persistingTrustedTools){});
        m_enableTools = ReadValue<bool>(json, "enable_tools", true);

    } catch (const std::exception& e) {
        wxString errmsg;
        errmsg << "Invalid JSON file:" << GetFullPath() << "." << e.what();
        return StatusOther(errmsg);
    }
    return StatusOk();
}

void Config::Save(bool save_prompts)
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

    if (save_prompts) {
        // Write the prompts.
        for (const auto& [prompt_label, prompt_content] : m_prompts) {
            wxString label = wxString::FromUTF8(prompt_label);
            wxString content = wxString::FromUTF8(prompt_content);
            auto result = WritePromptToFile(label, content);
            if (!result.ok()) {
                clWARNING() << "Failed to write prompt file:" << GetPromptFilePath(label) << "." << result.message()
                            << endl;
            }
        }
    }
    std::map<std::string, std::string> prompts;
    for (const auto& [prompt_label, _] : m_prompts) {
        prompts[prompt_label] = "@" + GetPromptFilePath(wxString::FromUTF8(prompt_label));
    }

    j["prompts"] = prompts;
    j["enable_tools"] = m_enableTools.load();
    j["caching_policy"] = m_cachingPolicy.ToStdString(wxConvUTF8);
    j["trusted_tools"] = m_persistingTrustedTools;

    wxString content = wxString::FromUTF8(j.dump(2));
    if (FileUtils::WriteFileContent(GetFullPath(), content, wxConvUTF8)) {
        clGetManager()->ReloadFile(GetFullPath());
    }
}

wxString Config::GetPrompt(const wxString& label) const
{
    std::scoped_lock lk{m_mutex};
    auto prompt_iter = m_prompts.find(label.ToStdString(wxConvUTF8));
    if (prompt_iter != m_prompts.end() && !prompt_iter->second.empty()) {
        return wxString::FromUTF8(prompt_iter->second);
    }
    clWARNING() << "No prompt found for key: '" << label << "'" << endl;
    return wxEmptyString;
}

void Config::DeletePrompt(const wxString& label)
{
    std::scoped_lock lk{m_mutex};
    if (IsBuiltInPrompt(label))
        return;
    m_prompts.erase(label.ToStdString(wxConvUTF8));
}

void Config::AddPrompt(const wxString& label, const wxString& prompt)
{
    std::scoped_lock lk{m_mutex};
    std::string utf8 = label.ToStdString(wxConvUTF8);
    m_prompts.insert_or_assign(utf8, prompt.ToStdString(wxConvUTF8));
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
    return builtin_prompts.contains(prompt.ToStdString(wxConvUTF8));
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
        } else {
            *it = pat;
        }
    }
}

void Config::DeleteTrustedTool(const wxString& toolname)
{
    std::scoped_lock lk{m_mutex};
    m_persistingTrustedTools.erase(toolname.ToStdString(wxConvUTF8));
    m_transientTrustedTools.erase(toolname.ToStdString(wxConvUTF8));
}

bool Config::IsToolTrustedFor(const wxString& toolname, OnTrustPattern check_pattern_cb) const
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

    std::vector<std::string> all_patterns;
    auto patterns_persist = check_list(m_persistingTrustedTools);
    auto patterns_non_persistent = check_list(m_transientTrustedTools);
    if (patterns_persist) {
        all_patterns.insert(all_patterns.end(), patterns_persist->begin(), patterns_persist->end());
    }

    if (patterns_non_persistent) {
        all_patterns.insert(all_patterns.end(), patterns_non_persistent->begin(), patterns_non_persistent->end());
    }

    if (all_patterns.empty()) {
        return false;
    }

    if (check_pattern_cb == nullptr) {
        return true;
    }

    for (const auto& pat : all_patterns) {
        if (check_pattern_cb(pat.empty() ? wxString{} : wxString::FromUTF8(pat)))
            return true;
    }
    return false;
}

wxString Config::GetPromptFilePath(const wxString& label) const
{
    wxString file_name = FileUtils::NormaliseFilename(label);
    wxFileName fn{clStandardPaths::Get().GetUserDataDir(), file_name};
    fn.AppendDir("db");
    fn.AppendDir("assistant");
    fn.AppendDir("prompts");
    fn.SetExt("md");
    fn.Mkdir(wxS_DIR_DEFAULT);
    return fn.GetFullPath();
}

clStatus Config::WritePromptToFile(const wxString& label, const wxString& content) const
{
    wxFileName file_name{GetPromptFilePath(label)};
    if (!FileUtils::WriteFileContent(file_name, content)) {
        return StatusIOError("Write error");
    }
    return StatusOk();
}

clStatusOr<wxString> Config::ReadPromptFromFile(const wxString& label) const
{
    wxFileName file_name{GetPromptFilePath(label)};
    if (!file_name.FileExists()) {
        return StatusNotFound();
    }

    wxString content;
    if (!FileUtils::ReadFileContent(file_name, content)) {
        return StatusIOError("Read error");
    }
    return content;
}

} // namespace llm
