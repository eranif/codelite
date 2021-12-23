//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : SSHAccountManagerDlg.h
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

#ifndef SSHACCOUNTMANAGERDLG_H
#define SSHACCOUNTMANAGERDLG_H
#if USE_SFTP
#include "AddSSHAcountDlg.h"
#include "sftp_ui.h"

class WXDLLIMPEXP_SDK SSHAccountManagerDlg : public SSHAccountManagerDlgBase
{
public:
    SSHAccountManagerDlg(wxWindow* parent);
    virtual ~SSHAccountManagerDlg();

protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnEditAccountUI(wxUpdateUIEvent& event);
    virtual void OnDeleteAccountUI(wxUpdateUIEvent& event);
    void DoAddAccount(const SSHAccountInfo& account);
    void DoEditAccount(const wxDataViewItem& item);

public:
    SSHAccountInfo::Vect_t GetAccounts() const;

protected:
    virtual void OnAddAccount(wxCommandEvent& event);
    virtual void OnDeleteAccount(wxCommandEvent& event);
    virtual void OnEditAccount(wxCommandEvent& event);
};
#endif // USE_SFTP
#endif // SSHACCOUNTMANAGERDLG_H
