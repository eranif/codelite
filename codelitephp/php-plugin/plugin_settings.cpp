#include "plugin_settings.h"

PluginSettings::PluginSettings()
    : clConfigItem("php-plugin")
{
}

PluginSettings::~PluginSettings()
{
}

void PluginSettings::FromJSON(const JSONItem& json)
{
    m_openWorkspaceFilter = json.namedObject("m_openWorkspaceFilter").toString();
}

JSONItem PluginSettings::ToJSON() const
{
    JSONItem e = JSONItem::createObject(GetName());
    e.addProperty("m_openWorkspaceFilter", m_openWorkspaceFilter);
    return e;
}

void PluginSettings::Load(PluginSettings& settings)
{
    clConfig::Get().ReadItem( &settings );
}

void PluginSettings::Save(const PluginSettings& settings)
{
    clConfig::Get().WriteItem( &settings );
}
