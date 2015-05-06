#include "WordCompletionSettings.h"

WordCompletionSettings::WordCompletionSettings()
    : clConfigItem("WordCompletionSettings")
    , m_comparisonMethod(kComparisonStartsWith)
{
}

WordCompletionSettings::~WordCompletionSettings() {}

void WordCompletionSettings::FromJSON(const JSONElement& json)
{
    m_comparisonMethod = json.namedObject("m_comparisonMethod").toInt(m_comparisonMethod);
}

JSONElement WordCompletionSettings::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_comparisonMethod", m_comparisonMethod);
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
