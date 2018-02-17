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

#include "SFTPBookmark.h"
#include "SFTPManageBookmarkDlg.h"
#include "SFTPQuickConnectDlg.h"
#include "SFTPSettingsDialog.h"
#include "SFTPTreeView.h"
#include "SFTPUploadDialog.h"
#include "SSHAccountManagerDlg.h"
#include "SSHTerminal.h"
#include "bitmap_loader.h"
#include "clFileOrFolderDropTarget.h"
#include "cl_config.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "macros.h"
#include "sftp.h"
#include "sftp_item_comparator.h"
#include "sftp_settings.h"
#include "sftp_worker_thread.h"
#include "ssh_account_info.h"
#include <algorithm>
#include <vector>
#include <wx/busyinfo.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/textdlg.h>

static const int ID_NEW = ::wxNewId();
static const int ID_RENAME = ::wxNewId();
static const int ID_DELETE = ::wxNewId();
static const int ID_OPEN = ::wxNewId();
static const int ID_NEW_FILE = ::wxNewId();
static const int ID_REFRESH_FOLDER = ::wxNewId();
static const int ID_EXECUTE_COMMAND = ::wxNewId();
static const int ID_SHOW_SIZE_COL = ::wxNewId();
static const int ID_SHOW_TYPE_COL = ::wxNewId();
static const int ID_OPEN_WITH_DEFAULT_APP = ::wxNewId();
static const int ID_OPEN_CONTAINING_FOLDER = ::wxNewId();

SFTPTreeView::SFTPTreeView(wxWindow* parent, SFTP* plugin)
    : SFTPTreeViewBase(parent)
    , m_plugin(plugin)
{
    m_bmpLoader = clGetManager()->GetStdIcons();
    wxImageList* il = m_bmpLoader->MakeStandardMimeImageList();
    m_treeCtrl->AssignImageList(il);

    SFTPSettings settings;
    settings.Load();

    m_treeCtrl->Connect(ID_OPEN, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuOpen), NULL, this);
    m_treeCtrl->Connect(ID_OPEN_WITH_DEFAULT_APP, wxEVT_MENU,
                        wxCommandEventHandler(SFTPTreeView::OnMenuOpenWithDefaultApplication), NULL, this);
    m_treeCtrl->Connect(ID_OPEN_CONTAINING_FOLDER, wxEVT_MENU,
                        wxCommandEventHandler(SFTPTreeView::OnMenuOpenContainingFolder), NULL, this);
    m_treeCtrl->Connect(ID_DELETE, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuDelete), NULL, this);
    m_treeCtrl->Connect(ID_NEW, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuNew), NULL, this);
    m_treeCtrl->Connect(ID_RENAME, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuRename), NULL, this);
    m_treeCtrl->Connect(ID_NEW_FILE, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuNewFile), NULL, this);
    m_treeCtrl->Connect(ID_REFRESH_FOLDER, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuRefreshFolder), NULL,
                        this);

    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnCopy, this, wxID_COPY);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnCut, this, wxID_CUT);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnPaste, this, wxID_PASTE);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnSelectAll, this, wxID_SELECTALL);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnUndo, this, wxID_UNDO);
    wxTheApp->GetTopWindow()->Bind(wxEVT_MENU, &SFTPTreeView::OnRedo, this, wxID_REDO);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING, &SFTPTreeView::OnEditorClosing, this);
    m_treeCtrl->SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FILE_DROPPED, &SFTPTreeView::OnFileDropped, this);

    m_keyboardHelper.reset(new clTreeKeyboardInput(m_treeCtrl));
    ::MSWSetNativeTheme(m_treeCtrl);
}

SFTPTreeView::~SFTPTreeView()
{
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CLOSING, &SFTPTreeView::OnEditorClosing, this);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnCopy, this, wxID_COPY);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnCut, this, wxID_CUT);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnPaste, this, wxID_PASTE);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnSelectAll, this, wxID_SELECTALL);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnUndo, this, wxID_UNDO);
    wxTheApp->GetTopWindow()->Unbind(wxEVT_MENU, &SFTPTreeView::OnRedo, this, wxID_REDO);

    m_treeCtrl->Disconnect(ID_OPEN, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuOpen), NULL, this);
    m_treeCtrl->Disconnect(ID_OPEN_WITH_DEFAULT_APP, wxEVT_MENU,
                           wxCommandEventHandler(SFTPTreeView::OnMenuOpenWithDefaultApplication), NULL, this);
    m_treeCtrl->Disconnect(ID_OPEN_CONTAINING_FOLDER, wxEVT_MENU,
                           wxCommandEventHandler(SFTPTreeView::OnMenuOpenContainingFolder), NULL, this);

    m_treeCtrl->Disconnect(ID_DELETE, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuDelete), NULL, this);
    m_treeCtrl->Disconnect(ID_NEW, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuNew), NULL, this);
    m_treeCtrl->Disconnect(ID_RENAME, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuRename), NULL, this);
    m_treeCtrl->Disconnect(ID_NEW_FILE, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuNewFile), NULL, this);
    m_treeCtrl->Disconnect(ID_REFRESH_FOLDER, wxEVT_MENU, wxCommandEventHandler(SFTPTreeView::OnMenuRefreshFolder),
                           NULL, this);
    Unbind(wxEVT_DND_FILE_DROPPED, &SFTPTreeView::OnFileDropped, this);
}

void SFTPTreeView::OnDisconnect(wxCommandEvent& event) { DoCloseSession(); }
void SFTPTreeView::OnConnect(wxCommandEvent& event) { DoOpenSession(); }

void SFTPTreeView::DoBuildTree(const wxString& initialFolder)
{
    m_treeCtrl->DeleteAllItems();
    // add the root item
    MyClientData* cd = new MyClientData(initialFolder);
    cd->SetIsFolder(true);

    wxTreeItemId root =
        m_treeCtrl->AppendItem(m_treeCtrl->GetRootItem(), initialFolder,
                               m_bmpLoader->GetMimeImageId(FileExtManager::TypeFolder), wxNOT_FOUND, cd);

    m_treeCtrl->AppendItem(root, "<dummy>");
    DoExpandItem(root);
}

void SFTPTreeView::OnItemActivated(wxTreeEvent& event)
{
    event.Skip();
    MyClientData* cd = GetItemData(event.GetItem());
    CHECK_PTR_RET(cd);

    if(cd->IsFolder()) {
        wxTreeItemId item = event.GetItem();
        if(m_treeCtrl->IsExpanded(item)) {
            m_treeCtrl->CallAfter(&wxTreeCtrl::Collapse, item);
        } else {
            m_treeCtrl->CallAfter(&wxTreeCtrl::Expand, item);
        }

    } else {
        DoOpenFile(cd->GetFullPath());
    }
}

void SFTPTreeView::OnItemExpanding(wxTreeEvent& event)
{
    if(!DoExpandItem(event.GetItem())) { event.Veto(); }
}

void SFTPTreeView::OnOpenAccountManager(wxCommandEvent& event)
{
    SSHAccountManagerDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        SFTPSettings settings;
        settings.Load().SetAccounts(dlg.GetAccounts());
        settings.Save();
    }
}

void SFTPTreeView::DoCloseSession()
{
    // Check if we have unmodified files belonged to this session
    // Load the session name
    IEditor::List_t editors;
    IEditor::List_t modeditors;
    clGetManager()->GetAllEditors(editors);

    // Create a session
    SFTPSessionInfo sess;
    wxArrayString remoteFiles;
    std::for_each(editors.begin(), editors.end(), [&](IEditor* editor) {
        SFTPClientData* pcd = dynamic_cast<SFTPClientData*>(editor->GetClientData("sftp"));
        if(pcd) {
            sess.GetFiles().push_back(pcd->GetRemotePath());
            if(!clGetManager()->CloseEditor(editor)) { modeditors.push_back(editor); }
        }
    });

    // User cancel to close request, so dont close the session just yet
    if(!modeditors.empty()) { return; }

    // Set the session name
    if(m_sftp) {
        sess.SetAccount(m_sftp->GetAccount());
        sess.SetRootFolder(m_textCtrlQuickJump->GetValue()); // Keep the root folder
        m_sessions.Load().SetSession(sess).Save();
    }

    m_sftp.reset(NULL);
    m_treeCtrl->DeleteAllItems();
}

bool SFTPTreeView::DoExpandItem(const wxTreeItemId& item)
{
    MyClientData* cd = GetItemData(item);
    CHECK_PTR_RET_FALSE(cd);

    // already initialized this folder before?
    if(cd->IsInitialized()) { return true; }

    // get list of files and populate the tree
    SFTPAttribute::List_t attributes;
    try {
        attributes = m_sftp->List(cd->GetFullPath(), clSFTP::SFTP_BROWSE_FILES | clSFTP::SFTP_BROWSE_FOLDERS);

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxOK | wxICON_ERROR | wxCENTER, EventNotifier::Get()->TopFrame());
        return false;
    }

    // Remove the dummy item and replace it with real items
    wxTreeItemIdValue cookie;
    wxTreeItemId dummyItem = m_treeCtrl->GetFirstChild(item, cookie);
    m_treeCtrl->Delete(dummyItem);
    cd->SetInitialized(true);

    int nNumOfRealChildren = 0;
    SFTPAttribute::List_t::iterator iter = attributes.begin();
    for(; iter != attributes.end(); ++iter) {
        SFTPAttribute::Ptr_t attr = (*iter);
        if(attr->GetName() == "." || attr->GetName() == "..") continue;

        ++nNumOfRealChildren;
        // determine the icon index
        int imgIdx = wxNOT_FOUND;
        if(attr->IsFolder()) {
            imgIdx = m_bmpLoader->GetMimeImageId(FileExtManager::TypeFolder);

        } else {
            imgIdx = m_bmpLoader->GetMimeImageId(attr->GetName());
        }

        if(imgIdx == wxNOT_FOUND) { imgIdx = m_bmpLoader->GetMimeImageId(FileExtManager::TypeText); }

        wxString path;
        path << cd->GetFullPath() << "/" << attr->GetName();
        while(path.Replace("//", "/")) {}

        MyClientData* childClientData = new MyClientData(path);
        childClientData->SetIsFolder(attr->IsFolder());

        wxTreeItemId child = m_treeCtrl->AppendItem(item, attr->GetName(), imgIdx, imgIdx, childClientData);
        // if its type folder, add a fake child item
        if(attr->IsFolder()) { m_treeCtrl->AppendItem(child, "<dummy>"); }
    }

    return nNumOfRealChildren > 0;
}

MyClientData* SFTPTreeView::GetItemData(const wxTreeItemId& item)
{
    CHECK_ITEM_RET_NULL(item);
    MyClientData* cd = dynamic_cast<MyClientData*>(m_treeCtrl->GetItemData(item));
    return cd;
}

MyClientDataVect_t SFTPTreeView::GetSelectionsItemData()
{
    MyClientDataVect_t res;
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);

    for(size_t i = 0; i < items.GetCount(); ++i) {
        MyClientData* cd = GetItemData(items.Item(i));
        if(cd) { res.push_back(cd); }
    }
    return res;
}

void SFTPTreeView::OnDisconnectUI(wxUpdateUIEvent& event) { event.Enable(m_sftp); }

void SFTPTreeView::OnContextMenu(wxContextMenuEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.size() == 0) return;

    wxTreeItemId item = items.Item(0);
    CHECK_ITEM_RET(item);

    MyClientData* cd = GetItemData(item);
    wxMenu menu;
    if(cd) {

        // Just incase, make sure the item is selected
        m_treeCtrl->SelectItem(item);
        if(!cd->IsFolder()) {
            menu.Append(ID_OPEN, _("Open"));
            menu.Append(ID_OPEN_WITH_DEFAULT_APP, _("Open with Default Application..."));
            menu.AppendSeparator();
            menu.Append(ID_OPEN_CONTAINING_FOLDER, _("Download and Open Containing Folder..."));
            menu.AppendSeparator();

        } else {
            menu.Append(ID_NEW, _("Create new directory..."));
            menu.Append(ID_NEW_FILE, _("Create new file..."));
            menu.AppendSeparator();
            menu.Append(ID_REFRESH_FOLDER, _("Refresh"));
            menu.AppendSeparator();
        }
        menu.Append(ID_DELETE, _("Delete"));

#ifdef __WXMAC__
        menu.Enable(ID_DELETE, false);
#endif

        menu.Append(ID_RENAME, _("Rename"));
    }
    m_treeCtrl->PopupMenu(&menu);
}

void SFTPTreeView::OnMenuDelete(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.empty()) return;

    wxString message;
    message << _("Are you sure you want to delete the selected items?");
    if(::wxMessageBox(message, "Confirm", wxYES_NO | wxCANCEL | wxICON_QUESTION) != wxYES) { return; }

    try {

        for(size_t i = 0; i < items.size(); ++i) {
            MyClientData* cd = GetItemData(items.Item(i));
            if(cd->IsFolder()) {
                m_sftp->RemoveDir(cd->GetFullPath());

            } else {
                m_sftp->UnlinkFile(cd->GetFullPath());
            }
            // Remove the selection
            m_treeCtrl->Delete(items.Item(i));
        }

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK | wxCENTER);
    }
}

void SFTPTreeView::OnMenuNew(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.size() != 1) return;

    MyClientData* cd = GetItemData(items.Item(0));
    CHECK_PTR_RET(cd);

    if(!cd->IsFolder()) { return; }

    wxString new_name = ::wxGetTextFromUser(_("Enter the new directory name:"), _("New Directory"));
    if(!new_name.IsEmpty()) {

        wxString fullpath = cd->GetFullPath();
        fullpath << "/" << new_name;
        DoAddFolder(items.Item(0), fullpath);
    }
}
void SFTPTreeView::OnMenuNewFile(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.size() != 1) return;

    MyClientData* cd = GetItemData(items.Item(0));
    CHECK_PTR_RET(cd);

    if(!cd->IsFolder()) { return; }

    wxString defaultValue;
    static size_t s_untitledCounter = 0;
    defaultValue << "Untitled" << ++s_untitledCounter;

    wxString new_name = ::wxGetTextFromUser(_("Enter the new file name:"), _("New File"), defaultValue);
    if(!new_name.IsEmpty()) {
        wxString fullpath = cd->GetFullPath();
        fullpath << "/" << new_name;
        wxTreeItemId fileItem = DoAddFile(items.Item(0), fullpath);
        if(fileItem.IsOk()) { DoOpenFile(fileItem); }
    }
}

void SFTPTreeView::OnMenuRename(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.empty()) return;

    try {
        for(size_t i = 0; i < items.size(); ++i) {

            MyClientData* cd = GetItemData(items.Item(i));
            if(!cd) continue; // ??

            wxString new_name = ::wxGetTextFromUser(_("Enter new name:"), _("Rename"), cd->GetFullName());
            if(!new_name.IsEmpty()) {
                wxString old_path = cd->GetFullPath();
                cd->SetFullName(new_name);
                m_sftp->Rename(old_path, cd->GetFullPath());

                // Remove the selection
                m_treeCtrl->SetItemText(items.Item(i), new_name);
            }
        }

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK | wxCENTER);
    }
}

void SFTPTreeView::OnMenuOpen(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.empty()) return;

    for(size_t i = 0; i < items.size(); ++i) {
        MyClientData* cd = GetItemData(items.Item(i));
        if(!cd || cd->IsFolder()) { continue; }

        RemoteFileInfo remoteFile;
        remoteFile.SetAccount(m_account);
        remoteFile.SetRemoteFile(cd->GetFullPath());

        SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
        SFTPWorkerThread::Instance()->Add(req);

        m_plugin->AddRemoteFile(remoteFile);
    }
}

wxTreeItemId SFTPTreeView::DoAddFile(const wxTreeItemId& parent, const wxString& path)
{
    try {
        wxMemoryBuffer memBuffer;
        m_sftp->Write(memBuffer, path);
        SFTPAttribute::Ptr_t attr = m_sftp->Stat(path);
        // Update the UI
        MyClientData* newFile = new MyClientData(path);
        newFile->SetIsFolder(false);
        newFile->SetInitialized(false);

        wxTreeItemId child = m_treeCtrl->AppendItem(
            parent, newFile->GetFullName(),
            m_bmpLoader->GetMimeImageId(FileExtManager::GetType(path, FileExtManager::TypeText)), wxNOT_FOUND, newFile);
        return child;

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK | wxCENTER);
    }
    return wxTreeItemId();
}

wxTreeItemId SFTPTreeView::DoAddFolder(const wxTreeItemId& parent, const wxString& path)
{
    try {
        m_sftp->CreateDir(path);
        SFTPAttribute::Ptr_t attr = m_sftp->Stat(path);
        // Update the UI
        MyClientData* newCd = new MyClientData(path);
        newCd->SetIsFolder(true);
        newCd->SetInitialized(false);

        wxTreeItemId child = m_treeCtrl->AppendItem(
            parent, newCd->GetFullName(), m_bmpLoader->GetMimeImageId(FileExtManager::TypeFolder), wxNOT_FOUND, newCd);

        m_treeCtrl->AppendItem(child, "<dummy>");
        return child;

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK | wxCENTER);
    }
    return wxTreeItemId();
}

void SFTPTreeView::OnAddBookmark(wxAuiToolBarEvent& event)
{
    if(event.IsDropDownClicked()) {

        // Show the menu
        const wxArrayString& bookmarks = m_account.GetBookmarks();
        wxMenu menu;
        for(size_t i = 0; i < bookmarks.GetCount(); ++i) {
            menu.Append(ID_SFTP_BOOKMARK_FIRST + i, bookmarks.Item(i));
        }
        menu.AppendSeparator();
        menu.Append(ID_SFTP_BOOKMARK_SETTINGS, _("Manage bookmarks..."));

        wxPoint pt = event.GetItemRect().GetBottomLeft();
        pt.y++;

        int sel = m_auibar->GetPopupMenuSelectionFromUser(menu, pt);
        if(sel >= ID_SFTP_BOOKMARK_FIRST && sel <= ID_SFTP_BOOKMARK_LAST) {
            // A bookmark was selected
            CallAfter(&SFTPTreeView::DoBuildTree, bookmarks.Item(sel - ID_SFTP_BOOKMARK_FIRST));

        } else if(sel == ID_SFTP_BOOKMARK_SETTINGS) {
            // Bookmark settings
            CallAfter(&SFTPTreeView::ManageBookmarks);
        }

    } else {
        try {
            // sanity
            if(!IsConnected()) return;

            // Get the current selection
            MyClientDataVect_t selections = GetSelectionsItemData();
            if(selections.size() != 1) return;

            MyClientData* cd = selections.at(0);
            CHECK_PTR_RET(cd);

            if(!cd->IsFolder()) return;

            m_account.AddBookmark(cd->GetFullPath());
            SFTPSettings settings;
            settings.Load();
            settings.UpdateAccount(m_account);
            settings.Save();

        } catch(clException& e) {
            ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR | wxOK | wxCENTER);
        }
    }
}

void SFTPTreeView::OnAddBookmarkUI(wxUpdateUIEvent& event) { event.Enable(IsConnected()); }
void SFTPTreeView::OnGotoLocation(wxCommandEvent& event) { DoBuildTree(m_textCtrlQuickJump->GetValue()); }
void SFTPTreeView::OnGotoLocationUI(wxUpdateUIEvent& event) { event.Enable(IsConnected()); }

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

void SFTPTreeView::OnChoiceAccountUI(wxUpdateUIEvent& event) { event.Enable(!IsConnected()); }
void SFTPTreeView::OnSelectionChanged(wxTreeEvent& event)
{
    MyClientDataVect_t items = GetSelectionsItemData();
    if(items.size() != 1) return;

    MyClientData* cd = items.at(0);
    if(cd->IsFolder()) { m_textCtrlQuickJump->ChangeValue(cd->GetFullPath()); }
}
void SFTPTreeView::OnConnection(wxCommandEvent& event)
{
    SFTPImages images;
    if(IsConnected()) {
        // Disconnect
        DoCloseSession();
        // Update toobar image
        m_auibar->FindTool(ID_SFTP_CONNECT)->SetBitmap(images.Bitmap("sftp_disconnected"));
        m_auibar->FindTool(ID_SFTP_CONNECT)->SetShortHelp(_("Disconnected. Click to connect"));
    } else {
        DoOpenSession();
        if(IsConnected()) {
            // Update toobar image
            m_auibar->FindTool(ID_SFTP_CONNECT)->SetBitmap(images.Bitmap("sftp_connected"));
            m_auibar->FindTool(ID_SFTP_CONNECT)->SetShortHelp(_("Connected. Click to disconnect"));
        }
    }
}

void SFTPTreeView::DoOpenSession()
{
    DoCloseSession();
    if(!GetAccountFromUser(m_account)) { return; }

    wxString message;
    wxProgressDialog dlg(_("SFTP"), wxString(' ', 100) + "\n\n", 10);
    dlg.Show();
    dlg.Update(1, wxString() << _("Connecting to: ") << m_account.GetAccountName() << "..."
                             << _("\n(this may take a few seconds)"));

    // We know that there is a bug that libssh succeeded on connecting only on the second attempt..
    // to workaround it, we issue first connect with 1 second timeout, and then re-open the connection
    try {
        clSSH::Ptr_t ssh(
            new clSSH(m_account.GetHost(), m_account.GetUsername(), m_account.GetPassword(), m_account.GetPort()));
        ssh->Connect(wxNOT_FOUND);
    } catch(...) {
    }

    try {
        clSSH::Ptr_t ssh(
            new clSSH(m_account.GetHost(), m_account.GetUsername(), m_account.GetPassword(), m_account.GetPort()));
        ssh->Connect(5);
        dlg.Update(5, _("Connected!"));
        dlg.Update(6, _("Authenticating server..."));

        if(!ssh->AuthenticateServer(message)) {
            if(::wxMessageBox(message, "SSH", wxYES_NO | wxCENTER | wxICON_QUESTION) == wxYES) {
                dlg.Update(7, _("Accepting server authentication server..."));
                ssh->AcceptServerAuthentication();
            }
        } else {
            dlg.Update(7, _("Server authenticated"));
        }

        dlg.Update(8, _("Logging in.."));
        ssh->Login();
        m_sftp.reset(new clSFTP(ssh));
        m_sftp->Initialize();
        m_sftp->SetAccount(m_account.GetAccountName());
        m_plugin->GetManager()->SetStatusMessage(wxString() << _("Done!"));

        dlg.Update(9, _("Fetching directory list..."));
        DoBuildTree(m_account.GetDefaultFolder().IsEmpty() ? "/" : m_account.GetDefaultFolder());
        dlg.Update(10, _("Done"));

        CallAfter(&SFTPTreeView::DoLoadSession);

        // If this is a new account, offer the user to save it
        SFTPSettings s;
        s.Load();
        SSHAccountInfo dummy;
        if(!s.GetAccount(m_account.GetAccountName(), dummy)) {
            wxString message;
            message << _("Would you like to save this account?\n") << _("It will be saved as '")
                    << m_account.GetAccountName() << "'";
            wxStandardID res = ::PromptForYesNoCancelDialogWithCheckbox(message, "SFTPQuickConnectSaveDlg");
            if(res == wxID_YES) {
                // This 'Connect' was via Quick Connect option
                SSHAccountInfo::Vect_t accounts = s.GetAccounts();
                accounts.push_back(m_account);
                s.SetAccounts(accounts);
                s.Save();
            }
        }

    } catch(clException& e) {
        ::wxMessageBox(e.What(), "CodeLite", wxICON_ERROR | wxOK);
        DoCloseSession();
    }
}

void SFTPTreeView::OnMenuRefreshFolder(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.size() != 1) return;

    wxTreeItemId item = items.Item(0);
    MyClientData* cd = GetItemData(item);
    if(!cd || !cd->IsFolder()) { return; }

    // Uninitialize the folder
    cd->SetInitialized(false);

    // Delete all the children
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrl->GetFirstChild(item, cookie);
    while(child.IsOk()) {
        wxTreeItemId nextChild = m_treeCtrl->GetNextSibling(child);
        m_treeCtrl->Delete(child);
        child = nextChild;
    }

    // Re-append the dummy item
    m_treeCtrl->AppendItem(item, "<dummy>");
    m_treeCtrl->Collapse(item);
}

void SFTPTreeView::OnCopy(wxCommandEvent& event)
{
    event.Skip();
    if(m_textCtrlQuickJump->HasFocus()) { event.Skip(false); }
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

void SFTPTreeView::OnOpenTerminal(wxAuiToolBarEvent& event)
{
    // Open terminal to the selected account
    if(event.IsDropDownClicked()) {
        SFTPTreeViewBase::ShowAuiToolMenu(event);

    } else {
        SSHAccountInfo account;
        if(!GetAccountFromUser(account)) { return; }

        wxString connectString;
        connectString << account.GetUsername() << "@" << account.GetHost();

        SFTPSettings settings;
        settings.Load();

        const wxString& sshClient = settings.GetSshClient();
        FileUtils::OpenSSHTerminal(sshClient, connectString, account.GetPassword(), account.GetPort());
    }
}

void SFTPTreeView::OnOpenTerminalUI(wxUpdateUIEvent& event) { event.Enable(true); }

bool SFTPTreeView::DoOpenFile(const wxTreeItemId& item)
{
    MyClientData* cd = GetItemData(item);
    if(!cd || cd->IsFolder()) { return false; }

    RemoteFileInfo remoteFile;
    remoteFile.SetAccount(m_account);
    remoteFile.SetRemoteFile(cd->GetFullPath());

    SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
    SFTPWorkerThread::Instance()->Add(req);

    m_plugin->AddRemoteFile(remoteFile);
    return true;
}

void SFTPTreeView::DoDeleteColumn(int colIdx) { wxUnusedVar(colIdx); }

void SFTPTreeView::OnSftpSettings(wxCommandEvent& event)
{
    // Show the SFTP settings dialog
    SFTPSettingsDialog dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

void SFTPTreeView::OnMenuOpenWithDefaultApplication(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.empty()) return;

    for(size_t i = 0; i < items.size(); ++i) {
        MyClientData* cd = GetItemData(items.Item(i));
        if(!cd || cd->IsFolder()) { continue; }

        RemoteFileInfo remoteFile;
        remoteFile.SetAccount(m_account);
        remoteFile.SetRemoteFile(cd->GetFullPath());

        SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
        req->SetAction(eSFTPActions::kDownloadAndOpenWithDefaultApp);
        SFTPWorkerThread::Instance()->Add(req);
    }
}

void SFTPTreeView::OnMenuOpenContainingFolder(wxCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    if(items.empty()) return;

    for(size_t i = 0; i < items.size(); ++i) {
        MyClientData* cd = GetItemData(items.Item(i));
        if(!cd || cd->IsFolder()) { continue; }

        RemoteFileInfo remoteFile;
        remoteFile.SetAccount(m_account);
        remoteFile.SetRemoteFile(cd->GetFullPath());

        SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
        req->SetAction(eSFTPActions::kDownloadAndOpenContainingFolder);
        SFTPWorkerThread::Instance()->Add(req);
    }
}

void SFTPTreeView::OnFileDropped(clCommandEvent& event)
{
    wxArrayTreeItemIds items;
    m_treeCtrl->GetSelections(items);
    wxTreeItemId parenItem;
    wxString defaultPath = m_textCtrlQuickJump->GetValue();
    if(items.size() == 1) {
        // Use the selected folder as the default path
        MyClientData* cd = GetItemData(items.Item(0));
        if(cd && cd->IsFolder()) {
            defaultPath = cd->GetFullPath();
            parenItem = items.Item(0);
        }
    }

    SFTPUploadDialog dlg(EventNotifier::Get()->TopFrame());
    dlg.GetTextCtrlRemoteFolder()->ChangeValue(defaultPath);
    if(dlg.ShowModal() != wxID_OK) return;

    const wxString targetFolder = dlg.GetTextCtrlRemoteFolder()->GetValue();
    const wxArrayString& files = event.GetStrings();
    for(size_t i = 0; i < files.size(); ++i) {
        wxFileName localFile(files.Item(i));
        wxString remotePath;
        remotePath << targetFolder << "/" << localFile.GetFullName();
        if(parenItem.IsOk()) {
            wxTreeItemId fileItem = DoAddFile(parenItem, remotePath);
            if(!fileItem.IsOk()) continue;
        }
        SFTPWorkerThread::Instance()->Add(new SFTPThreadRequet(m_account, remotePath, localFile.GetFullPath(), 0));
    }
}

bool SFTPTreeView::GetAccountFromUser(SSHAccountInfo& account)
{
    SFTPQuickConnectDlg connectDialog(EventNotifier::Get()->TopFrame());
    if(connectDialog.ShowModal() != wxID_OK) { return false; }

    // Get the selected account
    account = connectDialog.GetSelectedAccount();
    return true;
}

void SFTPTreeView::DoOpenFile(const wxString& path)
{
    RemoteFileInfo remoteFile;
    remoteFile.SetAccount(m_account);
    remoteFile.SetRemoteFile(path);

    SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
    SFTPWorkerThread::Instance()->Add(req);

    m_plugin->AddRemoteFile(remoteFile);
    // Update the session
    SFTPSessionInfo& sess = GetSession(false);
    if(sess.IsOk()) {
        sess.AddFile(path);
        m_sessions.Save();
    }
}

void SFTPTreeView::OnEditorClosing(wxCommandEvent& evt)
{
    evt.Skip();
    IEditor* editor = (IEditor*)evt.GetClientData();
    SFTPClientData* pcd = dynamic_cast<SFTPClientData*>(editor->GetClientData("sftp"));
    if(pcd) {
        // A file opened by SFTP, remove it from the current session
        SFTPSessionInfo& sess = GetSession(false);
        if(sess.IsOk()) {
            sess.RemoveFile(pcd->GetRemotePath());
            m_sessions.Save();
        }
    }
}

SFTPSessionInfo& SFTPTreeView::GetSession(bool createIfMissing)
{
    SFTPSessionInfo& sess = m_sessions.GetSession(m_account.GetAccountName());
    if(!sess.IsOk() && createIfMissing) {
        SFTPSessionInfo s;
        s.SetAccount(m_account.GetAccountName());
        m_sessions.SetSession(s).Save();
        return m_sessions.GetSession(m_account.GetAccountName());
    }
    return sess;
}

void SFTPTreeView::DoLoadSession()
{
    // Now that we have successfully opened the connection, try to load the last saved session for this account
    const SFTPSessionInfo& sess = GetSession(true);
    if(sess.IsOk()) {
        wxString msg;
        msg << _("Would you like to load the saved session for this account?");
        wxStandardID ans = ::PromptForYesNoCancelDialogWithCheckbox(msg, "sftp-load-session-dlg");
        if(ans == wxID_YES) {
            // we have a session for this account, load it
            // Load the files
            const std::vector<wxString>& files = sess.GetFiles();
            std::for_each(files.begin(), files.end(), [&](const wxString& path) { DoOpenFile(path); });

            const wxString& rootFolder = sess.GetRootFolder();
            if(!rootFolder.IsEmpty()) {
                m_textCtrlQuickJump->ChangeValue(rootFolder);
                CallAfter(&SFTPTreeView::DoBuildTree, rootFolder);
            }
        }
    }
}
