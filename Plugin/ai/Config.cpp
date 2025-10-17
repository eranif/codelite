#include "Config.hpp"

#include "Prompts.cpp"
#include "cl_config.h"

namespace llm
{
extern const wxString PROMPT_DOCSTRING_GEN;
extern const wxString PROMPT_GIT_COMMIT_MSG;
extern const wxString PROMPT_GIT_RELEASE_NOTES;

constexpr size_t kMaxHistorySize = 50;

const wxStringMap_t kDefaultPromptTable = {
    {GetPromptString(PromptKind::kCommentGeneration), PROMPT_DOCSTRING_GEN},
    {GetPromptString(PromptKind::kReleaseNotesGenerate), PROMPT_GIT_RELEASE_NOTES},
    {GetPromptString(PromptKind::kReleaseNotesMerge), PROMPT_GIT_RELEASE_NOTES_MERGE},
    {GetPromptString(PromptKind::kGitCommitMessage), PROMPT_GIT_COMMIT_MSG}};

Config::Config() : clConfigItem("llm-config") {}

Config::~Config() {}

void Config::Load() { clConfig::Get().ReadItem(this); }

void Config::Save() { clConfig::Get().WriteItem(this); }

void Config::FromJSON(const JSONItem& json)
{
    std::scoped_lock lk{m_mutex};
    m_selectedModel = json.namedObject("selected_model").toString();
    m_history = json.namedObject("history").toArrayString({});
    m_prompts = json.namedObject("prompts").toStringMap(kDefaultPromptTable);

    // Merge new entries from the "kDefaultPromptTable" into the stored
    for (const auto& [k, v] : kDefaultPromptTable) {
        if (m_prompts.count(k)) {
            continue;
        }
        m_prompts.insert({k, v});
    }
}

JSONItem Config::ToJSON() const
{
    std::scoped_lock lk{m_mutex};
    auto obj = JSONItem::createObject(GetName());
    if (m_history.size() > kMaxHistorySize) {
        m_history.resize(kMaxHistorySize);
    }

    obj.addProperty("selected_model", m_selectedModel);
    obj.addProperty("history", m_history);
    obj.addProperty("prompts", m_prompts);
    return obj;
}

void Config::AddHistory(const wxString& prompt)
{
    std::scoped_lock lk{m_mutex};
    int where = m_history.Index(prompt);
    if (where != wxNOT_FOUND) {
        m_history.RemoveAt(where);
    }

    // Insert at the top
    m_history.Insert(prompt, 0);
}

wxString Config::GetPrompt(PromptKind kind) const
{
    if (kind == PromptKind::kMax) {
        return wxEmptyString;
    }

    std::scoped_lock lk{m_mutex};
    wxString key = GetPromptString(kind);
    if (m_prompts.count(key)) {
        return m_prompts.find(key)->second;
    }
    return wxEmptyString;
}

void Config::SetPrompt(PromptKind kind, const wxString& prompt)
{
    wxString label = GetPromptString(kind);
    std::scoped_lock lk{m_mutex};
    switch (kind) {
    case PromptKind::kMax:
        break;
    default:
        m_prompts.erase(label);
        m_prompts.insert({label, prompt});
        break;
    }
}
} // namespace llm
