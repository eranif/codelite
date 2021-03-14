#include "RemotyNewWorkspaceDlg.h"
#include "globals.h"
#include "ssh_account_info.h"
RemotyNewWorkspaceDlg::RemotyNewWorkspaceDlg(wxWindow* parent)
    : RemotyNewWorkspaceDlgBase(parent)
{
    m_textCtrlPath->SetEditable(false);
    GetSizer()->Fit(this);
    CenterOnParent();
}

RemotyNewWorkspaceDlg::~RemotyNewWorkspaceDlg() {}

void RemotyNewWorkspaceDlg::OnBrowse(wxCommandEvent& event)
{
    auto result = ::clRemoteFileSelector(_("Seelct a folder"));
    if(result.second.empty() || result.second.empty()) {
        return;
    }
    m_textCtrlPath->SetEditable(true);
    m_textCtrlPath->ChangeValue(result.second);
    m_textCtrlPath->SetEditable(false);
    m_account = result.first;
}

void RemotyNewWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_account.empty() && !m_textCtrlPath->GetValue().empty() && !m_textCtrlName->GetValue().empty());
}

void RemotyNewWorkspaceDlg::GetData(wxString& name, wxString& path, wxString& account)
{
    account = m_account;
    path = m_textCtrlPath->GetValue();
    name = m_textCtrlName->GetValue();
}
