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
#include <wx/clntdata.h>
#include "wxStringHash.h"

class RemoteFileInfo : public wxClientData
{
    SSHAccountInfo m_account; // the account for which this file belongs to
    wxString m_localFile;
    wxString m_remoteFile;
    size_t m_premissions;
    int m_lineNumber = 0;

public:
    typedef std::unordered_map<wxString, RemoteFileInfo> Map_t;

public:
    RemoteFileInfo();
    virtual ~RemoteFileInfo();

    void SetAccount(const SSHAccountInfo& account) { this->m_account = account; }
    const SSHAccountInfo& GetAccount() const { return m_account; }
    const wxString& GetLocalFile() const { return m_localFile; }
    void SetRemoteFile(const wxString& remoteFile);
    const wxString& GetRemoteFile() const { return m_remoteFile; }
    void SetPremissions(size_t premissions) { this->m_premissions = premissions; }
    size_t GetPremissions() const { return m_premissions; }

    void SetLineNumber(int lineNumber) { this->m_lineNumber = lineNumber; }
    int GetLineNumber() const { return m_lineNumber; }
};

#endif // REMOTEFILEINFO_H
