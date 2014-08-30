#include "new_php_workspace_dlg.h"
#include <windowattrmanager.h>
#include "php_strings.h"

NewPHPWorkspaceDlg::NewPHPWorkspaceDlg( wxWindow* parent )
    : NewPHPWorkspaceBaseDlg( parent )
{
    WindowAttrManager::Load(this, wxT("NewPHPWorkspaceDlg"), NULL);
}

NewPHPWorkspaceDlg::~NewPHPWorkspaceDlg()
{
    WindowAttrManager::Save(this, wxT("NewPHPWorkspaceDlg"), NULL);
}

wxString NewPHPWorkspaceDlg::GetWorkspacePath() const
{
    if ( m_textCtrlPath->IsEmpty() || m_textCtrlName->IsEmpty() ) {
        return "";
    }
    
    wxFileName filepath(m_textCtrlPath->GetValue(), m_textCtrlName->GetValue());
    if ( m_checkBoxCreateInSeparateDir->IsChecked() ) {
        filepath.AppendDir( filepath.GetName() );
    }
    filepath.SetExt(PHPStrings::PHP_WORKSPACE_EXT);
    return filepath.GetFullPath();
}

void NewPHPWorkspaceDlg::OnNameUpdated(wxCommandEvent& event)
{
    event.Skip();
    m_textCtrlPreview->ChangeValue( GetWorkspacePath() );
}

void NewPHPWorkspaceDlg::OnOK(wxCommandEvent& event)
{
    wxFileName fn ( GetWorkspacePath() );
    wxLogNull noLog;
    wxMkdir( fn.GetPath() );
    EndModal(wxID_OK);
}
void NewPHPWorkspaceDlg::OnCheckMakeSeparateDir(wxCommandEvent& event)
{
    m_textCtrlPreview->ChangeValue( GetWorkspacePath() );
}

void NewPHPWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlPath->IsEmpty() && !m_textCtrlName->GetValue().IsEmpty() );
}

void NewPHPWorkspaceDlg::OnBrowse(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("select a folder"));
    if ( !path.IsEmpty() ) {
        m_textCtrlPath->ChangeValue( path );
        m_textCtrlPreview->ChangeValue( GetWorkspacePath() );
    }
}
