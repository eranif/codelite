//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : AddSSHAccountDlg.h
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

#pragma once

#if USE_SFTP

#include "sftp_ui.hpp"
#include "ssh/ssh_account_info.h"

class WXDLLIMPEXP_SDK AddSSHAccountDlg : public AddSSHAccountDlgBase
{
public:
    explicit AddSSHAccountDlg(wxWindow* parent);
    AddSSHAccountDlg(wxWindow* parent, const SSHAccountInfo& account);
    ~AddSSHAccountDlg() override = default;

public:
    void GetAccountInfo(SSHAccountInfo& info);

protected:
    void OnHomeFolderUpdated(wxCommandEvent& event) override;
    void OnTestConnectionUI(wxUpdateUIEvent& event) override;
    void OnTestConnection(wxCommandEvent& event) override;
    void OnOKUI(wxUpdateUIEvent& event) override;
};
#endif // USE_SFTP
