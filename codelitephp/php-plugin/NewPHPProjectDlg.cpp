#include "NewPHPProjectDlg.h"
#include <wx/filename.h>
#include "windowattrmanager.h"
#include "php_configuration_data.h"
#include "php_project.h"
#include "php_workspace.h"

NewPHPProjectDlg::NewPHPProjectDlg(wxWindow* parent)
    : NewPHPProjectDlgBase(parent)
{
    WindowAttrManager::Load(this, "NewPHPProjectDlg");
    // By default set the workspace path
    m_dirPickerPath->SetPath(PHPWorkspace::Get()->GetFilename().GetPath());
    DoUpdateProjectFolder();
}

NewPHPProjectDlg::~NewPHPProjectDlg()
{
    WindowAttrManager::Save(this, "NewPHPProjectDlg");
}

void NewPHPProjectDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->GetValue().IsEmpty() && wxFileName::Exists(m_dirPickerPath->GetPath()));
}

PHPProject::CreateData NewPHPProjectDlg::GetCreateData()
{
    PHPConfigurationData conf;
    PHPProject::CreateData cd;
    conf.Load();
    cd.importFilesUnderPath = m_checkBoxImportFiles->IsChecked();
    cd.name = m_textCtrlName->GetValue();
    cd.phpExe = conf.GetPhpExe();
    cd.path = m_dirPickerPath->GetPath();
    return cd;
}
void NewPHPProjectDlg::OnNameUpdated(wxCommandEvent& event)
{
    event.Skip();
    DoUpdateProjectFolder();
}

void NewPHPProjectDlg::OnPathUpdated(wxFileDirPickerEvent& event)
{
    event.Skip();
    DoUpdateProjectFolder();
}

void NewPHPProjectDlg::DoUpdateProjectFolder()
{
    // Build the file name
    wxFileName fn(m_dirPickerPath->GetPath(), "");
    if(m_checkBoxSeparateFolder->IsChecked()) {
        fn.AppendDir(m_textCtrlName->GetValue());
    }
    fn.SetName(m_textCtrlName->GetValue());
    fn.SetExt("phprj");
    m_textCtrlPreview->ChangeValue(fn.GetFullPath());
}

void NewPHPProjectDlg::OnCreateUnderSeparateFolder(wxCommandEvent& event)
{
    event.Skip();
    DoUpdateProjectFolder();
}
