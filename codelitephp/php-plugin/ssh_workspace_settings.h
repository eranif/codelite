#ifndef SSHWORKSPACESETTINGS_H
#define SSHWORKSPACESETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class SSHWorkspaceSettings : public clConfigItem
{
    wxString m_account;
    wxString m_remoteFolder;
    bool     m_remoteUploadEnabled;

public:
    SSHWorkspaceSettings();
    virtual ~SSHWorkspaceSettings();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    void Load();
    void Save();

    void EnableRemoteUpload(bool b) {
        m_remoteUploadEnabled  = b;
    }

    bool IsRemoteUploadSet() const;
    bool IsRemoteUploadEnabled() const {
        return m_remoteUploadEnabled;
    }
    void SetAccount(const wxString& account) {
        this->m_account = account;
    }
    void SetRemoteFolder(const wxString& remoteFolder) {
        this->m_remoteFolder = remoteFolder;
    }
    const wxString& GetAccount() const {
        return m_account;
    }
    const wxString& GetRemoteFolder() const {
        return m_remoteFolder;
    }
};

#endif // SSHWORKSPACESETTINGS_H
