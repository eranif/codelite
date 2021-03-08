//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ssh_account_info.cpp
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

#include "ssh_account_info.h"
#include "xor_string.h"

SSHAccountInfo::SSHAccountInfo()
    : clConfigItem("ssh-account")
    , m_port(22)
{
}

SSHAccountInfo::~SSHAccountInfo() {}

SSHAccountInfo& SSHAccountInfo::operator=(const SSHAccountInfo& other)
{
    if(&other == this)
        return *this;

    m_accountName = other.m_accountName;
    m_username = other.m_username;
    m_password = other.m_password;
    m_port = other.m_port;
    m_host = other.m_host;
    m_bookmarks = other.m_bookmarks;
    m_defaultFolder = other.m_defaultFolder;
    return *this;
}

void SSHAccountInfo::FromJSON(const JSONItem& json)
{
    m_accountName = json.namedObject("m_accountName").toString();
    m_username = json.namedObject("m_username").toString();
    wxString pass = json.namedObject("m_password").toString();
    XORString x(pass);
    m_password = x.Decrypt();
    m_port = json.namedObject("m_port").toInt(22);
    m_host = json.namedObject("m_host").toString();
    m_bookmarks = json.namedObject("m_bookmarks").toArrayString();
    m_defaultFolder = json.namedObject("m_defaultFolder").toString();
}

JSONItem SSHAccountInfo::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_accountName", m_accountName);
    element.addProperty("m_username", m_username);
    element.addProperty("m_port", m_port);
    element.addProperty("m_host", m_host);
    element.addProperty("m_bookmarks", m_bookmarks);
    element.addProperty("m_defaultFolder", m_defaultFolder);
    XORString x(m_password);
    element.addProperty("m_password", x.Encrypt());
    return element;
}

void SSHAccountInfo::AddBookmark(const wxString& location)
{
    if(m_bookmarks.Index(location) == wxNOT_FOUND) {
        m_bookmarks.Add(location);
    }
}

SSHAccountInfo::Vect_t SSHAccountInfo::Load(const function<bool(const SSHAccountInfo&)>& matcher)
{
    wxFileName jsonfile(clStandardPaths::Get().GetUserDataDir(), "sftp-settings.conf");
    jsonfile.AppendDir("config");

    JSON root(jsonfile);
    if(!root.isOk()) {
        return {};
    }
    JSONItem s = root.toElement()["sftp-settings"];
    auto accounts = s["accounts"];
    size_t count = accounts.arraySize();
    if(count == 0) {
        return {};
    }
    Vect_t accountsVec;
    accountsVec.reserve(count);
    for(size_t i = 0; i < count; ++i) {
        auto account = accounts[i];
        SSHAccountInfo ai;
        ai.FromJSON(account);
        if(!matcher || (matcher && matcher(ai))) {
            accountsVec.emplace_back(ai);
        }
    }
    return accountsVec;
}

SSHAccountInfo SSHAccountInfo::LoadAccount(const wxString& accountName)
{
    auto res = Load([&](const SSHAccountInfo& ai) { return ai.GetAccountName() == accountName; });
    if(res.size() == 1) {
        return res[0];
    }
    return {};
}

wxString SSHAccountInfo::GetSSHClient()
{
    wxFileName jsonfile(clStandardPaths::Get().GetUserDataDir(), "sftp-settings.conf");
    jsonfile.AppendDir("config");

    JSON root(jsonfile);
    if(!root.isOk()) {
        return wxEmptyString;
    }
    return root.toElement()["sftp-settings"]["sshClient"].toString();
}
