//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : sftp_settings.cpp
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

#include "fileutils.h"
#include "sftp_settings.h"
#include <algorithm>
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#include <wx/volume.h>
#endif

#if USE_SFTP

SFTPSettings::SFTPSettings()
    : clConfigItem("sftp-settings")
#ifdef __WXMSW__
    , m_sshClient("putty")
#else
    , m_sshClient("ssh")
#endif
{
}

SFTPSettings::~SFTPSettings() {}

void SFTPSettings::FromJSON(const JSONElement& json)
{
    m_accounts.clear();
    m_sshClient = json.namedObject("sshClient").toString(m_sshClient);
    JSONElement arrAccounts = json.namedObject("accounts");
    int size = arrAccounts.arraySize();
    for(int i = 0; i < size; ++i) {
        SSHAccountInfo account;
        account.FromJSON(arrAccounts.arrayItem(i));
        m_accounts.push_back(account);
    }
}

JSONElement SFTPSettings::ToJSON() const
{
    JSONElement element = JSONElement::createObject(GetName());
    element.addProperty("sshClient", m_sshClient);
    JSONElement arrAccounts = JSONElement::createArray("accounts");
    element.append(arrAccounts);
    for(size_t i = 0; i < m_accounts.size(); ++i) {
        arrAccounts.append(m_accounts.at(i).ToJSON());
    }
    return element;
}

SFTPSettings& SFTPSettings::Load()
{
    clConfig config("sftp-settings.conf");
    config.ReadItem(this);
    MSWImportPuTTYAccounts();
    return *this;
}

SFTPSettings& SFTPSettings::Save()
{
    clConfig config("sftp-settings.conf");
    config.WriteItem(this);
    return *this;
}

bool SFTPSettings::GetAccount(const wxString& name, SSHAccountInfo& account) const
{
    for(size_t i = 0; i < m_accounts.size(); ++i) {
        const SSHAccountInfo& currentAccount = m_accounts.at(i);
        if(name == currentAccount.GetAccountName()) {
            account = currentAccount;
            return true;
        }
    }
    return false;
}

bool SFTPSettings::UpdateAccount(const SSHAccountInfo& account)
{
    for(size_t i = 0; i < m_accounts.size(); ++i) {
        SSHAccountInfo& currentAccount = m_accounts.at(i);
        if(account.GetAccountName() == currentAccount.GetAccountName()) {
            currentAccount = account;
            return true;
        }
    }
    return false;
}

void SFTPSettings::MSWImportPuTTYAccounts()
{
#ifdef __WXMSW__
    SSHAccountInfo::Vect_t puttyAccounts;
    {
        // 64 bit version
        // HKEY_CURRENT_USER\Software\SimonTatham\\PuTTY\Sessions
        wxRegKey regPutty(wxRegKey::HKCU, "Software\\SimonTatham\\PuTTY\\Sessions");
        if(regPutty.Exists()) {
            long index = wxNOT_FOUND;
            wxString strKeyName;
            bool res = regPutty.GetFirstKey(strKeyName, index);
            while(res) {
                wxString hostname;
                wxString username;
                long port = wxNOT_FOUND;
                wxRegKey rk(wxRegKey::HKCU, "Software\\SimonTatham\\PuTTY\\Sessions\\" + strKeyName);
                if(rk.Exists()) {
                    rk.QueryValue("HostName", hostname);
                    int where = hostname.Find('@');
                    if(where != wxNOT_FOUND) {
                        hostname = hostname.Mid(where + 1);
                        username = hostname.Mid(where);
                    }
                    rk.QueryValue("PortNumber", &port);
                    if(!hostname.IsEmpty() && port != wxNOT_FOUND) {
                        SSHAccountInfo acc;
                        acc.SetAccountName(FileUtils::DecodeURI(strKeyName));
                        acc.SetHost(hostname);
                        acc.SetPort(port);
                        acc.SetUsername(username);
                        puttyAccounts.push_back(acc);
                    }
                }
                res = regPutty.GetNextKey(strKeyName, index);
            }
        }
    }

    {
        // 32 bit version
        // HKEY_CURRENT_USER\Software\SimonTatham\\PuTTY\Sessions
        wxRegKey regPutty(wxRegKey::HKCU, "Software\\Wow6432Node\\SimonTatham\\PuTTY\\Sessions");
        if(regPutty.Exists()) {
            long index = wxNOT_FOUND;
            wxString strKeyName;
            bool res = regPutty.GetFirstKey(strKeyName, index);
            while(res) {
                wxString hostname;
                wxString username;
                long port = wxNOT_FOUND;
                wxRegKey rk(wxRegKey::HKCU, "Software\\Wow6432Node\\SimonTatham\\PuTTY\\Sessions\\" + strKeyName);
                if(rk.Exists()) {
                    rk.QueryValue("HostName", hostname);
                    int where = hostname.Find('@');
                    if(where != wxNOT_FOUND) {
                        hostname = hostname.Mid(where + 1);
                        username = hostname.Mid(where);
                    }
                    rk.QueryValue("PortNumber", &port);
                    if(!hostname.IsEmpty() && port != wxNOT_FOUND) {
                        SSHAccountInfo acc;
                        acc.SetAccountName(FileUtils::DecodeURI(strKeyName));
                        acc.SetHost(hostname);
                        acc.SetPort(port);
                        acc.SetUsername(username);
                        puttyAccounts.push_back(acc);
                    }
                }
                res = regPutty.GetNextKey(strKeyName, index);
            }
        }
    }

    std::for_each(puttyAccounts.begin(), puttyAccounts.end(), [&](const SSHAccountInfo& acc) {
        SSHAccountInfo::Vect_t::iterator iter =
            std::find_if(m_accounts.begin(), m_accounts.end(),
                         [&](const SSHAccountInfo& a) { return a.GetAccountName() == acc.GetAccountName(); });
        if(iter == m_accounts.end()) {
            // No such account
            // add it
            m_accounts.push_back(acc);
        }
    });
#endif
}
#endif // USE_SFTP
