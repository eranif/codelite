//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : sftp_workspace_settings.h
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
    
    void Clear();
    
    bool IsOk() const {
        return !m_account.IsEmpty() && !m_remoteWorkspacePath.IsEmpty();
    }
    
public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;
};

#endif // SFTPWORKSPACESETTINGS_H
