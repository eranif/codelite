#include "Config.hpp"

#include "cl_config.h"

namespace llm
{
constexpr size_t kMaxHistorySize = 50;

Config::Config()
    : clConfigItem("llm-config")
{
}

Config::~Config() {}

void Config::Load() { clConfig::Get().ReadItem(this); }

void Config::Save() { clConfig::Get().WriteItem(this); }

void Config::FromJSON(const JSONItem& json)
{
    std::scoped_lock lk{m_mutex};
    m_selectedModel = json.namedObject("selected_model").toString();
    m_history = json.namedObject("history").toArrayString({});
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
} // namespace llm
