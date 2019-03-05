#include "WordCompletionSettings.h"

WordCompletionSettings::WordCompletionSettings()
    : clConfigItem("WordCompletionSettings")
    , m_comparisonMethod(kComparisonStartsWith)
    , m_enabled(true)
{
}

WordCompletionSettings::~WordCompletionSettings() {}

void WordCompletionSettings::FromJSON(const JSONItem& json)
{
    m_comparisonMethod = json.namedObject("m_comparisonMethod").toInt(m_comparisonMethod);
    m_enabled = json.namedObject("m_enabled").toBool(m_enabled);
}

JSONItem WordCompletionSettings::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_comparisonMethod", m_comparisonMethod);
    element.addProperty("m_enabled", m_enabled);
    return element;
}

WordCompletionSettings& WordCompletionSettings::Load()
{
    clConfig config("word-completion.conf");
    config.ReadItem(this);
    return *this;
}

WordCompletionSettings& WordCompletionSettings::Save()
{
    clConfig config("word-completion.conf");
    config.WriteItem(this);
    return *this;
}
