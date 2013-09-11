#ifndef ADDSSHACOUNTDLG_H
#define ADDSSHACOUNTDLG_H

#include "sftp_ui.h"
#include "ssh_account_info.h"

class AddSSHAcountDlg : public AddSSHAcountDlgBase
{
public:
    AddSSHAcountDlg(wxWindow* parent);
    virtual ~AddSSHAcountDlg();

public:
    void GetAccountInfo(SSHAccountInfo& info);
    
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // ADDSSHACOUNTDLG_H
