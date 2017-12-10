//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : sftp_settings.h
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

#ifndef SFTPSETTINGS_H
#define SFTPSETTINGS_H
#if USE_SFTP

#include "cl_config.h" // Base class: clConfigItem
#include "ssh_account_info.h"

class WXDLLIMPEXP_SDK SFTPSettings : public clConfigItem
{
    SSHAccountInfo::Vect_t m_accounts;
    wxString m_sshClient;

private:
    void MSWImportPuTTYAccounts();

public:
    SFTPSettings();
    virtual ~SFTPSettings();

    void SetAccounts(const SSHAccountInfo::Vect_t& accounts) { this->m_accounts = accounts; }
    const SSHAccountInfo::Vect_t& GetAccounts() const { return m_accounts; }

    /**
     * @brief update account details
     */
    bool UpdateAccount(const SSHAccountInfo& account);

    void SetSshClient(const wxString& sshClient) { this->m_sshClient = sshClient; }
    const wxString& GetSshClient() const { return m_sshClient; }
    bool GetAccount(const wxString& name, SSHAccountInfo& account) const;
    SFTPSettings& Load();
    SFTPSettings& Save();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
};
#endif // USE_SFTP
#endif // SFTPSETTINGS_H
