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

        if (json.contains("chat_history") && json["chat_history"].is_array()) {
            auto chat_history = json["chat_history"];
            for (const auto& history : chat_history) {
                Conversation ch;
                for (const auto& msg : history) {
                    assistant::Message history_message;
                    history_message.text = msg["content"].get<std::string>();
                    history_message.role = msg["role"].get<std::string>();
                    ch.push_back(history_message);
                }
                if (!ch.empty()) {
                    m_history.push_back(ch);
                }
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

    nlohmann::json history_array = nlohmann::json::array();
    for (const auto& hst : m_history) {
        nlohmann::json chat = nlohmann::json::array();
        for (const auto& msg : hst) {
            nlohmann::json o = {{"role", msg.role}, {"content", msg.text}};
            chat.push_back(o);
        }
        if (!hst.empty()) {
            history_array.push_back(chat);
        }
    }
    j["chat_history"] = history_array;

    wxString content = wxString::FromUTF8(j.dump(2));
    FileUtils::WriteFileContent(GetFullPath(), content, wxConvUTF8);
}

void Config::AddConversation(const Conversation& conversation)
{
    std::scoped_lock lk{m_mutex};
    auto res = GetConversationLabel(conversation);
    if (!res.has_value()) {
        return;
    }

    auto new_label = res.value();
    auto iter = std::find_if(m_history.begin(), m_history.end(), [&new_label](const Conversation& h) {
        auto l = GetConversationLabel(h);
        return l.has_value() && l.value() == new_label;
    });

    // Delete the old chat history
    if (iter != m_history.end()) {
        m_history.erase(iter);
    }

    // Insert at the top
    m_history.insert(m_history.begin(), conversation);
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
