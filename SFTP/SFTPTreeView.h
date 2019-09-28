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
#include "cl_command_event.h"
#include "cl_sftp.h"
#include "ssh_account_info.h"
#include <vector>
#include "clSSHChannel.h"
#include <wx/timer.h>

class MyClientData;
class SFTP;

typedef std::vector<MyClientData*> MyClientDataVect_t;

class SFTPTreeView : public SFTPTreeViewBase
{
    clSFTP::Ptr_t m_sftp;
    clSSHChannel::Ptr_t m_channel;
    BitmapLoader* m_bmpLoader;
    SSHAccountInfo m_account;
    SFTP* m_plugin;
    wxString m_commandOutput;
    SFTPSessionInfoList m_sessions;
    wxTimer* m_timer = nullptr;

public:
    enum {
        ID_SFTP_BOOKMARK_FIRST = 13000,
        ID_SFTP_BOOKMARK_LAST = 13100,
        ID_SFTP_BOOKMARK_SETTINGS = 13101,
    };

public:
    SFTPTreeView(wxWindow* parent, SFTP* plugin);
    virtual ~SFTPTreeView();
    bool IsConnected() const { return m_sftp && m_sftp->IsConnected(); }
    const SSHAccountInfo& GetAccount() const { return m_account; }

protected:
    virtual void OnSftpSettings(wxCommandEvent& event);
    virtual void OnOpenTerminal(wxCommandEvent& event);
    virtual void OnOpenTerminalMenu(wxCommandEvent& event);
    virtual void OnOpenTerminalUI(wxUpdateUIEvent& event);
    virtual void OnConnection(wxCommandEvent& event);
    virtual void OnSelectionChanged(wxTreeEvent& event);
    virtual void OnChoiceAccount(wxCommandEvent& event);
    virtual void OnChoiceAccountUI(wxUpdateUIEvent& event);
    virtual void OnGotoLocation(wxCommandEvent& event);
    virtual void OnGotoLocationUI(wxUpdateUIEvent& event);
    virtual void OnAddBookmark(wxCommandEvent& event);
    virtual void OnAddBookmarkMenu(wxCommandEvent& event);
    virtual void OnAddBookmarkUI(wxUpdateUIEvent& event);
    virtual void OnContextMenu(wxContextMenuEvent& event);
    virtual void OnDisconnect(wxCommandEvent& event);
    virtual void OnDisconnectUI(wxUpdateUIEvent& event);
    virtual void OnConnect(wxCommandEvent& event);
    virtual void OnMenuNew(wxCommandEvent& event);
    virtual void OnMenuOpen(wxCommandEvent& event);
    virtual void OnMenuDelete(wxCommandEvent& event);
    virtual void OnMenuRename(wxCommandEvent& event);
    virtual void OnMenuNewFile(wxCommandEvent& event);
    virtual void OnMenuOpenWithDefaultApplication(wxCommandEvent& event);
    virtual void OnMenuOpenContainingFolder(wxCommandEvent& event);
    virtual void OnMenuRefreshFolder(wxCommandEvent& event);
    virtual void OnExecuteCommand(wxCommandEvent& event);
    void OnFileDropped(clCommandEvent& event);
    void OnEditorClosing(wxCommandEvent& evt);
    void OnRemoteFind(wxCommandEvent& event);
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
    bool DoExpandItem(const wxTreeItemId& item);
    void DoBuildTree(const wxString& initialFolder);
    void ManageBookmarks();
    /**
     * @brief open remote file path
     * @param path the remote file path
     */
    void DoOpenFile(const wxString& path);

    wxTreeItemId DoAddFolder(const wxTreeItemId& parent, const wxString& path);
    wxTreeItemId DoAddFile(const wxTreeItemId& parent, const wxString& path);

    MyClientData* GetItemData(const wxTreeItemId& item);
    MyClientDataVect_t GetSelectionsItemData();
    bool DoOpenFile(const wxTreeItemId& item);
    void DoDeleteColumn(int colIdx);
    bool GetAccountFromUser(SSHAccountInfo& account);
    SFTPSessionInfo& GetSession(bool createIfMissing);
    void DoLoadSession();

protected:
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnItemExpanding(wxTreeEvent& event);
    virtual void OnOpenAccountManager(wxCommandEvent& event);
};
#endif // SFTPTREEVIEW_H
