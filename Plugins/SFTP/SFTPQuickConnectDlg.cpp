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

    for (const auto& account : settings.GetAccounts()) {
        m_choiceAccount->Append(account.GetAccountName());
    }
    int where = m_choiceAccount->FindString(selectedAccount);
    if (where != wxNOT_FOUND) {
        m_choiceAccount->SetSelection(where);
    }

    m_textCtrlHost->ChangeValue(host);
    m_textCtrlUsername->ChangeValue(user);
    m_textCtrlPort->ChangeValue(port);
    CenterOnParent();
}

SFTPQuickConnectDlg::~SFTPQuickConnectDlg()
{
    clConfig::Get().Write("SFTPQuickConnect/SelectedAccount", m_choiceAccount->GetStringSelection());
    clConfig::Get().Write("SFTPQuickConnect/Host", m_textCtrlHost->GetValue());
    clConfig::Get().Write("SFTPQuickConnect/User", m_textCtrlUsername->GetValue());
    clConfig::Get().Write("SFTPQuickConnect/Port", m_textCtrlPort->GetValue());
}

void SFTPQuickConnectDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(GetSelectedAccount().IsOk()); }

SSHAccountInfo SFTPQuickConnectDlg::GetSelectedAccount() const
{
    SSHAccountInfo acc;
    if (m_notebook->GetSelection() == 0) {
        // Connect by account name
        SFTPSettings settings;
        settings.Load();
        settings.GetAccount(m_choiceAccount->GetStringSelection(), acc);
    } else {
        // Create a dummy account
        acc.SetUsername(m_textCtrlUsername->GetValue());
        acc.SetAccountName(wxString() << m_textCtrlUsername->GetValue() << "@" << m_textCtrlHost->GetValue());
        acc.SetPassword(m_textCtrlPassword->GetValue());
        acc.SetHost(m_textCtrlHost->GetValue());
        long nPort{22}; // Default is 22
        if (!m_textCtrlPort->GetValue().ToCLong(&nPort)) {
            nPort = 22;
        }
        acc.SetPortNumber(nPort);
    }
    return acc;
}

void SFTPQuickConnectDlg::OnPageChanged(wxNotebookEvent& event)
{
    event.Skip();
    GetSizer()->Fit(this);
}
