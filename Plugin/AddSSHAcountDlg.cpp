//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : AddSSHAcountDlg.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "AddSSHAcountDlg.h"
#if USE_SFTP
#include "cl_exception.h"
#include "cl_ssh.h"
#include "windowattrmanager.h"

#include <wx/msgdlg.h>

AddSSHAcountDlg::AddSSHAcountDlg(wxWindow* parent)
    : AddSSHAcountDlgBase(parent)
{
    SetName("AddSSHAcountDlg");
    WindowAttrManager::Load(this);
}

AddSSHAcountDlg::AddSSHAcountDlg(wxWindow* parent, const SSHAccountInfo& account)
    : AddSSHAcountDlgBase(parent)
{
    m_textCtrlHost->ChangeValue(account.GetHost());
    m_textCtrlPassword->ChangeValue(account.GetPassword());
    m_textCtrlPort->ChangeValue(wxString() << account.GetPort());
    m_textCtrlUsername->ChangeValue(account.GetUsername());
    m_textCtrlName->ChangeValue(account.GetAccountName());
    m_textCtrlHomeFolder->ChangeValue(account.GetDefaultFolder());
    SetName("AddSSHAcountDlg");
    WindowAttrManager::Load(this);
}

AddSSHAcountDlg::~AddSSHAcountDlg() {}

void AddSSHAcountDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxString homeFolder = m_textCtrlHomeFolder->GetValue();
    bool homeFolderValid = (homeFolder.IsEmpty() || (homeFolder.StartsWith("/")));

    event.Enable(!m_textCtrlHost->IsEmpty() && !m_textCtrlPort->IsEmpty() && !m_textCtrlUsername->IsEmpty() &&
                 homeFolderValid);
}

void AddSSHAcountDlg::GetAccountInfo(SSHAccountInfo& info)
{
    info.SetHost(m_textCtrlHost->GetValue());
    info.SetPassword(m_textCtrlPassword->GetValue());

    long port = 22;
    m_textCtrlPort->GetValue().ToCLong(&port);
    info.SetPort(port);
    info.SetUsername(m_textCtrlUsername->GetValue());
    info.SetAccountName(m_textCtrlName->GetValue());
    info.SetDefaultFolder(m_textCtrlHomeFolder->GetValue());
}

void AddSSHAcountDlg::OnTestConnection(wxCommandEvent& event)
{
    SSHAccountInfo account;
    GetAccountInfo(account);
    clSSH::Ptr_t ssh(new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()));

    try {
        wxString message;
        ssh->Connect();
        if(!ssh->AuthenticateServer(message)) {
            if(::wxMessageBox(message, "SSH", wxYES_NO | wxCENTER | wxICON_QUESTION, this) == wxYES) {
                ssh->AcceptServerAuthentication();
            }
        }

        // Try the login methods:
        ssh->Login();
        ::wxMessageBox(_("Successfully connected to host!"));

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SSH", wxICON_WARNING | wxOK, this);
    }
}
void AddSSHAcountDlg::OnTestConnectionUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlHost->IsEmpty() && !m_textCtrlPort->IsEmpty() && !m_textCtrlUsername->IsEmpty());
}
void AddSSHAcountDlg::OnHomeFolderUpdated(wxCommandEvent& event)
{
    wxString homeFolder = m_textCtrlHomeFolder->GetValue();
    if(!homeFolder.StartsWith("/")) {
        m_infobar->ShowMessage(_("Default folder must be set to full path (i.e. it should start with a '/')"),
                               wxICON_WARNING);
    }
}
#endif
