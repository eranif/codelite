#include "AutoSaveSettings.h"

AutoSaveSettings::AutoSaveSettings()
    : clConfigItem("auto-save")
    , m_flags(0)
    , m_checkInterval(5)
{
}

AutoSaveSettings::~AutoSaveSettings() {}

void AutoSaveSettings::FromJSON(const JSONElement& json)
{
    m_flags = json.namedObject("m_flags").toSize_t(m_flags);
    m_checkInterval = json.namedObject("m_checkInterval").toSize_t(m_checkInterval);
}

JSONElement AutoSaveSettings::ToJSON() const
{
    JSONElement json = JSONElement::createObject(GetName());
    json.addProperty("m_flags", m_flags);
    json.addProperty("m_checkInterval", m_checkInterval);
    return json;
}

AutoSaveSettings AutoSaveSettings::Load()
{
    AutoSaveSettings settings;
    clConfig config("auto-save.conf");
    config.ReadItem(&settings);
    return settings;
}

void AutoSaveSettings::Save(const AutoSaveSettings& settings)
{
    clConfig config("auto-save.conf");
    config.WriteItem(&settings);
}
