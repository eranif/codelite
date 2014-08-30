#include "plugin_settings.h"

PluginSettings::PluginSettings()
    : clConfigItem("php-plugin")
{
}

PluginSettings::~PluginSettings()
{
}

void PluginSettings::FromJSON(const JSONElement& json)
{
    m_openWorkspaceFilter = json.namedObject("m_openWorkspaceFilter").toString();
}

JSONElement PluginSettings::ToJSON() const
{
    JSONElement e = JSONElement::createObject(GetName());
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
