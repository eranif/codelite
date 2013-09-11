#include "AddSSHAcountDlg.h"

AddSSHAcountDlg::AddSSHAcountDlg(wxWindow* parent)
    : AddSSHAcountDlgBase(parent)
{
}

AddSSHAcountDlg::~AddSSHAcountDlg()
{
}
void AddSSHAcountDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlHost->IsEmpty() && !m_textCtrlPassword->IsEmpty() && !m_textCtrlPort->IsEmpty() && !m_textCtrlUsername->IsEmpty());
}

void AddSSHAcountDlg::GetAccountInfo(SSHAccountInfo& info)
{
    info.SetHost( m_textCtrlHost->GetValue() );
    info.SetPassword( m_textCtrlPassword->GetValue() );
    
    long port = 22;
    m_textCtrlPort->GetValue().ToCLong( &port );
    info.SetPort( port );
    info.SetUsername( m_textCtrlUsername->GetValue() );
}
