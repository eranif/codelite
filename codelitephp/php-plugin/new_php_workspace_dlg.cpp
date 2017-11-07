#include "new_php_workspace_dlg.h"
#include <windowattrmanager.h>
#include "php_strings.h"
#include "cl_standard_paths.h"

NewPHPWorkspaceDlg::NewPHPWorkspaceDlg(wxWindow* parent)
    : NewPHPWorkspaceBaseDlg(parent)
{
    m_textCtrlPath->ChangeValue(clStandardPaths::Get().GetDocumentsDir());
    CenterOnParent();
    SetName("NewPHPWorkspaceDlg");
    WindowAttrManager::Load(this);
}

NewPHPWorkspaceDlg::~NewPHPWorkspaceDlg() {}

wxString NewPHPWorkspaceDlg::GetWorkspacePath() const
{
    if(m_textCtrlPath->IsEmpty() || m_textCtrlName->IsEmpty()) {
        return "";
    }

    wxFileName filepath(m_textCtrlPath->GetValue(), m_textCtrlName->GetValue());
    filepath.SetExt(PHPStrings::PHP_WORKSPACE_EXT);
    return filepath.GetFullPath();
}

void NewPHPWorkspaceDlg::OnNameUpdated(wxCommandEvent& event)
{
    event.Skip();
    m_textCtrlPreview->ChangeValue(GetWorkspacePath());
}

void NewPHPWorkspaceDlg::OnOK(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void NewPHPWorkspaceDlg::OnCheckMakeSeparateDir(wxCommandEvent& event)
{
    m_textCtrlPreview->ChangeValue(GetWorkspacePath());
}

void NewPHPWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlPath->GetValue().IsEmpty() && !m_textCtrlName->GetValue().IsEmpty());
}

void NewPHPWorkspaceDlg::OnBrowse(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("select a folder"), m_textCtrlPath->GetValue());
    if(!path.IsEmpty()) {
        m_textCtrlPath->SetValue(path); // we want event here
        m_textCtrlPreview->ChangeValue(GetWorkspacePath());
    }
}

void NewPHPWorkspaceDlg::OnFolderSelected(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxFileName fn(m_textCtrlPath->GetValue());
    
    // Use the last folder path as the project name
    m_textCtrlName->ChangeValue(fn.GetName());
}
