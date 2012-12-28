#include "gitCloneDlg.h"
#include "windowattrmanager.h"
#include <wx/uri.h>

gitCloneDlg::gitCloneDlg(wxWindow* parent)
    : gitCloneDlgBaseClass(parent)
{
    m_textCtrlURL->SetFocus();
    WindowAttrManager::Load(this, "gitCloneDlg", NULL);
}

gitCloneDlg::~gitCloneDlg()
{
    WindowAttrManager::Save(this, "gitCloneDlg", NULL);
}

void gitCloneDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlURL->IsEmpty() && !m_dirPickerTargetDir->GetPath().IsEmpty() );
}

void gitCloneDlg::OnUseCredentialsUI(wxUpdateUIEvent& event)
{
    event.Enable( m_checkBoxUseLogin->IsChecked() );
}

wxString gitCloneDlg::GetCloneURL() const
{
    wxString urlString = m_textCtrlURL->GetValue();
    wxURI uri( urlString.Trim().Trim(false) );
    wxString credentials;
    if ( m_checkBoxUseLogin->IsChecked() ) {
        credentials << m_textCtrlUsername->GetValue().Trim() << ":" << m_textCtrlPassword->GetValue().Trim() << "@";
    }
    
    wxString cloneURL;
    cloneURL << uri.GetScheme() << "://" << credentials << uri.GetServer() << uri.GetPath();
    return cloneURL;
}
