//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : AddSSHAcountDlg.h
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

#ifndef ADDSSHACOUNTDLG_H
#define ADDSSHACOUNTDLG_H

#if USE_SFTP

#include "sftp_ui.h"
#include "ssh_account_info.h"

class WXDLLIMPEXP_SDK AddSSHAcountDlg : public AddSSHAcountDlgBase
{
public:
    AddSSHAcountDlg(wxWindow* parent);
    AddSSHAcountDlg(wxWindow* parent, const SSHAccountInfo& account);
    virtual ~AddSSHAcountDlg();

public:
    void GetAccountInfo(SSHAccountInfo& info);

protected:
    virtual void OnHomeFolderUpdated(wxCommandEvent& event);
    virtual void OnTestConnectionUI(wxUpdateUIEvent& event);
    virtual void OnTestConnection(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // USE_SFTP
#endif // ADDSSHACOUNTDLG_H
