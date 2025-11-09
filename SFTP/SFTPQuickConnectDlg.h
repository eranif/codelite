#ifndef SFTPQUICKCONNECTDLG_H
#define SFTPQUICKCONNECTDLG_H

// clang-format off
#include <wx/wx.h>
// clang-format on

#include "UI.h"
#include "ssh/ssh_account_info.h"

class SFTPQuickConnectDlg : public SFTPQuickConnectBaseDlg
{
public:
    SFTPQuickConnectDlg(wxWindow* parent);
    virtual ~SFTPQuickConnectDlg();
    SSHAccountInfo GetSelectedAccount() const;

protected:
    virtual void OnCheckboxChooseAccount(wxCommandEvent& event);
    virtual void OnChooseAccountUI(wxUpdateUIEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnQuickConnect(wxCommandEvent& event);
    virtual void OnQuickConnectUI(wxUpdateUIEvent& event);
};
#endif // SFTPQUICKCONNECTDLG_H
