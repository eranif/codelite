#ifndef WORKSPACESVNSETTINGS_H
#define WORKSPACESVNSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class WorkspaceSvnSettings : public clConfigItem
{
    wxString m_repoPath;
    wxFileName m_workspaceFileName;
    
public:
    WorkspaceSvnSettings(const wxFileName& fn);
    WorkspaceSvnSettings();
    virtual ~WorkspaceSvnSettings();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    WorkspaceSvnSettings& Load();
    void Save();

    void SetRepoPath(const wxString& repoPath) {
        this->m_repoPath = repoPath;
    }
    const wxString& GetRepoPath() const {
        return m_repoPath;
    }
    wxFileName GetLocalConfigFile() const;
};

#endif // WORKSPACESVNSETTINGS_H
