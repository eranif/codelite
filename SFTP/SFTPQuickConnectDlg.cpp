#include "SFTPQuickConnectDlg.h"
#include "cl_config.h"
#include "sftp_settings.h"
#include "windowattrmanager.h"

SFTPQuickConnectDlg::SFTPQuickConnectDlg(wxWindow* parent)
    : SFTPQuickConnectBaseDlg(parent)
{
    SFTPSettings settings;
    settings.Load();

    // Read previous settings
    bool selecteExistingAccount = true;
    wxString selectedAccount;
    wxString host;
    wxString user;
    wxString port;

    selecteExistingAccount = clConfig::Get().Read("SFTPQuickConnect/ChooseExistingAccount", selecteExistingAccount);
    selectedAccount = clConfig::Get().Read("SFTPQuickConnect/SelectedAccount", selectedAccount);
    host = clConfig::Get().Read("SFTPQuickConnect/Host", wxString());
    user = clConfig::Get().Read("SFTPQuickConnect/User", wxString());
    port = clConfig::Get().Read("SFTPQuickConnect/Port", wxString("22"));

    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    SSHAccountInfo::Vect_t::const_iterator iter = accounts.begin();
    for(; iter != accounts.end(); ++iter) {
        m_choiceAccount->Append(iter->GetAccountName());
    }
    int where = m_choiceAccount->FindString(selectedAccount);
    if(where != wxNOT_FOUND) { m_choiceAccount->SetSelection(where); }

    m_checkBoxChooseAccount->SetValue(selecteExistingAccount);
    m_checkBoxQuickConnect->SetValue(!selecteExistingAccount);
    m_textCtrlHost->ChangeValue(host);
    m_textCtrlUsername->ChangeValue(user);
    m_textCtrlPort->ChangeValue(port);
    WindowAttrManager::Load(this);
}

SFTPQuickConnectDlg::~SFTPQuickConnectDlg()
{
    clConfig::Get().Write("SFTPQuickConnect/ChooseExistingAccount", m_checkBoxChooseAccount->IsChecked());
    clConfig::Get().Write("SFTPQuickConnect/SelectedAccount", m_choiceAccount->GetStringSelection());
    clConfig::Get().Write("SFTPQuickConnect/Host", m_textCtrlHost->GetValue());
    clConfig::Get().Write("SFTPQuickConnect/User", m_textCtrlUsername->GetValue());
    clConfig::Get().Write("SFTPQuickConnect/Port", m_textCtrlPort->GetValue());
}

void SFTPQuickConnectDlg::OnCheckboxChooseAccount(wxCommandEvent& event)
{
    m_checkBoxQuickConnect->SetValue(!event.IsChecked());
}

void SFTPQuickConnectDlg::OnChooseAccountUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxChooseAccount->IsChecked());
}

void SFTPQuickConnectDlg::OnOKUI(wxUpdateUIEvent& event) {}

void SFTPQuickConnectDlg::OnQuickConnect(wxCommandEvent& event)
{
    m_checkBoxChooseAccount->SetValue(!event.IsChecked());
}

void SFTPQuickConnectDlg::OnQuickConnectUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxQuickConnect->IsChecked());
}

SSHAccountInfo SFTPQuickConnectDlg::GetSelectedAccount() const
{
    SSHAccountInfo acc;
    if(m_checkBoxChooseAccount->IsChecked()) {
        SFTPSettings settings;
        settings.Load();
        settings.GetAccount(m_choiceAccount->GetStringSelection(), acc);
    } else {
        // Create a dummy account
        acc.SetUsername(m_textCtrlUsername->GetValue());
        acc.SetAccountName(wxString() << m_textCtrlUsername->GetValue() << "@" << m_textCtrlHost->GetValue());
        acc.SetPassword(m_textCtrlPassword->GetValue());
        acc.SetHost(m_textCtrlHost->GetValue());
        long nPort = 22; // Default is 22
        if(!m_textCtrlPort->GetValue().ToCLong(&nPort)) { nPort = 22; }
        acc.SetPort(nPort);
    }
    return acc;
}
