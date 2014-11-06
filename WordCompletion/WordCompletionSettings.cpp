#include "WordCompletionSettings.h"

WordCompletionSettings::WordCompletionSettings()
    : clConfigItem("WordCompletionSettings")
    , m_completeTypes(kCompleteWords)
    , m_comparisonMethod(kComparisonStartsWith)
{
}

WordCompletionSettings::~WordCompletionSettings() {}

void WordCompletionSettings::FromJSON(const JSONElement& json)
{
    m_completeTypes = json.namedObject("m_completeTypes").toSize_t(m_completeTypes);
    m_comparisonMethod = json.namedObject("m_comparisonMethod").toInt(m_comparisonMethod);
}

JSONElement WordCompletionSettings::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("m_completeTypes", m_completeTypes);
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
