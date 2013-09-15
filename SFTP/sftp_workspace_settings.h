#ifndef SFTPWORKSPACESETTINGS_H
#define SFTPWORKSPACESETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class SFTPWorkspaceSettings : public clConfigItem
{
    wxString m_account;
    wxString m_remoteWorkspacePath;

public:
    SFTPWorkspaceSettings();
    virtual ~SFTPWorkspaceSettings();

    static void Load(SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile);
    static void Save(const SFTPWorkspaceSettings& settings, const wxFileName& workspaceFile);

    void SetAccount(const wxString& account) {
        this->m_account = account;
    }
    void SetRemoteWorkspacePath(const wxString& remoteWorkspacePath) {
        this->m_remoteWorkspacePath = remoteWorkspacePath;
    }
    const wxString& GetAccount() const {
        return m_account;
    }
    const wxString& GetRemoteWorkspacePath() const {
        return m_remoteWorkspacePath;
    }
    
public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
};

#endif // SFTPWORKSPACESETTINGS_H
