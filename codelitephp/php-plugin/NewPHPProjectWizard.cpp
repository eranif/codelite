#include "NewPHPProjectWizard.h"
#include <wx/filename.h>
#include "php_workspace.h"
#include "php_project.h"
#include "php_configuration_data.h"
#include <wx/tokenzr.h>

NewPHPProjectWizard::NewPHPProjectWizard(wxWindow* parent)
    : NewPHPProjectWizardBase(parent)
    , m_nameModified(false)
{
    // Use the default PHP 
    PHPConfigurationData conf;
    conf.Load();
    m_filePickerPhpExe->SetPath(conf.GetPhpExe());
}

NewPHPProjectWizard::~NewPHPProjectWizard() {}

void NewPHPProjectWizard::OnFinish(wxWizardEvent& event) { event.Skip(); }

void NewPHPProjectWizard::OnPageChanging(wxWizardEvent& event)
{
    event.Skip();
    if(event.GetDirection() && event.GetPage() == m_wizardPageCreateMethod) {
        if(m_radioBoxCreateMethod->GetSelection() == 0) {
            // empty project
            m_checkBoxSeparateFolder->Show(true);
        } else {
            m_checkBoxSeparateFolder->Show(false);
        }
    } else if(event.GetDirection() && event.GetPage() == m_wizardPageProjectDetails) {
        wxFileName projectFilePath(m_textCtrlPreview->GetValue());
        wxString newpath = projectFilePath.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
        const PHPProject::Map_t& projects = PHPWorkspace::Get()->GetProjects();
        PHPProject::Map_t::const_iterator iter = projects.begin();
        for(; iter != projects.end(); ++iter) {
            if(newpath.StartsWith(iter->second->GetFilename().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME))) {
                // The new path is a sub folder of a project
                wxString message;
                message << _("Unable to create a project at the selected path\n") << _("Path '") << newpath
                        << _("' is already part of project '") << iter->second->GetName() << "'";
                ::wxMessageBox(message, "CodeLite", wxOK|wxICON_ERROR|wxCENTER, this);
                event.Skip(false);
                event.Veto();
                return;

            } else if(iter->second->GetFilename().GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).StartsWith(
                          newpath)) {
                // The new project is a parent of an existing project
                wxString message;
                message << _("Unable to create a project at the selected path\n") << _("Project '")
                        << iter->second->GetName() << _("' is located under this path");
                ::wxMessageBox(message, "CodeLite", wxOK|wxICON_ERROR|wxCENTER, this);
                event.Skip(false);
                event.Veto();
                return;
            }
        }
    }
}

void NewPHPProjectWizard::DoUpdateProjectFolder()
{
    // Build the file name
    wxFileName fn(m_dirPickerPath->GetPath(), "");
    if(m_checkBoxSeparateFolder->IsShown() && m_checkBoxSeparateFolder->IsChecked()) {
        fn.AppendDir(m_textCtrlName->GetValue());
    }
    fn.SetName(m_textCtrlName->GetValue());
    fn.SetExt("phprj");
    m_textCtrlPreview->ChangeValue(fn.GetFullPath());
}

void NewPHPProjectWizard::OnDirSelected(wxFileDirPickerEvent& event)
{
    if(!m_nameModified) {
        wxFileName path(event.GetPath(), "");
        m_textCtrlName->ChangeValue(path.GetDirs().Last());
    }
    DoUpdateProjectFolder();
}

void NewPHPProjectWizard::OnNameUpdated(wxCommandEvent& event)
{
    m_nameModified = true;
    DoUpdateProjectFolder();
}

PHPProject::CreateData NewPHPProjectWizard::GetCreateData()
{
    PHPConfigurationData conf;
    PHPProject::CreateData cd;
    conf.Load();
    cd.importFilesUnderPath = true;
    cd.name = m_textCtrlName->GetValue();
    cd.phpExe = m_filePickerPhpExe->GetPath().IsEmpty() ? conf.GetPhpExe() : m_filePickerPhpExe->GetPath();
    cd.path = wxFileName(m_textCtrlPreview->GetValue()).GetPath();
    cd.projectType = GetProjectType();
    cd.ccPaths = m_textCtrlCCPaths->GetValue();
    return cd;
}

void NewPHPProjectWizard::OnCheckSeparateFolder(wxCommandEvent& event)
{
    DoUpdateProjectFolder();
}
void NewPHPProjectWizard::OnBrowseForCCFolder(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector();
    if(path.IsEmpty()) return;
    
    wxString currentContent = m_textCtrlCCPaths->GetValue();
    wxArrayString paths = ::wxStringTokenize(currentContent, "\n", wxTOKEN_STRTOK);
    if(paths.Index(path) == wxNOT_FOUND) {
        paths.Add(path);
    }
    paths.Sort();
    currentContent = ::wxJoin(paths, '\n');
    m_textCtrlCCPaths->ChangeValue(currentContent);
}
