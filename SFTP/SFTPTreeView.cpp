#include "SFTPTreeView.h"
#include "SSHAccountManagerDlg.h"
#include "sftp_settings.h"
#include <wx/msgdlg.h>
#include "ssh_account_info.h"
#include "bitmap_loader.h"

class MyClientData : public wxClientData
{
    wxString m_folder;
    bool     m_initialized;
public:
    MyClientData(const wxString &folder) : m_folder(folder), m_initialized(false) {}
    virtual ~MyClientData() {}

    void SetInitialized(bool initialized) {
        this->m_initialized = initialized;
    }
    bool IsInitialized() const {
        return m_initialized;
    }
    void SetFolder(const wxString& folder) {
        this->m_folder = folder;
    }
    const wxString& GetFolder() const {
        return m_folder;
    }
};

SFTPTreeView::SFTPTreeView(wxWindow* parent)
    : SFTPTreeViewBase(parent)
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

    SSHAccountInfo account;
    if ( !settings.GetAccount( accountName, account) ) {
        ::wxMessageBox(wxString() << _("Could not find account: ") << accountName, "codelite", wxICON_ERROR|wxOK, this);
        return;
    }

    clSSH::Ptr_t ssh( new clSSH(account.GetHost(), account.GetUsername(), account.GetPassword(), account.GetPort()) );
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
}

void SFTPTreeView::OnItemExpanding(wxTreeListEvent& event)
{
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
    event.Enable( !m_choiceAccount->GetStringSelection().IsEmpty() );
}

void SFTPTreeView::DoExpandItem(const wxTreeListItem& item)
{
    MyClientData *cd = dynamic_cast<MyClientData*>(m_treeListCtrl->GetItemData(item));
    if ( !cd ) return;
    
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
    attributes = m_sftp->List( cd->GetFolder(), clSFTP::SFTP_BROWSE_FILES|clSFTP::SFTP_BROWSE_FOLDERS );
    SFTPAttribute::List_t::iterator iter = attributes.begin();
    for( ; iter != attributes.end(); ++iter ) {
        
    }
}
