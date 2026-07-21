#pragma once

// clang-format off
#include <wx/wx.h>
// clang-format on

#include "UI.hpp"
#include "ssh/ssh_account_info.h"

class SFTPQuickConnectDlg : public SFTPQuickConnectBaseDlg
{
public:
    SFTPQuickConnectDlg(wxWindow* parent);
    virtual ~SFTPQuickConnectDlg();
    SSHAccountInfo GetSelectedAccount() const;

protected:
    void OnPageChanged(wxNotebookEvent& event) override;
    void OnOKUI(wxUpdateUIEvent& event) override;
};
