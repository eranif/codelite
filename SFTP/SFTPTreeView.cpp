#include "SFTPTreeView.h"
#include "SSHAccountManagerDlg.h"
#include "sftp_settings.h"
#include <wx/msgdlg.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include "ssh_account_info.h"
#include "bitmap_loader.h"
#include "macros.h"
#include "sftp_worker_thread.h"
#include "sftp.h"
#include "event_notifier.h"
#include <vector>
#include "sftp_item_comparator.h"

static const int ID_NEW = ::wxNewId();
static const int ID_RENAME        = ::wxNewId();
static const int ID_DELETE        = ::wxNewId();
static const int ID_OPEN          = ::wxNewId();

SFTPTreeView::SFTPTreeView(wxWindow* parent, SFTP* plugin)
    : SFTPTreeViewBase(parent)
    , m_plugin(plugin)
{
    wxImageList* il = m_bmpLoader.MakeStandardMimeImageList();
    m_treeListCtrl->AssignImageList( il );

    SFTPSettings settings;
    SFTPSettings::Load( settings );

    const SSHAccountInfo::List_t& accounts = settings.GetAccounts();
    SSHAccountInfo::List_t::const_iterator iter = accounts.begin();
    for(; iter != accounts.end(); ++iter ) {
        m_choiceAccount->Append( iter->GetAccountName() );
    }

    if ( !m_choiceAccount->IsEmpty() ) {
        m_choiceAccount->SetSelection(0);
    }

#ifdef __WXMSW__
    m_treeListCtrl->GetDataView()->SetIndent( 16 );
#endif
    m_treeListCtrl->SetItemComparator( new SFTPItemComparator );
    m_treeListCtrl->Connect(ID_OPEN,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTPTreeView::OnMenuOpen), NULL, this);
    m_treeListCtrl->Connect(ID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTPTreeView::OnMenuDelete), NULL, this);
    m_treeListCtrl->Connect(ID_NEW,    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTPTreeView::OnMenuNew), NULL, this);
    m_treeListCtrl->Connect(ID_RENAME, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SFTPTreeView::OnMenuRename), NULL, this);
}

SFTPTreeView::~SFTPTreeView()
{
}

void SFTPTreeView::OnConnect(wxCommandEvent& event)
{
    DoCloseSession();
    wxString accountName = m_choiceAccount->GetStringSelection();
    if ( accountName.IsEmpty() ) {
        return;
    }

    SFTPSettings settings;
    SFTPSettings::Load( settings );

    m_account = SSHAccountInfo();
    if ( !settings.GetAccount( accountName, m_account) ) {
        ::wxMessageBox(wxString() << _("Could not find account: ") << accountName, "codelite", wxICON_ERROR|wxOK);
        return;
    }

    clSSH::Ptr_t ssh( new clSSH(m_account.GetHost(), m_account.GetUsername(), m_account.GetPassword(), m_account.GetPort()) );
    try {
        wxString message;
        m_plugin->GetManager()->SetStatusMessage( wxString() << _("Connecting to: ") << accountName << "..." );
        ssh->Connect();
        if ( !ssh->AuthenticateServer( message ) ) {
            if ( ::wxMessageBox(message, "SSH", wxYES_NO|wxCENTER|wxICON_QUESTION) == wxYES ) {
                ssh->AcceptServerAuthentication();
            }
        }

        ssh->Login();
        m_sftp.reset( new clSFTP(ssh) );
        m_sftp->Initialize();
        m_plugin->GetManager()->SetStatusMessage( wxString() << _("Done!") );
        // add the root item
        wxTreeListItem root = m_treeListCtrl->AppendItem( m_treeListCtrl->GetRootItem(),
                              "/",
                              m_bmpLoader.GetMimeImageId(FileExtManager::TypeFolder),
                              wxNOT_FOUND,
                              new MyClientData("/")
                                                        );
        m_treeListCtrl->AppendItem(root, "<dummy>");
        DoExpandItem( root );

    } catch (clException &e) {
        ::wxMessageBox(e.What(), "codelite", wxICON_ERROR|wxOK);
        DoCloseSession();
    }
}

void SFTPTreeView::OnItemActivated(wxTreeListEvent& event)
{
    event.Skip();
    MyClientData *cd = GetItemData(event.GetItem());
    CHECK_PTR_RET(cd);

    if ( cd->IsFolder() ) {
        m_treeListCtrl->Expand( event.GetItem() );

    } else {

        RemoteFileInfo remoteFile;
        remoteFile.SetAccount( m_account );
        remoteFile.SetRemoteFile( cd->GetFullPath() );

        SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
        SFTPWorkerThread::Instance()->Add( req );

        m_plugin->AddRemoteFile(remoteFile);
    }
}

void SFTPTreeView::OnItemExpanding(wxTreeListEvent& event)
{
    if ( !DoExpandItem(event.GetItem()) ) {
        event.Veto();
    }
}

void SFTPTreeView::OnOpenAccountManager(wxCommandEvent& event)
{
    SSHAccountManagerDlg dlg(this);
    if ( dlg.ShowModal() == wxID_OK ) {

        SFTPSettings settings;
        SFTPSettings::Load( settings );

        settings.SetAccounts( dlg.GetAccounts() );
        SFTPSettings::Save( settings );

        // Update the selections at the top
        wxString curselection = m_choiceAccount->GetStringSelection();

        m_choiceAccount->Clear();
        const SSHAccountInfo::List_t& accounts = settings.GetAccounts();
        if ( accounts.empty() ) {
            DoCloseSession();
            return;

        } else {
            SSHAccountInfo::List_t::const_iterator iter = accounts.begin();
            for(; iter != accounts.end(); ++iter ) {
                m_choiceAccount->Append( iter->GetAccountName() );
            }

            int where = m_choiceAccount->FindString(curselection);
            if ( where == wxNOT_FOUND ) {
                // Our previous session is no longer available, close the session
                DoCloseSession();
                where = 0;
            }

            m_choiceAccount->SetSelection(where);
        }
    }
}

void SFTPTreeView::DoCloseSession()
{
    m_sftp.reset(NULL);
    m_treeListCtrl->DeleteAllItems();
}

void SFTPTreeView::OnConnectUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_choiceAccount->GetStringSelection().IsEmpty() && !m_sftp );
}

bool SFTPTreeView::DoExpandItem(const wxTreeListItem& item)
{
    MyClientData *cd = GetItemData( item );
    CHECK_PTR_RET_FALSE(cd);

    // already initialized this folder before?
    if ( cd->IsInitialized() ) {
        return true;
    }

    // get list of files and populate the tree
    SFTPAttribute::List_t attributes;
    try {
        attributes = m_sftp->List( cd->GetFullPath(), clSFTP::SFTP_BROWSE_FILES|clSFTP::SFTP_BROWSE_FOLDERS );

    } catch (clException &e) {
        ::wxMessageBox(e.What(), "SFTP", wxOK|wxICON_ERROR|wxCENTER, EventNotifier::Get()->TopFrame());
        return false;
    }

    // Remove the dummy item and replace it with real items
    wxTreeListItem dummyItem = m_treeListCtrl->GetFirstChild(item);
    m_treeListCtrl->DeleteItem( dummyItem );
    cd->SetInitialized( true );

    SFTPAttribute::List_t::iterator iter = attributes.begin();
    for( ; iter != attributes.end(); ++iter ) {
        SFTPAttribute::Ptr_t attr = (*iter);
        if ( attr->GetName() == "." || attr->GetName() == ".." )
            continue;

        // determine the icon index
        int imgIdx = wxNOT_FOUND;
        if ( attr->IsFolder() ) {
            imgIdx = m_bmpLoader.GetMimeImageId(FileExtManager::TypeFolder);

        } else {
            imgIdx = m_bmpLoader.GetMimeImageId(attr->GetName());
        }

        if ( imgIdx == wxNOT_FOUND ) {
            imgIdx = m_bmpLoader.GetMimeImageId(FileExtManager::TypeText);
        }

        wxString path;
        path << cd->GetFullPath() << "/" << attr->GetName();
        while (path.Replace("//", "/")) {}

        MyClientData* childClientData = new MyClientData(path);
        childClientData->SetIsFolder( attr->IsFolder() );

        wxTreeListItem child = m_treeListCtrl->AppendItem(item, attr->GetName(), imgIdx, imgIdx, childClientData);
        m_treeListCtrl->SetItemText(child, 1, attr->GetTypeAsString());
        m_treeListCtrl->SetItemText(child, 2, wxString() << attr->GetSize());

        // if its type folder, add a fake child item
        if ( attr->IsFolder() ) {
            m_treeListCtrl->AppendItem(child, "<dummy>");
        }
    }
    return true;
}

MyClientData* SFTPTreeView::GetItemData(const wxTreeListItem& item)
{
    CHECK_ITEM_RET_NULL(item);
    MyClientData *cd = dynamic_cast<MyClientData*>(m_treeListCtrl->GetItemData(item));
    return cd;
}

MyClientDataVect_t SFTPTreeView::GetSelectionsItemData()
{
    MyClientDataVect_t res;
    wxTreeListItems items;
    m_treeListCtrl->GetSelections(items);

    for(size_t i=0; i<items.size(); ++i) {
        MyClientData *cd = GetItemData(items.at(i));
        if ( cd ) {
            res.push_back( cd );
        }
    }
    return res;
}

void SFTPTreeView::OnDisconnect(wxCommandEvent& event)
{
    DoCloseSession();
}

void SFTPTreeView::OnDisconnectUI(wxUpdateUIEvent& event)
{
    event.Enable( m_sftp );
}

void SFTPTreeView::OnContextMenu(wxTreeListEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());

    MyClientData* cd = GetItemData(event.GetItem());
    CHECK_PTR_RET(cd);

    // Just incase, make sure the item is selected
    m_treeListCtrl->Select(event.GetItem());

    wxMenu menu;

    if ( !cd->IsFolder() ) {
        menu.Append(ID_OPEN, _("Open"));
        menu.AppendSeparator();

    } else {
        menu.Append(ID_NEW, _("Create directory"));
        menu.AppendSeparator();
    }
    menu.Append(ID_DELETE, _("Delete"));

#ifdef __WXMAC__
    menu.Enable(ID_DELETE, false);
#endif

    menu.Append(ID_RENAME, _("Rename"));
    m_treeListCtrl->PopupMenu( &menu );
}

void SFTPTreeView::OnMenuDelete(wxCommandEvent& event)
{
    wxTreeListItems items;
    m_treeListCtrl->GetSelections(items);
    if ( items.empty() )
        return;

    wxString message;
    message << _("Are you sure you want to delete the selected items?");
    if ( ::wxMessageBox(message, "Confirm", wxYES_NO|wxCANCEL|wxICON_QUESTION) != wxYES ) {
        return;
    }

    try {

        for(size_t i=0; i<items.size(); ++i) {
            MyClientData *cd = GetItemData(items.at(i));
            if ( cd->IsFolder() ) {
                m_sftp->RemoveDir( cd->GetFullPath() );

            } else {
                m_sftp->UnlinkFile( cd->GetFullPath() );
            }
            // Remove the selection
            m_treeListCtrl->DeleteItem( items.at(i) );
        }

    } catch (clException &e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR|wxOK|wxCENTER);
    }
}

void SFTPTreeView::OnMenuNew(wxCommandEvent& event)
{
    wxTreeListItems items;
    m_treeListCtrl->GetSelections(items);
    if ( items.size() != 1)
        return;

    MyClientData *cd = GetItemData(items.at(0));
    CHECK_PTR_RET(cd);

    if ( !cd->IsFolder() ) {
        return;
    }

    wxString new_name = ::wxGetTextFromUser(_("Enter the new directory name:"), _("New Directory"));
    if ( !new_name.IsEmpty() ) {

        wxString fullpath = cd->GetFullPath();
        fullpath << "/" << new_name;
        DoAddFolder(items.at(0), fullpath);
    }
}

void SFTPTreeView::OnMenuRename(wxCommandEvent& event)
{
    wxTreeListItems items;
    m_treeListCtrl->GetSelections(items);
    if ( items.empty() )
        return;

    try {
        for(size_t i=0; i<items.size(); ++i) {

            MyClientData *cd = GetItemData(items.at(i));
            if ( !cd ) continue; // ??

            wxString new_name = ::wxGetTextFromUser(_("Enter new name:"), _("Rename"), cd->GetFullName());
            if ( !new_name.IsEmpty() ) {
                wxString old_path = cd->GetFullPath();
                cd->SetFullName( new_name );
                m_sftp->Rename( old_path, cd->GetFullPath() );

                // Remove the selection
                m_treeListCtrl->SetItemText(items.at(i), 0, new_name);

            }
        }

    } catch (clException &e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR|wxOK|wxCENTER);
    }
}


void SFTPTreeView::OnMenuOpen(wxCommandEvent& event)
{
    wxTreeListItems items;
    m_treeListCtrl->GetSelections(items);
    if ( items.empty() )
        return;

    for(size_t i=0; i<items.size(); ++i) {
        MyClientData *cd = GetItemData(items.at(i));
        if ( !cd || cd->IsFolder() ) {
            continue;
        }

        RemoteFileInfo remoteFile;
        remoteFile.SetAccount( m_account );
        remoteFile.SetRemoteFile( cd->GetFullPath() );

        SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
        SFTPWorkerThread::Instance()->Add( req );

        m_plugin->AddRemoteFile(remoteFile);
    }
}

wxTreeListItem SFTPTreeView::DoAddFolder(const wxTreeListItem& parent, const wxString& path)
{
    try {
        m_sftp->CreateDir(path);
        SFTPAttribute::Ptr_t attr = m_sftp->Stat(path);
        // Update the UI
        MyClientData *newCd = new MyClientData(path);
        newCd->SetIsFolder( true );
        newCd->SetInitialized(false);

        wxTreeListItem child = m_treeListCtrl->AppendItem(parent, newCd->GetFullName(), m_bmpLoader.GetMimeImageId(FileExtManager::TypeFolder), wxNOT_FOUND, newCd);
        m_treeListCtrl->SetItemText(child, 1, attr->GetTypeAsString());
        m_treeListCtrl->SetItemText(child, 2, wxString() << attr->GetSize());
        m_treeListCtrl->AppendItem(child, "<dummy>");
        m_treeListCtrl->SetSortColumn(0);
        return child;

    } catch (clException &e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR|wxOK|wxCENTER);
    }
    return wxTreeListItem();
}
