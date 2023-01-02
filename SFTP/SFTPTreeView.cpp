//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SFTPTreeView.cpp
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

#include "SFTPTreeView.h"

#include "SFTPBookmark.h"
#include "SFTPGrep.h"
#include "SFTPManageBookmarkDlg.h"
#include "SFTPQuickConnectDlg.h"
#include "SFTPSettingsDialog.h"
#include "SFTPStatusPage.h"
#include "SFTPUploadDialog.h"
#include "SSHAccountManagerDlg.h"
#include "bitmap_loader.h"
#include "clFileOrFolderDropTarget.h"
#include "clToolBarButtonBase.h"
#include "cl_config.h"
#include "console_frame.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "macros.h"
#include "sftp.h"
#include "sftp_item_comparator.hpp"
#include "sftp_settings.h"
#include "sftp_worker_thread.h"
#include "ssh_account_info.h"

#include <algorithm>
#include <vector>
#include <wx/busyinfo.h>
#include <wx/fdrepdlg.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/textdlg.h>
#include <wx/utils.h>

SFTPTreeView::SFTPTreeView(wxWindow* parent, SFTP* plugin)
    : SFTPTreeViewBase(parent)
    , m_plugin(plugin)
{
    m_view = new clRemoteDirCtrl(this);
    GetSizer()->Add(m_view, 1, wxEXPAND);

    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &SFTPTreeView::OnKeepAliveTimer, this, m_timer->GetId());
    m_timer->Start(30000); // every 30 seconds send a hearbeat

    SFTPSettings settings;
    settings.Load();

    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnCopy, this, wxID_COPY);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnCut, this, wxID_CUT);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnPaste, this, wxID_PASTE);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnSelectAll, this, wxID_SELECTALL);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnUndo, this, wxID_UNDO);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnRedo, this, wxID_REDO);

    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(XRCID("ID_OPEN_ACCOUNT_MANAGER"), _("Open account manager..."), images->Add("folder-users"));
    m_toolbar->AddTool(XRCID("ID_SFTP_CONNECT"), _("Disconnected. Click to connect"), images->Add("disconnected"),
                       wxEmptyString, wxITEM_CHECK);
#if !wxUSE_NATIVE_TOOLBAR
    m_toolbar->AddTool(XRCID("ID_ADD_BOOKMARK"), _("Add Bookmark"), images->Add("bookmark"), "", wxITEM_DROPDOWN);
#endif
    m_toolbar->AddTool(XRCID("ID_SSH_OPEN_TERMINAL"), _("Open Terminal"), images->Add("console"), "", wxITEM_DROPDOWN);
    m_toolbar->Realize();

    // Bind the toolbar events
    m_toolbar->Bind(wxEVT_TOOL, &SFTPTreeView::OnOpenAccountManager, this, XRCID("ID_OPEN_ACCOUNT_MANAGER"));
    m_toolbar->Bind(wxEVT_TOOL, &SFTPTreeView::OnConnection, this, XRCID("ID_SFTP_CONNECT"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &SFTPTreeView::OnConnectionUI, this, XRCID("ID_SFTP_CONNECT"));
#if !wxUSE_NATIVE_TOOLBAR
    m_toolbar->Bind(wxEVT_TOOL, &SFTPTreeView::OnAddBookmark, this, XRCID("ID_ADD_BOOKMARK"));
#endif
    m_toolbar->Bind(wxEVT_UPDATE_UI, &SFTPTreeView::OnAddBookmarkUI, this, XRCID("ID_ADD_BOOKMARK"));
    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &SFTPTreeView::OnAddBookmarkMenu, this, XRCID("ID_ADD_BOOKMARK"));
    m_toolbar->Bind(wxEVT_TOOL, &SFTPTreeView::OnOpenTerminal, this, XRCID("ID_SSH_OPEN_TERMINAL"));
    m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &SFTPTreeView::OnOpenTerminalMenu, this, XRCID("ID_SSH_OPEN_TERMINAL"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &SFTPTreeView::OnOpenTerminalUI, this, XRCID("ID_SSH_OPEN_TERMINAL"));
}

SFTPTreeView::~SFTPTreeView()
{
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnCopy, this, wxID_COPY);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnCut, this, wxID_CUT);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnPaste, this, wxID_PASTE);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnSelectAll, this, wxID_SELECTALL);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnUndo, this, wxID_UNDO);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnRedo, this, wxID_REDO);

    m_timer->Stop();
    Unbind(wxEVT_TIMER, &SFTPTreeView::OnKeepAliveTimer, this, m_timer->GetId());
    wxDELETE(m_timer);
}

void SFTPTreeView::OnDisconnect(wxCommandEvent& event) { DoCloseSession(); }
void SFTPTreeView::OnConnect(wxCommandEvent& event) { DoOpenSession(); }

void SFTPTreeView::DoBuildTree(const wxString& initialFolder)
{
    m_view->Close(false);
    m_view->Open(initialFolder, m_account);
}

void SFTPTreeView::OnOpenAccountManager(wxCommandEvent& event)
{
    SSHAccountManagerDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

void SFTPTreeView::DoCloseSession()
{
    // Clear the 'search' view
    m_plugin->GetOutputPane()->ClearSearchOutput();
    m_view->Close(true);
}

void SFTPTreeView::OnDisconnectUI(wxUpdateUIEvent& event) { event.Enable(m_view->IsConnected()); }

void SFTPTreeView::OnAddBookmarkMenu(wxCommandEvent& event)
{
    wxUnusedVar(event);
#if !wxUSE_NATIVE_TOOLBAR
    // Show the menu
    const wxArrayString& bookmarks = m_account.GetBookmarks();
    wxMenu menu;
    for(size_t i = 0; i < bookmarks.GetCount(); ++i) {
        if(!bookmarks[i].empty()) {
            menu.Append(ID_SFTP_BOOKMARK_FIRST + i, bookmarks.Item(i));
        }
    }
    menu.AppendSeparator();
    menu.Append(ID_SFTP_BOOKMARK_SETTINGS, _("Manage bookmarks..."));
    int sel = m_toolbar->GetMenuSelectionFromUser(XRCID("ID_ADD_BOOKMARK"), &menu);
    if((sel >= ID_SFTP_BOOKMARK_FIRST) && (sel <= ID_SFTP_BOOKMARK_LAST)) {
        // A bookmark was selected
        CallAfter(&SFTPTreeView::DoChangeLocation, bookmarks.Item(sel - ID_SFTP_BOOKMARK_FIRST));

    } else if(sel == ID_SFTP_BOOKMARK_SETTINGS) {
        // Bookmark settings
        CallAfter(&SFTPTreeView::ManageBookmarks);
    }
#endif
}

void SFTPTreeView::OnAddBookmark(wxCommandEvent& event)
{
    try {
        // sanity
        if(!m_view->IsConnected())
            return;

        wxString path = m_view->GetSelectedFolder();
        m_account.AddBookmark(path);
        SFTPSettings settings;
        settings.Load();
        settings.UpdateAccount(m_account);
        settings.Save();

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK | wxCENTER);
    }
}

void SFTPTreeView::OnAddBookmarkUI(wxUpdateUIEvent& event) { event.Enable(m_view->IsConnected()); }
void SFTPTreeView::OnGotoLocation(wxCommandEvent& event)
{
    // check that the target folder exists
    m_view->SetNewRoot(m_textCtrlQuickJump->GetValue());
}

void SFTPTreeView::OnGotoLocationUI(wxUpdateUIEvent& event) { event.Enable(m_view->IsConnected()); }

void SFTPTreeView::ManageBookmarks()
{
    SFTPManageBookmarkDlg dlg(NULL, m_account.GetBookmarks());
    if(dlg.ShowModal() == wxID_OK) {
        m_account.SetBookmarks(dlg.GetBookmarks());
        SFTPSettings settings;
        settings.Load();
        settings.UpdateAccount(m_account);
        settings.Save();
    }
}

void SFTPTreeView::OnChoiceAccount(wxCommandEvent& event) { event.Skip(); }
void SFTPTreeView::OnChoiceAccountUI(wxUpdateUIEvent& event) { event.Enable(!m_view->IsConnected()); }

void SFTPTreeView::OnConnectionUI(wxUpdateUIEvent& event) { event.Check(m_view->IsConnected()); }

void SFTPTreeView::OnConnection(wxCommandEvent& event)
{
    if(m_view->IsConnected()) {
        // Disconnect
        DoCloseSession();
    } else {
        DoOpenSession();
    }
}

void SFTPTreeView::DoOpenSession()
{
    DoCloseSession();
    if(!GetAccountFromUser(m_account)) {
        return;
    }
    DoBuildTree(m_account.GetDefaultFolder().IsEmpty() ? "/" : m_account.GetDefaultFolder());
    // CallAfter(&SFTPTreeView::DoLoadSession);
}

void SFTPTreeView::OnCopy(wxCommandEvent& event)
{
    event.Skip();
    if(m_textCtrlQuickJump->HasFocus()) {
        event.Skip(false);
    }
}

void SFTPTreeView::OnPaste(wxCommandEvent& event)
{
    event.Skip();
    if(m_textCtrlQuickJump->HasFocus()) {
        event.Skip(false);
        m_textCtrlQuickJump->Paste();
    }
}

void SFTPTreeView::OnRedo(wxCommandEvent& event)
{
    event.Skip();
    if(m_textCtrlQuickJump->HasFocus()) {
        event.Skip(false);
        m_textCtrlQuickJump->Redo();
    }
}

void SFTPTreeView::OnSelectAll(wxCommandEvent& event)
{
    event.Skip();
    if(m_textCtrlQuickJump->HasFocus()) {
        event.Skip(false);
        m_textCtrlQuickJump->SelectAll();
    }
}

void SFTPTreeView::OnUndo(wxCommandEvent& event)
{
    event.Skip();
    if(m_textCtrlQuickJump->HasFocus()) {
        event.Skip(false);
        m_textCtrlQuickJump->Undo();
    }
}

void SFTPTreeView::OnCut(wxCommandEvent& event)
{
    event.Skip();
    if(m_textCtrlQuickJump->HasFocus()) {
        event.Skip(false);
        m_textCtrlQuickJump->Cut();
    }
}

void SFTPTreeView::OnOpenTerminalMenu(wxCommandEvent& event)
{
    wxMenu menu;
    menu.Append(wxID_PREFERENCES);
    menu.Bind(wxEVT_MENU, &SFTPTreeView::OnSftpSettings, this, wxID_PREFERENCES);
    m_toolbar->ShowMenuForButton(XRCID("ID_SSH_OPEN_TERMINAL"), &menu);
}

void SFTPTreeView::OnOpenTerminal(wxCommandEvent& event)
{
    // Open terminal to the selected account
    SSHAccountInfo account;
    if(!GetAccountFromUser(account)) {
        return;
    }

    wxString connectString;
    connectString << account.GetUsername() << "@" << account.GetHost();

    SFTPSettings settings;
    settings.Load();

    const wxString& sshClient = settings.GetSshClient();
    FileUtils::OpenSSHTerminal(sshClient, connectString, account.GetPassword(), account.GetPort());
}

void SFTPTreeView::OnOpenTerminalUI(wxUpdateUIEvent& event) { event.Enable(true); }

void SFTPTreeView::OnSftpSettings(wxCommandEvent& event)
{
    // Show the SFTP settings dialog
    SFTPSettingsDialog dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

bool SFTPTreeView::GetAccountFromUser(SSHAccountInfo& account)
{
    SFTPQuickConnectDlg connectDialog(EventNotifier::Get()->TopFrame());
    if(connectDialog.ShowModal() != wxID_OK) {
        return false;
    }

    // Get the selected account
    account = connectDialog.GetSelectedAccount();
    return true;
}

// SFTPSessionInfo& SFTPTreeView::GetSession(bool createIfMissing)
//{
//    SFTPSessionInfo& sess = m_sessions.GetSession(m_account.GetAccountName());
//    if(!sess.IsOk() && createIfMissing) {
//        SFTPSessionInfo s;
//        s.SetAccount(m_account.GetAccountName());
//        m_sessions.SetSession(s).Save();
//        return m_sessions.GetSession(m_account.GetAccountName());
//    }
//    return sess;
//}

// void SFTPTreeView::DoLoadSession()
//{
//    // Now that we have successfully opened the connection, try to load the last saved session for this account
//    const SFTPSessionInfo& sess = GetSession(true);
//    if(sess.IsOk()) {
//        wxString msg;
//        msg << _("Would you like to load the saved session for this account?");
//        wxStandardID ans = ::PromptForYesNoCancelDialogWithCheckbox(msg, "sftp-load-session-dlg");
//        if(ans == wxID_YES) {
//            // we have a session for this account, load it
//            // Load the files
//            const std::vector<wxString>& files = sess.GetFiles();
//            std::for_each(files.begin(), files.end(), [&](const wxString& path) { DoOpenFile(path); });
//
//            const wxString& rootFolder = sess.GetRootFolder();
//            if(!rootFolder.IsEmpty()) {
//                m_textCtrlQuickJump->ChangeValue(rootFolder);
//                CallAfter(&SFTPTreeView::DoBuildTree, rootFolder);
//            }
//        }
//    }
//}

// void SFTPTreeView::OnRemoteFind(wxCommandEvent& event)
//{
//    if(!m_sftp || !m_sftp->GetSsh()) {
//        return;
//    }
//
//    wxArrayTreeItemIds items;
//    m_treeCtrl->GetSelections(items);
//    if(items.GetCount() != 1) {
//        return;
//    }
//    clRemoteDirCtrlItemData* cd = GetItemData(items.Item(0));
//    if(!cd || !cd->IsFolder()) {
//        return;
//    }
//
//    wxString remoteFolder = cd->GetFullPath();
//
//    SFTPGrep grep(EventNotifier::Get()->TopFrame());
//    if(grep.ShowModal() != wxID_OK) {
//        return;
//    }
//
//    try {
//        if(m_channel && m_channel->IsOpen()) {
//            m_channel->Close();
//        }
//        m_channel.reset(new clSSHChannel(m_sftp->GetSsh(), clSSHChannel::kRemoteCommand, m_plugin->GetOutputPane()));
//        m_channel->Open();
//
//        // Prepare the UI for new search
//        m_plugin->GetOutputPane()->ClearSearchOutput();
//        m_plugin->GetOutputPane()->ShowSearchTab();
//        clGetManager()->ShowOutputPane(_("SFTP Log"));
//
//        // Run the search
//        GrepData gd = grep.GetData();
//        wxString command = gd.GetGrepCommand(remoteFolder);
//        m_plugin->GetOutputPane()->AddSearchText(wxString() << "Running command: " << command);
//        m_channel->Execute(command);
//
//    } catch(clException& e) {
//        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK | wxCENTER);
//    }
//}

void SFTPTreeView::OnKeepAliveTimer(wxTimerEvent& event)
{
    //    wxUnusedVar(event);
    //    if(IsConnected()) {
    //        // Perform a minimal operation to keep our session alive
    //        try {
    //            m_sftp->Stat(".");
    //            clDEBUG1() << "SFTP: Heartbeat successfully sent!";
    //        } catch(clException& e) {
    //            clGetManager()->SetStatusMessage(_("SFTP: session closed by the server"));
    //        }
    //    }
    event.Skip();
}

void SFTPTreeView::OnFindOutput(clCommandEvent& event) {}

void SFTPTreeView::OnFindFinished(clCommandEvent& event) {}

void SFTPTreeView::OnFindError(clCommandEvent& event) {}

void SFTPTreeView::DoChangeLocation(const wxString& path) { m_view->SetNewRoot(path); }
