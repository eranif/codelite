//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SFTPBrowserDlg.cpp
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

#if USE_SFTP

#include "SFTPBrowserDlg.h"
#include "sftp_settings.h"
#include "ssh_account_info.h"
#include <wx/msgdlg.h>
#include "windowattrmanager.h"
#include "fileextmanager.h"
#include "my_sftp_tree_model.h"
#include "SSHAccountManagerDlg.h"

// ================================================================================
// ================================================================================

class SFTPBrowserEntryClientData : public wxClientData
{
    SFTPAttribute::Ptr_t m_attribute;
    wxString m_fullpath;

public:
    SFTPBrowserEntryClientData(SFTPAttribute::Ptr_t attr, const wxString& fullpath)
        : m_attribute(attr)
        , m_fullpath(fullpath)
    {
        wxFileName fn;
        if(m_attribute->IsFolder()) {
            fn = wxFileName(fullpath, "", wxPATH_UNIX);
            fn.Normalize();
            m_fullpath = fn.GetPath(false, wxPATH_UNIX);
        } else {
            fn = wxFileName(fullpath, wxPATH_UNIX);
            fn.Normalize();
            m_fullpath = fn.GetFullPath(wxPATH_UNIX);
        }
    }

    virtual ~SFTPBrowserEntryClientData() {}

    const wxString& GetDisplayName() const { return GetAttribute()->GetName(); }
    void SetAttribute(const SFTPAttribute::Ptr_t& attribute) { this->m_attribute = attribute; }
    void SetFullpath(const wxString& fullpath) { this->m_fullpath = fullpath; }
    const SFTPAttribute::Ptr_t& GetAttribute() const { return m_attribute; }
    const wxString& GetFullpath() const { return m_fullpath; }
};

// ================================================================================
// ================================================================================

SFTPBrowserDlg::SFTPBrowserDlg(wxWindow* parent, const wxString& title, const wxString& filter, size_t flags)
    : SFTPBrowserBaseDlg(parent)
    , m_sftp(NULL)
    , m_filter(filter)
    , m_flags(flags)
{
    m_dataviewModel = new MySFTPTreeModel();
    m_dataview->AssociateModel(m_dataviewModel.get());

    SetLabel(title);

    BitmapLoader bl;
    m_bitmaps = bl.MakeStandardMimeMap();

    SFTPSettings settings;
    settings.Load();

    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    SSHAccountInfo::Vect_t::const_iterator iter = accounts.begin();
    for(; iter != accounts.end(); ++iter) {
        m_choiceAccount->Append(iter->GetAccountName());
    }

    if(!m_choiceAccount->IsEmpty()) {
        m_choiceAccount->SetSelection(0);
    }

    SetName("SFTPBrowserDlg");
    WindowAttrManager::Load(this);
}

SFTPBrowserDlg::~SFTPBrowserDlg() {}

void SFTPBrowserDlg::OnRefresh(wxCommandEvent& event)
{
    DoCloseSession();
    wxString accountName = m_choiceAccount->GetStringSelection();

    SFTPSettings settings;
    settings.Load();

    SSHAccountInfo account;
    if(!settings.GetAccount(accountName, account)) {
        ::wxMessageBox(
            wxString() << _("Could not find account: ") << accountName, "codelite", wxICON_ERROR | wxOK, this);
        return;
    }

    clSSH::Ptr_t ssh(new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()));
    try {
        wxString message;
        ssh->Connect();
        if(!ssh->AuthenticateServer(message)) {
            if(::wxMessageBox(message, "SSH", wxYES_NO | wxCENTER | wxICON_QUESTION, this) == wxYES) {
                ssh->AcceptServerAuthentication();
            }
        }

        ssh->Login();
        m_sftp.reset(new clSFTP(ssh));
        m_sftp->Initialize();

        DoDisplayEntriesForPath();

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "codelite", wxICON_ERROR | wxOK, this);
        DoCloseSession();
    }
}

void SFTPBrowserDlg::OnRefreshUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlRemoteFolder->IsEmpty()); }

void SFTPBrowserDlg::DoDisplayEntriesForPath(const wxString& path)
{
    try {
        wxString folder;
        SFTPAttribute::List_t attributes;
        if(path.IsEmpty()) {
            folder = m_textCtrlRemoteFolder->GetValue();
            attributes = m_sftp->List(folder, m_flags, m_filter);

        } else if(path == "..") {
            // cd up
            attributes = m_sftp->CdUp(m_flags, m_filter);
            m_textCtrlRemoteFolder->ChangeValue(m_sftp->GetCurrentFolder());
            folder = m_sftp->GetCurrentFolder();

        } else {
            folder = path;
            attributes = m_sftp->List(folder, m_flags, m_filter);
        }

        SFTPAttribute::List_t::iterator iter = attributes.begin();
        for(; iter != attributes.end(); ++iter) {

            // Set the columns Name (icontext) | Type (text) | Size (text)
            wxVector<wxVariant> cols;

            // determine the bitmap type
            wxBitmap bmp = m_bitmaps[FileExtManager::TypeText];
            wxString fullname;
            fullname << folder << "/" << (*iter)->GetName();

            if((*iter)->IsFolder()) {
                bmp = m_bitmaps[FileExtManager::TypeFolder];
            } else {
                wxFileName fn(fullname);
                FileExtManager::FileType type = FileExtManager::GetType(fn.GetFullName());
                if(m_bitmaps.count(type)) {
                    bmp = m_bitmaps[type];
                }
            }

            cols.push_back(SFTPTreeModel::CreateIconTextVariant((*iter)->GetName(), bmp));
            cols.push_back((*iter)->GetTypeAsString());
            cols.push_back(wxString() << (*iter)->GetSize());

            SFTPBrowserEntryClientData* cd = new SFTPBrowserEntryClientData((*iter), fullname);
            m_dataviewModel->AppendItem(wxDataViewItem(0), cols, cd);
        }
        m_dataview->Refresh();
        m_dataview->SetFocus();

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK);
        DoCloseSession();
    }
}

void SFTPBrowserDlg::DoCloseSession()
{
    m_sftp.reset(NULL);
    m_dataviewModel->Clear();
}

void SFTPBrowserDlg::OnItemActivated(wxDataViewEvent& event)
{
    if(!m_sftp) {
        DoCloseSession();
        return;
    }

    SFTPBrowserEntryClientData* cd =
        dynamic_cast<SFTPBrowserEntryClientData*>(m_dataviewModel->GetClientObject(event.GetItem()));
    if(cd && cd->GetAttribute()->IsFolder()) {
        m_textCtrlRemoteFolder->ChangeValue(cd->GetFullpath());
        m_dataviewModel->Clear();
        DoDisplayEntriesForPath();
    }
}

void SFTPBrowserDlg::OnTextEnter(wxCommandEvent& event)
{
    if(!m_sftp) {
        OnRefresh(event);
    } else {
        m_dataviewModel->Clear();
        DoDisplayEntriesForPath();
    }
}
void SFTPBrowserDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxString selection = m_textCtrlRemoteFolder->GetValue();
    if(m_filter.IsEmpty()) { // folder / files - and there is a selection
        event.Enable(!selection.IsEmpty());

    } else if(!(m_flags & clSFTP::SFTP_BROWSE_FILES)) { // folders only
        event.Enable(!selection.IsEmpty());

    } else if(!selection.IsEmpty() && (m_flags & clSFTP::SFTP_BROWSE_FILES) && ::wxMatchWild(m_filter, selection)) {
        event.Enable(true);

    } else {
        event.Enable(false);
    }
}

SFTPBrowserEntryClientData* SFTPBrowserDlg::DoGetItemData(const wxDataViewItem& item) const
{
    if(!item.IsOk()) {
        return NULL;
    }

    SFTPBrowserEntryClientData* cd = dynamic_cast<SFTPBrowserEntryClientData*>(m_dataviewModel->GetClientObject(item));
    return cd;
}

wxString SFTPBrowserDlg::GetPath() const { return m_textCtrlRemoteFolder->GetValue(); }

void SFTPBrowserDlg::OnItemSelected(wxDataViewEvent& event)
{
    SFTPBrowserEntryClientData* cd = DoGetItemData(m_dataview->GetSelection());
    if(cd) {
        m_textCtrlRemoteFolder->ChangeValue(cd->GetFullpath());
    }
}

wxString SFTPBrowserDlg::GetAccount() const { return m_choiceAccount->GetStringSelection(); }

void SFTPBrowserDlg::Initialize(const wxString& account, const wxString& path)
{
    m_textCtrlRemoteFolder->ChangeValue(path);
    int where = m_choiceAccount->FindString(account);
    if(where != wxNOT_FOUND) {
        m_choiceAccount->SetSelection(where);
    }
}

void SFTPBrowserDlg::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    wxChar ch = (wxChar)event.GetKeyCode();

    if(::wxIsprint(ch)) {
        if(!m_textCtrlInlineSearch->IsShown()) {
            m_textCtrlInlineSearch->SetFocus();
            m_textCtrlInlineSearch->Clear();
#ifdef __WXMSW__
            m_textCtrlInlineSearch->ChangeValue(wxString() << (wxChar)event.GetKeyCode());
#endif
            m_textCtrlInlineSearch->SetInsertionPoint(m_textCtrlInlineSearch->GetLastPosition());
            m_textCtrlInlineSearch->Show();
            GetSizer()->Layout();
        }
    }
}

void SFTPBrowserDlg::OnInlineSearch()
{
    wxString text = m_textCtrlInlineSearch->GetValue();
    wxDataViewItemArray children;
    m_dataviewModel->GetChildren(wxDataViewItem(0), children);

    for(size_t i = 0; i < children.GetCount(); ++i) {
        SFTPBrowserEntryClientData* cd = DoGetItemData(children.Item(i));
        if(cd && cd->GetDisplayName().StartsWith(text)) {
            m_dataview->Select(children.Item(i));
            m_dataview->EnsureVisible(children.Item(i));
            break;
        }
    }
}

void SFTPBrowserDlg::OnInlineSearchEnter()
{
    wxDataViewItem item = m_dataview->GetSelection();
    if(!item.IsOk()) {
        return;
    }

    SFTPBrowserEntryClientData* cd = dynamic_cast<SFTPBrowserEntryClientData*>(m_dataviewModel->GetClientObject(item));
    if(cd && cd->GetAttribute()->IsFolder()) {
        m_textCtrlRemoteFolder->ChangeValue(cd->GetFullpath());
        m_dataviewModel->Clear();
        DoDisplayEntriesForPath();
        m_dataview->SetFocus();
    }
}
void SFTPBrowserDlg::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CallAfter(&SFTPBrowserDlg::OnInlineSearchEnter);
}

void SFTPBrowserDlg::OnFocusLost(wxFocusEvent& event)
{
    event.Skip();
    m_textCtrlInlineSearch->Hide();
    GetSizer()->Layout();
}

void SFTPBrowserDlg::OnTextUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    CallAfter(&SFTPBrowserDlg::OnInlineSearch);
}

void SFTPBrowserDlg::OnCdUp(wxCommandEvent& event)
{
    m_dataviewModel->Clear();
    DoDisplayEntriesForPath("..");
}

void SFTPBrowserDlg::OnCdUpUI(wxUpdateUIEvent& event) { event.Enable(m_sftp); }

void SFTPBrowserDlg::OnSSHAccountManager(wxCommandEvent& event)
{
    SSHAccountManagerDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {

        SFTPSettings settings;
        settings.Load().SetAccounts(dlg.GetAccounts());
        settings.Save();

        // Update the selections at the top
        wxString curselection = m_choiceAccount->GetStringSelection();

        m_choiceAccount->Clear();
        const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
        if(accounts.empty()) {
            DoCloseSession();
            return;

        } else {
            SSHAccountInfo::Vect_t::const_iterator iter = accounts.begin();
            for(; iter != accounts.end(); ++iter) {
                m_choiceAccount->Append(iter->GetAccountName());
            }

            int where = m_choiceAccount->FindString(curselection);
            if(where == wxNOT_FOUND) {
                // Our previous session is no longer available, close the session
                DoCloseSession();
                where = 0;
            }

            m_choiceAccount->SetSelection(where);
        }
    }
}
#endif // USE_SFTP
