#include "VimSettings.h"

VimSettings::VimSettings()
    : clConfigItem("vim")
    , m_enabled(false)
{
}

void VimSettings::FromJSON(const JSONItem& json) { m_enabled = json.namedObject("enabled").toBool(m_enabled); }

JSONItem VimSettings::ToJSON() const { return nlohmann::json{{"enabled", m_enabled}}; }

VimSettings& VimSettings::Load()
{
    clConfig config("vim.conf");
    config.ReadItem(*this);
    return *this;
}

VimSettings& VimSettings::Save()
{
    clConfig config("vim.conf");
    config.WriteItem(*this);
    return *this;
}
