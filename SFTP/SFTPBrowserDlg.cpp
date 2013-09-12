#include "SFTPBrowserDlg.h"
#include "sftp_settings.h"
#include "ssh_account_info.h"
#include <wx/msgdlg.h>

SFTPBrowserDlg::SFTPBrowserDlg(wxWindow* parent)
    : SFTPBrowserBaseDlg(parent)
    , m_ssh(NULL)
{
    
}

SFTPBrowserDlg::~SFTPBrowserDlg()
{
}

void SFTPBrowserDlg::OnRefresh(wxCommandEvent& event)
{
    DoCloseSession();
    wxString accountName = m_choiceAccount->GetStringSelection();
    
    SFTPSettings settings;
    SFTPSettings::Load( settings );
    
    SSHAccountInfo account;
    if ( !settings.GetAccount( accountName, account) ) {
        ::wxMessageBox(wxString() << _("Could not find account: ") << accountName, "codelite", wxICON_ERROR|wxOK);
        return;
    }
}

void SFTPBrowserDlg::OnRefreshUI(wxUpdateUIEvent& event)
{
}

void SFTPBrowserDlg::DoCloseSession()
{
    wxDELETE(m_ssh);
    m_dataviewModel->Clear();
}
