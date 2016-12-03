#include "EditorConfigSettings.h"
#include "cl_config.h"

EditorConfigSettings::EditorConfigSettings()
    : m_flags(kEnabled)
{
}

EditorConfigSettings::~EditorConfigSettings() {}

EditorConfigSettings& EditorConfigSettings::Load()
{
    m_flags = clConfig::Get().Read("EditorConfig/Flags", m_flags);
    return *this;
}

EditorConfigSettings& EditorConfigSettings::Save()
{
    clConfig::Get().Write("EditorConfig/Flags", m_flags);
    return *this;
}
