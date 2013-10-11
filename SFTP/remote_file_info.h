//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : remote_file_info.h
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

#ifndef REMOTEFILEINFO_H
#define REMOTEFILEINFO_H

#include "ssh_account_info.h"
#include <wx/filename.h>
#include <map>

class RemoteFileInfo
{
    SSHAccountInfo m_account; // the account for which this file belongs to
    wxFileName     m_localFile;
public:
    typedef std::map<wxString, RemoteFileInfo> Map_t;
    
public:
    RemoteFileInfo();
    virtual ~RemoteFileInfo();

    void SetAccount(const SSHAccountInfo& account) {
        this->m_account = account;
    }
    void SetLocalFile(const wxFileName& localFile) {
        this->m_localFile = localFile;
    }
    const SSHAccountInfo& GetAccount() const {
        return m_account;
    }
    const wxFileName& GetLocalFile() const {
        return m_localFile;
    }
};

#endif // REMOTEFILEINFO_H
