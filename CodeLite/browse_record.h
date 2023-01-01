//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : browse_record.h
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
#ifndef BROWSE_HISTORY_H
#define BROWSE_HISTORY_H

#include <wx/string.h>

class BrowseRecord
{
public:
    wxString filename;
    wxString project;
    int lineno = wxNOT_FOUND;
    int column = wxNOT_FOUND;
    int firstLineInView = wxNOT_FOUND;
    wxString ssh_account;

public:
    BrowseRecord() {}
    ~BrowseRecord() {}

    bool IsSameAs(const BrowseRecord& other) const
    {
        return filename == other.filename && lineno == other.lineno && ssh_account == other.ssh_account;
    }

    /**
     * @brief having a file name is enough for a jumping record
     */
    bool IsOk() const { return !filename.empty(); }

    /**
     * @brief is this record for a remote file?
     */
    bool IsRemote() const { return !ssh_account.empty(); }

    /**
     * @brief return the ssh account associated with this record
     */
    const wxString& GetSshAccount() const { return ssh_account; }
};

#endif // BROWSE_HISTORY_H
