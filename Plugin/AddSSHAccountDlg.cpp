//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : AddSSHAccountDlg.cpp
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

#include "AddSSHAccountDlg.h"

#include <wx/tokenzr.h>
#if USE_SFTP
#include "cl_exception.h"
#include "environmentconfig.h"
#include "ssh/cl_ssh.h"
#include "windowattrmanager.h"

#include <wx/msgdlg.h>

AddSSHAccountDlg::AddSSHAccountDlg(wxWindow* parent)
    : AddSSHAccountDlgBase(parent)
{
    SetName("AddSSHAccountDlg");
    WindowAttrManager::Load(this);
}

AddSSHAccountDlg::AddSSHAccountDlg(wxWindow* parent, const SSHAccountInfo& account)
    : AddSSHAccountDlgBase(parent)
{
    m_textCtrlHost->ChangeValue(account.GetHost());
    m_textCtrlPassword->ChangeValue(account.GetPassword());
    m_textCtrlPort->ChangeValue(wxString() << account.GetPort());
    m_textCtrlUsername->ChangeValue(account.GetUsername());
    m_textCtrlName->ChangeValue(account.GetAccountName());
    m_textCtrlHomeFolder->ChangeValue(account.GetDefaultFolder());
    if (account.GetKeyFile().path.has_value()) {
        m_filePickerSSHKey->SetPath(account.GetKeyFile().path.value());
    }
    m_checkBoxPassphrase->SetValue(account.GetKeyFile().passphrase_required);
    if (!m_textCtrlPassword->IsEmpty()) {
        m_choicebookLogin->SetSelection(1);
    }
    SetName("AddSSHAccountDlg");
    WindowAttrManager::Load(this);
}

void AddSSHAccountDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxString homeFolder = m_textCtrlHomeFolder->GetValue();
    bool homeFolderValid = (homeFolder.IsEmpty() || (homeFolder.StartsWith("/")));

    event.Enable(!m_textCtrlHost->IsEmpty() && !m_textCtrlPort->IsEmpty() && !m_textCtrlUsername->IsEmpty() &&
                 homeFolderValid);
}

void AddSSHAccountDlg::GetAccountInfo(SSHAccountInfo& info)
{
    info.SetHost(m_textCtrlHost->GetValue());
    info.SetPassword(m_textCtrlPassword->GetValue());

    long port = 22;
    m_textCtrlPort->GetValue().ToCLong(&port);
    info.SetPortNumber(port);
    info.SetUsername(m_textCtrlUsername->GetValue());
    info.SetAccountName(m_textCtrlName->GetValue());
    info.SetDefaultFolder(m_textCtrlHomeFolder->GetValue());
    info.ClearKeyFile();

    // Login via SSH key
    SSHAccountInfo::KeyInfo key_info;
    key_info.passphrase_required = m_checkBoxPassphrase->IsChecked();

    wxString keyfile_path = m_filePickerSSHKey->GetPath();
    if (!keyfile_path.empty() && wxFileName::FileExists(keyfile_path)) {
        key_info.path = keyfile_path;
    }
    info.SetKeyFile(key_info);
}

void AddSSHAccountDlg::OnTestConnection(wxCommandEvent& event)
{
    SSHAccountInfo account;
    GetAccountInfo(account);
    clSSH::Ptr_t ssh(new clSSH(
        account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetKeyFile(), account.GetPort()));

    try {
        wxString message;
        EnvSetter env;
        ssh->Open();
        if (!ssh->AuthenticateServer(message)) {
            if (::wxMessageBox(message, "SSH", wxYES_NO | wxCENTER | wxICON_QUESTION, this) == wxYES) {
                ssh->AcceptServerAuthentication();
            }
        }

        // Try the login methods:
        ssh->Login();
        ::wxMessageBox(_("Successfully connected to host!"));

    } catch (const clException& e) {
        ::wxMessageBox(e.What(), "SSH", wxICON_WARNING | wxOK, this);
    }
}
void AddSSHAccountDlg::OnTestConnectionUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlHost->IsEmpty() && !m_textCtrlPort->IsEmpty() && !m_textCtrlUsername->IsEmpty());
}

void AddSSHAccountDlg::OnHomeFolderUpdated(wxCommandEvent& event)
{
    wxString homeFolder = m_textCtrlHomeFolder->GetValue();
    if (!homeFolder.StartsWith("/")) {
        m_infobar->ShowMessage(
            _("Default folder must be set to full path (i.e. it should start with a '/')"), wxICON_WARNING);
    }
}
#endif
