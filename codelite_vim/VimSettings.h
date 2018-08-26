#ifndef VIMSETTINGS_H
#define VIMSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class VimSettings : public clConfigItem
{
    bool m_enabled;

public:
    VimSettings();
    virtual ~VimSettings();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    VimSettings& Load();
    VimSettings& Save();
    VimSettings& SetEnabled(bool enabled)
    {
        this->m_enabled = enabled;
        return *this;
    }
    bool IsEnabled() const { return m_enabled; }
};

#endif // VIMSETTINGS_H
