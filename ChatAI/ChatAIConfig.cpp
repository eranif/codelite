#include "ChatAIConfig.hpp"

#include "cl_config.h"

constexpr size_t kMaxHistorySize = 50;

ChatAIConfig::ChatAIConfig()
    : clConfigItem("chat-ai")
{
}

ChatAIConfig::~ChatAIConfig() {}

void ChatAIConfig::Load() { clConfig::Get().ReadItem(this); }

void ChatAIConfig::Save() { clConfig::Get().WriteItem(this); }

void ChatAIConfig::FromJSON(const JSONItem& json)
{
    m_selectedModel = json.namedObject("selected_model").toString();
    m_history = json.namedObject("history").toArrayString({});
}

JSONItem ChatAIConfig::ToJSON() const
{
    auto obj = JSONItem::createObject(GetName());
    if (m_history.size() > kMaxHistorySize) {
        m_history.resize(kMaxHistorySize);
    }
    obj.addProperty("selected_model", m_selectedModel);
    obj.addProperty("history", m_history);
    return obj;
}

void ChatAIConfig::AddHistory(const wxString& prompt)
{
    int where = m_history.Index(prompt);
    if (where != wxNOT_FOUND) {
        m_history.RemoveAt(where);
    }

    // Insert at the top
    m_history.Insert(prompt, 0);
}
