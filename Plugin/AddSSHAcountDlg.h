#ifndef ADDSSHACOUNTDLG_H
#define ADDSSHACOUNTDLG_H

#include "sftp_ui.h"
#include "ssh_account_info.h"

class WXDLLIMPEXP_SDK AddSSHAcountDlg : public AddSSHAcountDlgBase
{
public:
    AddSSHAcountDlg(wxWindow* parent);
    AddSSHAcountDlg(wxWindow* parent, const SSHAccountInfo &account);
    virtual ~AddSSHAcountDlg();

public:
    void GetAccountInfo(SSHAccountInfo& info);

protected:
    virtual void OnTestConnectionUI(wxUpdateUIEvent& event);
    virtual void OnTestConnection(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // ADDSSHACOUNTDLG_H
