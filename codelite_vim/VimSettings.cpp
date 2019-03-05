#include "VimSettings.h"

VimSettings::VimSettings()
    : clConfigItem("vim")
    , m_enabled(false)
{
}

VimSettings::~VimSettings() {}

void VimSettings::FromJSON(const JSONItem& json) { m_enabled = json.namedObject("enabled").toBool(m_enabled); }

JSONItem VimSettings::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("enabled", m_enabled);
    return element;
}

VimSettings& VimSettings::Load()
{
    clConfig config("vim.conf");
    config.ReadItem(this);
    return *this;
}

VimSettings& VimSettings::Save()
{
    clConfig config("vim.conf");
    config.WriteItem(this);
    return *this;
}
