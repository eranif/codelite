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
    DoUpdateProjectFolder();
}

NewPHPProjectDlg::~NewPHPProjectDlg() { WindowAttrManager::Save(this, "NewPHPProjectDlg"); }

void NewPHPProjectDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->GetValue().IsEmpty() && wxFileName::Exists(m_dirPickerPath->GetPath()));
}

PHPProject::CreateData NewPHPProjectDlg::GetCreateData()
{
    PHPConfigurationData conf;
    PHPProject::CreateData cd;
    conf.Load();
    cd.importFilesUnderPath = true;
    cd.name = m_textCtrlName->GetValue();
    cd.phpExe = conf.GetPhpExe();
    cd.path = wxFileName(m_textCtrlPreview->GetValue()).GetPath();
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

void NewPHPProjectDlg::OnCreateUnderSeparateFolderUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->IsEmpty());
}

void NewPHPProjectDlg::OnOK(wxCommandEvent& event)
{
    wxFileName projectFilePath(m_textCtrlPreview->GetValue());
    wxString newpath = projectFilePath.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
    const PHPProject::Map_t& projects = PHPWorkspace::Get()->GetProjects();
    PHPProject::Map_t::const_iterator iter = projects.begin();
    for(; iter != projects.end(); ++iter) {
        if(newpath.StartsWith(iter->second->GetFilename().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME))) {
            // The new path is a sub folder of a project
            wxString message;
            message << _("Unable to create a project at the selected path. ") << _("Path '") << newpath
                    << _("' is already part of project '") << iter->second->GetName() << "'";
            m_infobar->ShowMessage(message, wxICON_WARNING);
            return;
            
        } else if(iter->second->GetFilename().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).StartsWith(newpath)) {
            // The new project is a parent of an existing project
            wxString message;
            message << _("Unable to create a project at the selected path. ") << _("Project '")
                    << iter->second->GetName() << _("' is located under this path");
            m_infobar->ShowMessage(message, wxICON_WARNING);
            return;
        }
    }
    
    EndModal(wxID_OK);
}
