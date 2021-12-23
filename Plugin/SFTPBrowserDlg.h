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

#include "bitmap_loader.h"
#include "cl_sftp.h"
#include "sftp_ui.h"

class SFTPBrowserEntryClientData;
class FloatingTextCtrl;

class WXDLLIMPEXP_SDK SFTPBrowserDlg : public SFTPBrowserBaseDlg
{
    clSFTP::Ptr_t m_sftp;
    wxString m_filter;
    size_t m_flags;

protected:
    void OnSSHAccountManager(wxCommandEvent& event);
    void OnConnectedUI(wxUpdateUIEvent& event);
    void OnNewFolder(wxCommandEvent& event);
    void OnCdUp(wxCommandEvent& event);
    void OnEnter(wxCommandEvent& event);
    void OnFocusLost(wxFocusEvent& event);
    void OnTextUpdated(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnItemSelected(wxDataViewEvent& event);
    void OnOKUI(wxUpdateUIEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnItemActivated(wxDataViewEvent& event);
    void DoCloseSession();
    void DoDisplayEntriesForPath(const wxString& path = "");
    void ClearView();
    SFTPBrowserEntryClientData* DoGetItemData(const wxDataViewItem& item) const;
    void DoBrowse();
    void DoSetLocationFocus();

public:
    SFTPBrowserDlg(wxWindow* parent, const wxString& title, const wxString& filter,
                   size_t flags = clSFTP::SFTP_BROWSE_FILES | clSFTP::SFTP_BROWSE_FOLDERS,
                   const wxString& selectedAccount = wxEmptyString);
    virtual ~SFTPBrowserDlg();

    void Initialize(const wxString& account, const wxString& path);
    wxString GetPath() const;
    wxString GetAccount() const;

    void OnInlineSearch();
    void OnInlineSearchEnter();

protected:
    virtual void OnRefresh(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
};
#endif // USE_SFTP

#endif // SFTPBROWSERDLG_H
