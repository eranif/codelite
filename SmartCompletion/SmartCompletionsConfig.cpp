#include "SmartCompletionsConfig.h"

SmartCompletionsConfig::SmartCompletionsConfig()
    : clConfigItem("settings")
    , m_flags(kEnabled)
{
}

SmartCompletionsConfig::~SmartCompletionsConfig() {}

SmartCompletionsConfig& SmartCompletionsConfig::Load()
{
    clConfig conf("SmartCompletions.conf");
    conf.ReadItem(this);
    m_db.Open();
    m_db.LoadCCUsageTable(m_CCweight);
    m_db.LoadGTAUsageTable(m_GTAweight);
    return *this;
}

SmartCompletionsConfig& SmartCompletionsConfig::Save()
{
    clConfig conf("SmartCompletions.conf");
    conf.WriteItem(this);
    return *this;
}

void SmartCompletionsConfig::FromJSON(const JSONItem& json)
{
    JSONItem e = json.namedObject(GetName());
    m_flags = e.namedObject("m_flags").toSize_t(m_flags);
}

JSONItem SmartCompletionsConfig::ToJSON() const
{
    JSONItem json = JSONItem::createObject(GetName());
    json.addProperty("m_flags", m_flags);
    return json;
}
