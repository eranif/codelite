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
        , m_fullpath(fullpath) {
        wxFileName fn;
        if ( m_attribute->IsFolder() ) {
            fn = wxFileName(fullpath, "", wxPATH_UNIX);
            fn.Normalize();
            m_fullpath = fn.GetPath(false, wxPATH_UNIX);
        } else {
            fn = wxFileName(fullpath, wxPATH_UNIX);
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

SFTPBrowserDlg::SFTPBrowserDlg(wxWindow* parent, const wxString &title, const wxString& filter)
    : SFTPBrowserBaseDlg(parent)
    , m_sftp(NULL)
    , m_filter(filter)
{
    SetLabel( title );

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
        wxString folder = m_textCtrlRemoteFolder->GetValue();
        SFTPAttribute::List_t attributes = m_sftp->List( folder, false, m_filter );
        SFTPAttribute::List_t::iterator iter = attributes.begin();
        for( ; iter != attributes.end(); ++iter ) {

            // Set the columns Name (icontext) | Type (text) | Size (text)
            wxVector<wxVariant> cols;

            // determine the bitmap type
            wxBitmap bmp = m_bitmaps[FileExtManager::TypeText];
            wxString fullname;
            fullname << folder << "/" << (*iter)->GetName();

            if ( (*iter)->IsFolder() ) {
                bmp = m_bitmaps[FileExtManager::TypeFolder];
            } else {
                wxFileName fn(fullname);
                FileExtManager::FileType type = FileExtManager::GetType(fn.GetFullName());
                if ( m_bitmaps.count(type) ) {
                    bmp = m_bitmaps[type];
                }
            }

            cols.push_back( SFTPTreeModel::CreateIconTextVariant((*iter)->GetName(), bmp ));
            cols.push_back( (*iter)->GetTypeAsString() );
            cols.push_back( wxString() << (*iter)->GetSize() );

            SFTPBrowserEntryClientData* cd = new SFTPBrowserEntryClientData((*iter), fullname);
            m_dataviewModel->AppendItem(wxDataViewItem(0), cols, cd);
        }
        m_dataview->Refresh();

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
    if ( !m_sftp ) {
        DoCloseSession();
        return;
    }

    SFTPBrowserEntryClientData* cd = dynamic_cast<SFTPBrowserEntryClientData*>(m_dataviewModel->GetClientObject(event.GetItem()));
    if ( cd && cd->GetAttribute()->IsFolder() ) {
        m_textCtrlRemoteFolder->ChangeValue(cd->GetFullpath());
        m_dataviewModel->Clear();
        DoDisplayEntriesForSelectedPath();
    }
}

void SFTPBrowserDlg::OnTextEnter(wxCommandEvent& event)
{
    if ( !m_sftp ) {
        OnRefresh(event);
    } else {
        m_dataviewModel->Clear();
        DoDisplayEntriesForSelectedPath();
    }
}
void SFTPBrowserDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(true);
}

SFTPBrowserEntryClientData* SFTPBrowserDlg::DoGetItemData(const wxDataViewItem& item) const
{
    if ( !item.IsOk() ) {
        return NULL;
    }

    SFTPBrowserEntryClientData* cd = dynamic_cast<SFTPBrowserEntryClientData*>(m_dataviewModel->GetClientObject( item ));
    return cd;
}

wxString SFTPBrowserDlg::GetPath() const
{
    return m_textCtrlRemoteFolder->GetValue();
}

void SFTPBrowserDlg::OnItemSelected(wxDataViewEvent& event)
{
    SFTPBrowserEntryClientData* cd = DoGetItemData( m_dataview->GetSelection() );
    if ( cd ) {
        m_textCtrlRemoteFolder->ChangeValue( cd->GetFullpath() );
    }
}

wxString SFTPBrowserDlg::GetAccount() const
{
    return m_choiceAccount->GetStringSelection();
}
