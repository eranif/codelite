//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SFTPBookmark.h
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

#ifndef SFTPBOOKMARK_H
#define SFTPBOOKMARK_H

#include <wx/string.h>
#include "JSON.h"
#include "ssh_account_info.h"

class SFTPBookmark
{
    wxString        m_name;
    SSHAccountInfo  m_account;
    wxString        m_folder;

public:
    SFTPBookmark();
    ~SFTPBookmark();

    void SetAccount(const SSHAccountInfo& account) {
        this->m_account = account;
    }
    const SSHAccountInfo& GetAccount() const {
        return m_account;
    }
    void SetFolder(const wxString& folder) {
        this->m_folder = folder;
    }
    const wxString& GetFolder() const {
        return m_folder;
    }

    void SetName(const wxString& name) {
        this->m_name = name;
    }
    const wxString& GetName() const {
        return m_name;
    }

    //-----------------------
    // Seerialization API
    //-----------------------
    JSONItem ToJSON() const;
    void FromJSON(const JSONItem &json);
};

#endif // SFTPBOOKMARK_H
