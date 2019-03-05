//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : ssh_workspace_settings.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

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
    void Reset();
};

#endif // SSHWORKSPACESETTINGS_H
