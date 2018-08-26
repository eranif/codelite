//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SSHAccountManagerDlg.cpp
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

#include "SSHAccountManagerDlg.h"
#if USE_SFTP

#include "AddSSHAcountDlg.h"
#include "sftp_settings.h"
#include "ssh_account_info.h"
#include "windowattrmanager.h"
#include <wx/msgdlg.h>

SSHAccountManagerDlg::SSHAccountManagerDlg(wxWindow* parent)
    : SSHAccountManagerDlgBase(parent)
{
    SFTPSettings settings;
    settings.Load();

    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    SSHAccountInfo::Vect_t::const_iterator iter = accounts.begin();
    for(; iter != accounts.end(); ++iter) {
        DoAddAccount(*iter);
    }
    m_dvListCtrlAccounts->GetColumn(0)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    m_dvListCtrlAccounts->GetColumn(1)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    m_dvListCtrlAccounts->GetColumn(2)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    SetName("SSHAccountManagerDlg");
    WindowAttrManager::Load(this);
}

SSHAccountManagerDlg::~SSHAccountManagerDlg()
{
    for(int i = 0; i < m_dvListCtrlAccounts->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrlAccounts->RowToItem(i);
        SSHAccountInfo* pAccount = (SSHAccountInfo*)(m_dvListCtrlAccounts->GetItemData(item));
        delete pAccount;
        m_dvListCtrlAccounts->SetItemData(item, (wxUIntPtr)NULL);
    }
    m_dvListCtrlAccounts->DeleteAllItems();
}

void SSHAccountManagerDlg::OnAddAccount(wxCommandEvent& event)
{
    AddSSHAcountDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        SSHAccountInfo account;
        dlg.GetAccountInfo(account);
        DoAddAccount(account);
    }
}

void SSHAccountManagerDlg::OnDeleteAccount(wxCommandEvent& event)
{
    wxDataViewItemArray sels;
    m_dvListCtrlAccounts->GetSelections(sels);

    if(::wxMessageBox(_("Are you sure you want to delete the selected accounts?"), "SFTP",
                      wxYES_NO | wxCENTER | wxCANCEL | wxICON_QUESTION | wxNO_DEFAULT) != wxYES) {
        return;
    }

    for(size_t i = 0; i < sels.GetCount(); ++i) {
        wxDataViewItem item = sels.Item(i);
        m_dvListCtrlAccounts->DeleteItem(m_dvListCtrlAccounts->ItemToRow(item));
    }

    m_dvListCtrlAccounts->Refresh();
}

void SSHAccountManagerDlg::OnEditAccount(wxCommandEvent& event)
{
    wxDataViewItemArray sels;
    m_dvListCtrlAccounts->GetSelections(sels);
    if(sels.GetCount() == 1) { DoEditAccount(sels.Item(0)); }
}

void SSHAccountManagerDlg::DoAddAccount(const SSHAccountInfo& account)
{
    wxVector<wxVariant> cols;
    cols.push_back(account.GetAccountName());
    cols.push_back(account.GetHost());
    cols.push_back(account.GetUsername());
    m_dvListCtrlAccounts->AppendItem(cols, (wxUIntPtr)(new SSHAccountInfo(account)));
}

SSHAccountInfo::Vect_t SSHAccountManagerDlg::GetAccounts() const
{
    SSHAccountInfo::Vect_t accounts;
    for(int i = 0; i < m_dvListCtrlAccounts->GetItemCount(); ++i) {
        accounts.push_back(*(SSHAccountInfo*)(m_dvListCtrlAccounts->GetItemData(m_dvListCtrlAccounts->RowToItem(i))));
    }
    return accounts;
}

void SSHAccountManagerDlg::OnDeleteAccountUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlAccounts->GetSelectedItemsCount());
}

void SSHAccountManagerDlg::OnEditAccountUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlAccounts->GetSelectedItemsCount());
}

void SSHAccountManagerDlg::OnItemActivated(wxDataViewEvent& event) { DoEditAccount(event.GetItem()); }

void SSHAccountManagerDlg::DoEditAccount(const wxDataViewItem& item)
{
    SSHAccountInfo* account = (SSHAccountInfo*)(m_dvListCtrlAccounts->GetItemData(item));
    if(account) {
        AddSSHAcountDlg dlg(this, *account);
        if(dlg.ShowModal() == wxID_OK) {
            // update the user info
            dlg.GetAccountInfo(*account);

            // update the UI
            m_dvListCtrlAccounts->GetStore()->SetValue(account->GetAccountName(), item, 0);
            m_dvListCtrlAccounts->GetStore()->SetValue(account->GetHost(), item, 1);
            m_dvListCtrlAccounts->GetStore()->SetValue(account->GetUsername(), item, 2);
            m_dvListCtrlAccounts->Refresh();
        }
    }
}
#endif // USE_SFTP
