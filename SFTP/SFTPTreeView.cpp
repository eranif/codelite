#include "SFTPTreeView.h"
#include "SSHAccountManagerDlg.h"
#include "sftp_settings.h"
#include <wx/msgdlg.h>
#include "ssh_account_info.h"
#include "bitmap_loader.h"
#include "macros.h"
#include "sftp_worker_thread.h"
#include "sftp.h"

class MyClientData : public wxClientData
{
    wxString m_path;
    bool     m_initialized;
    bool     m_isFolder;

public:
    MyClientData(const wxString &path) 
        : m_path(path)
        , m_initialized(false)
        , m_isFolder(false) 
    {
        while (m_path.Replace("//", "/")) {}
        while (m_path.Replace("\\\\", "\\")) {}
    }
    
    virtual ~MyClientData() {}

    void SetInitialized(bool initialized) {
        this->m_initialized = initialized;
    }
    bool IsInitialized() const {
        return m_initialized;
    }
    void SetPath(const wxString& path) {
        this->m_path = path;
    }
    const wxString& GetPath() const {
        return m_path;
    }
    void SetIsFolder(bool isFolder) {
        this->m_isFolder = isFolder;
    }
    bool IsFolder() const {
        return m_isFolder;
    }
};

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
        ::wxMessageBox(wxString() << _("Could not find account: ") << accountName, "codelite", wxICON_ERROR|wxOK, this);
        return;
    }

    clSSH::Ptr_t ssh( new clSSH(m_account.GetHost(), m_account.GetUsername(), m_account.GetPassword(), m_account.GetPort()) );
    try {
        wxString message;
        ssh->Connect();
        if ( !ssh->AuthenticateServer( message ) ) {
            if ( ::wxMessageBox(message, "SSH", wxYES_NO|wxCENTER|wxICON_QUESTION, this) == wxYES ) {
                ssh->AcceptServerAuthentication();
            }
        }

        ssh->Login();
        m_sftp.reset( new clSFTP(ssh) );
        m_sftp->Initialize();

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
        ::wxMessageBox(e.What(), "codelite", wxICON_ERROR|wxOK, this);
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
        DoExpandItem(event.GetItem());

    } else {
        
        RemoteFileInfo remoteFile;
        remoteFile.SetAccount( m_account );
        remoteFile.SetRemoteFile( cd->GetPath() );

        SFTPThreadRequet* req = new SFTPThreadRequet(remoteFile);
        SFTPWorkerThread::Instance()->Add( req );
        
        m_plugin->AddRemoteFile(remoteFile);
    }
}

void SFTPTreeView::OnItemExpanding(wxTreeListEvent& event)
{
    wxUnusedVar(event);
    DoExpandItem(event.GetItem());
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

void SFTPTreeView::DoExpandItem(const wxTreeListItem& item)
{
    MyClientData *cd = GetItemData( item );
    CHECK_PTR_RET(cd);

    // already initialized this folder before?
    if ( cd->IsInitialized() ) {
        return;
    }
    cd->SetInitialized( true );

    // Remove the dummy item and replace it with real items
    wxTreeListItem dummyItem = m_treeListCtrl->GetFirstChild(item);
    m_treeListCtrl->DeleteItem( dummyItem );

    // get list of files and populate the tree
    SFTPAttribute::List_t attributes;
    attributes = m_sftp->List( cd->GetPath(), clSFTP::SFTP_BROWSE_FILES|clSFTP::SFTP_BROWSE_FOLDERS );
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
        path << cd->GetPath() << "/" << attr->GetName();

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
}

MyClientData* SFTPTreeView::GetItemData(const wxTreeListItem& item)
{
    CHECK_ITEM_RET_NULL(item);
    MyClientData *cd = dynamic_cast<MyClientData*>(m_treeListCtrl->GetItemData(item));
    return cd;
}

void SFTPTreeView::OnDisconnect(wxCommandEvent& event)
{
    DoCloseSession();
}

void SFTPTreeView::OnDisconnectUI(wxUpdateUIEvent& event)
{
    event.Enable( m_sftp );
}
