#include "SFTPBrowserDlg.h"
#include "sftp_settings.h"
#include "ssh_account_info.h"
#include <wx/msgdlg.h>
#include "windowattrmanager.h"

// ================================================================================
// ================================================================================

class SFTPBrowserEntryClientData : public wxClientData
{
    SFTPAttribute::Ptr_t m_attribute;
    wxString             m_fullpath;
public:
    SFTPBrowserEntryClientData(SFTPAttribute::Ptr_t attr, const wxString &fullpath)
        : m_attribute(attr)
        , m_fullpath(fullpath)
    {
        wxFileName fn;
        if ( m_attribute->IsFolder() ) {
            fn = wxFileName(fullpath, "");
            fn.Normalize();
            m_fullpath = fn.GetPath(false, wxPATH_UNIX);
        } else {
            fn = wxFileName(fullpath);
            fn.Normalize();
            m_fullpath = fn.GetFullPath(wxPATH_UNIX);
        }
    }

    virtual ~SFTPBrowserEntryClientData()
    {}

    void SetAttribute(const SFTPAttribute::Ptr_t& attribute) {
        this->m_attribute = attribute;
    }
    void SetFullpath(const wxString& fullpath) {
        this->m_fullpath = fullpath;
    }
    const SFTPAttribute::Ptr_t& GetAttribute() const {
        return m_attribute;
    }
    const wxString& GetFullpath() const {
        return m_fullpath;
    }
};

// ================================================================================
// ================================================================================

SFTPBrowserDlg::SFTPBrowserDlg(wxWindow* parent)
    : SFTPBrowserBaseDlg(parent)
    , m_sftp(NULL)
{
    BitmapLoader bl;
    m_bitmaps = bl.MakeStandardMimeMap();

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
    WindowAttrManager::Load(this, "SFTPBrowserDlg", NULL);
}

SFTPBrowserDlg::~SFTPBrowserDlg()
{
    WindowAttrManager::Save(this, "SFTPBrowserDlg", NULL);
}

void SFTPBrowserDlg::OnRefresh(wxCommandEvent& event)
{
    DoCloseSession();
    wxString accountName = m_choiceAccount->GetStringSelection();

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

        DoDisplayEntriesForSelectedPath();

    } catch (clException &e) {
        ::wxMessageBox(e.What(), "codelite", wxICON_ERROR|wxOK, this);
        DoCloseSession();
    }
}

void SFTPBrowserDlg::OnRefreshUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlRemoteFolder->IsEmpty() );
}

void SFTPBrowserDlg::DoDisplayEntriesForSelectedPath()
{

    try {
        SFTPAttribute::List_t attributes = m_sftp->List( m_textCtrlRemoteFolder->GetValue() );
        SFTPAttribute::List_t::iterator iter = attributes.begin();
        for( ; iter != attributes.end(); ++iter ) {

            // Set the columns Name (icontext) | Type (text) | Size (text)
            wxVector<wxVariant> cols;
            cols.push_back( SFTPTreeModel::CreateIconTextVariant((*iter)->GetName(), (*iter)->IsFolder() ? m_bitmaps[FileExtManager::TypeFolder] : m_bitmaps[FileExtManager::TypeText]) );
            cols.push_back( (*iter)->GetTypeAsString() );
            cols.push_back( wxString() << (*iter)->GetSize() );
            
            SFTPBrowserEntryClientData* cd = new SFTPBrowserEntryClientData((*iter), wxString() << m_textCtrlRemoteFolder->GetValue() << "/" << (*iter)->GetName());
            m_dataviewModel->AppendItem(wxDataViewItem(0), cols, cd);
        }

    } catch (clException &e) {
        ::wxMessageBox(e.What(), "SFTP", wxICON_ERROR|wxOK);
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
    SFTPBrowserEntryClientData* cd = dynamic_cast<SFTPBrowserEntryClientData*>(m_dataviewModel->GetClientObject(event.GetItem()));
    if ( cd && cd->GetAttribute()->IsFolder() ) {
        m_textCtrlRemoteFolder->ChangeValue(cd->GetFullpath());
        m_dataviewModel->Clear();
        DoDisplayEntriesForSelectedPath();
    }
}
