//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : SFTPBrowserDlg.h
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

#ifndef SFTPBROWSERDLG_H
#define SFTPBROWSERDLG_H

#if USE_SFTP

#include "sftp_ui.h"
#include "ssh/cl_sftp.h"

class SFTPBrowserEntryClientData;
class FloatingTextCtrl;

class WXDLLIMPEXP_SDK SFTPBrowserDlg : public SFTPBrowserBaseDlg
{
public:
    SFTPBrowserDlg(wxWindow* parent,
                   const wxString& title,
                   const wxString& filter,
                   size_t flags = clSFTP::SFTP_BROWSE_FILES | clSFTP::SFTP_BROWSE_FOLDERS,
                   const wxString& selectedAccount = wxEmptyString);
    ~SFTPBrowserDlg() override;

    void Initialize(const wxString& account, const wxString& path);
    wxString GetPath() const;
    wxArrayString GetPaths() const;
    wxString GetAccount() const;

    void OnInlineSearch();
    void OnInlineSearchEnter();
    static std::optional<wxArrayString> ShowFilePicker(const wxString& title,
                                                       const wxString& selectedAccount,
                                                       const wxString& initialPath = wxEmptyString,
                                                       wxWindow* parent = nullptr);

    static std::optional<wxArrayString> ShowDirPicker(const wxString& title,
                                                      const wxString& selectedAccount,
                                                      const wxString& initialPath = wxEmptyString,
                                                      wxWindow* parent = nullptr);

protected:
    void OnItemSelected(wxDataViewEvent& event) override;
    void OnOKUI(wxUpdateUIEvent& event) override;
    void OnTextEnter(wxCommandEvent& event) override;
    void OnItemActivated(wxDataViewEvent& event) override;
    void OnRefresh(wxCommandEvent& event) override;
    void OnRefreshUI(wxUpdateUIEvent& event) override;

    void OnSSHAccountManager(wxCommandEvent& event);
    void OnConnectedUI(wxUpdateUIEvent& event);
    void OnNewFolder(wxCommandEvent& event);
    void OnCdUp(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event) override;
    void DoCloseSession();
    void DoDisplayEntriesForPath(const wxString& path);
    void ClearView();
    SFTPBrowserEntryClientData* DoGetItemData(const wxDataViewItem& item) const;
    void DoBrowse();
    void DoSetLocationFocus();

private:
    clSFTP::Ptr_t m_sftp;
    wxString m_filter;
    size_t m_flags;
};
#endif // USE_SFTP

#endif // SFTPBROWSERDLG_H
