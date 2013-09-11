#include "SSHAccountManagerDlg.h"
#include "AddSSHAcountDlg.h"

SSHAccountManagerDlg::SSHAccountManagerDlg(wxWindow* parent)
    : SSHAccountManagerDlgBase(parent)
{
}

SSHAccountManagerDlg::~SSHAccountManagerDlg()
{
}
void SSHAccountManagerDlg::OnAddAccount(wxCommandEvent& event)
{
    AddSSHAcountDlg dlg(this);
    if ( dlg.ShowModal() == wxID_OK ) {
        SSHAccountInfo account;
        dlg.GetAccountInfo( account );
        
        DoAddAccount( account );
    }
}

void SSHAccountManagerDlg::OnDeleteAccount(wxCommandEvent& event)
{
}
void SSHAccountManagerDlg::OnEditAccount(wxDataViewEvent& event)
{
}

void SSHAccountManagerDlg::DoAddAccount(const SSHAccountInfo& account)
{
    wxVector<wxVariant> cols;
    cols.push_back( account.GetHost() );
    cols.push_back( account.GetUsername() );
    m_dvListCtrlAccounts->AppendItem( cols, (wxUIntPtr)(new SSHAccountInfo(account)));
}
