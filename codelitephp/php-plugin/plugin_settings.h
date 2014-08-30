#ifndef PLUGINSETTINGS_H
#define PLUGINSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class PluginSettings : public clConfigItem
{
    wxString m_openWorkspaceFilter;

public:
    PluginSettings();
    virtual ~PluginSettings();
    
    void SetOpenWorkspaceFilter(const wxString& openWorkspaceFilter) {
        this->m_openWorkspaceFilter = openWorkspaceFilter;
    }
    const wxString& GetOpenWorkspaceFilter() const {
        return m_openWorkspaceFilter;
    }
    
    static void Load(PluginSettings& settings);
    static void Save(const PluginSettings& settings);
    
public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
};

#endif // PLUGINSETTINGS_H
