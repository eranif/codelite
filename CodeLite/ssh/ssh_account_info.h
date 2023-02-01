//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : ssh_account_info.h
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

#ifndef SSHACCOUNTINFO_H
#define SSHACCOUNTINFO_H

#include "cl_config.h" // Base class: clConfigItem
#include "codelite_exports.h"

#include <functional>
#include <vector>

class WXDLLIMPEXP_CL SSHAccountInfo : public clConfigItem
{
    wxString m_accountName;
    wxString m_username;
    wxString m_password;
    int m_port;
    wxString m_host;
    wxArrayString m_bookmarks;
    wxString m_defaultFolder;

public:
    typedef std::vector<SSHAccountInfo> Vect_t;

public:
    SSHAccountInfo();

    virtual ~SSHAccountInfo();
    SSHAccountInfo& operator=(const SSHAccountInfo& other);

    void AddBookmark(const wxString& location);

    void SetBookmarks(const wxArrayString& bookmarks) { this->m_bookmarks = bookmarks; }
    const wxArrayString& GetBookmarks() const { return m_bookmarks; }
    void SetAccountName(const wxString& accountName) { this->m_accountName = accountName; }
    const wxString& GetAccountName() const { return m_accountName; }
    void SetHost(const wxString& host) { this->m_host = host; }
    void SetPassword(const wxString& password) { this->m_password = password; }
    void SetPort(int port) { this->m_port = port; }
    void SetUsername(const wxString& username) { this->m_username = username; }
    const wxString& GetHost() const { return m_host; }
    const wxString& GetPassword() const { return m_password; }
    int GetPort() const { return m_port; }
    const wxString& GetUsername() const { return m_username; }
    void SetDefaultFolder(const wxString& defaultFolder) { this->m_defaultFolder = defaultFolder; }
    const wxString& GetDefaultFolder() const { return m_defaultFolder; }

    /**
     * @brief read list of accounts from the JSON file
     * @param matcher a callback that allows the user to filter matches
     */
    static SSHAccountInfo::Vect_t Load(const std::function<bool(const SSHAccountInfo&)>& matcher = nullptr);
    /**
     * @brief load a specific account from configuration
     */
    static SSHAccountInfo LoadAccount(const wxString& accountName);

    /**
     * @brief return user defined SSH client
     */
    static wxString GetSSHClient();

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;
};
#endif // SSHACCOUNTINFO_H
