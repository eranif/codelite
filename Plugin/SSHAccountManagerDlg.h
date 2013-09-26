#ifndef SSHACCOUNTMANAGERDLG_H
#define SSHACCOUNTMANAGERDLG_H

#include "sftp_ui.h"
#include "AddSSHAcountDlg.h"

class WXDLLIMPEXP_SDK SSHAccountManagerDlg : public SSHAccountManagerDlgBase
{
public:
    SSHAccountManagerDlg(wxWindow* parent);
    virtual ~SSHAccountManagerDlg();

protected:
    virtual void OnDeleteAccountUI(wxUpdateUIEvent& event);
    void DoAddAccount( const SSHAccountInfo& account );

public:
    SSHAccountInfo::List_t GetAccounts() const;

protected:
    virtual void OnAddAccount(wxCommandEvent& event);
    virtual void OnDeleteAccount(wxCommandEvent& event);
    virtual void OnEditAccount(wxDataViewEvent& event);
};
#endif // SSHACCOUNTMANAGERDLG_H
