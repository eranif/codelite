//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : SFTPTreeView.h
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

#ifndef SFTPTREEVIEW_H
#define SFTPTREEVIEW_H

#include "SFTPSessionInfo.h"
#include "UI.h"
#include "bitmap_loader.h"
#include "clRemoteDirCtrl.hpp"
#include "clSSHChannel.hpp"
#include "cl_command_event.h"
#include "cl_sftp.h"
#include "sftp_item_comparator.hpp"
#include "ssh_account_info.h"
#include <vector>
#include <wx/timer.h>

class SFTP;
class SFTPTreeView : public SFTPTreeViewBase
{
    SSHAccountInfo m_account;
    SFTP* m_plugin;
    wxString m_commandOutput;
    SFTPSessionInfoList m_sessions;
    wxTimer* m_timer = nullptr;
    clRemoteDirCtrl* m_view = nullptr;

public:
    enum {
        ID_SFTP_BOOKMARK_FIRST = 13000,
        ID_SFTP_BOOKMARK_LAST = 13100,
        ID_SFTP_BOOKMARK_SETTINGS = 13101,
    };

public:
    SFTPTreeView(wxWindow* parent, SFTP* plugin);
    virtual ~SFTPTreeView();
    const SSHAccountInfo& GetAccount() const { return m_account; }

protected:
    void OnSftpSettings(wxCommandEvent& event);
    void OnOpenTerminal(wxCommandEvent& event);
    void OnOpenTerminalMenu(wxCommandEvent& event);
    void OnOpenTerminalUI(wxUpdateUIEvent& event);
    void OnConnection(wxCommandEvent& event);
    void OnConnectionUI(wxUpdateUIEvent& event);
    void OnChoiceAccount(wxCommandEvent& event);
    void OnChoiceAccountUI(wxUpdateUIEvent& event);
    void OnGotoLocation(wxCommandEvent& event);
    void OnGotoLocationUI(wxUpdateUIEvent& event);
    void OnAddBookmark(wxCommandEvent& event);
    void OnAddBookmarkMenu(wxCommandEvent& event);
    void OnAddBookmarkUI(wxUpdateUIEvent& event);
    void OnDisconnect(wxCommandEvent& event);
    void OnDisconnectUI(wxUpdateUIEvent& event);
    void OnConnect(wxCommandEvent& event);
    void OnKeepAliveTimer(wxTimerEvent& event);

    // Edit events
    void OnCopy(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);

    // Find events
    void OnFindOutput(clCommandEvent& event);
    void OnFindFinished(clCommandEvent& event);
    void OnFindError(clCommandEvent& event);

    void DoCloseSession();
    void DoOpenSession();
    void DoBuildTree(const wxString& initialFolder);
    void ManageBookmarks();
    void DoChangeLocation(const wxString& path);

    bool GetAccountFromUser(SSHAccountInfo& account);
    //    SFTPSessionInfo& GetSession(bool createIfMissing);
    //    void DoLoadSession();

protected:
    virtual void OnOpenAccountManager(wxCommandEvent& event);
};
#endif // SFTPTREEVIEW_H
