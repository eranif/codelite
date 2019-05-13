#include "EOSProjectWizard.h"
#include <wx/filename.h>
#include "CompilerLocatorEosCDT.h"
#include "build_settings_config.h"
#include "compiler.h"
#include "globals.h"

EOSProjectWizard::EOSProjectWizard(wxWindow* parent)
    : EOSProjectWizardBase(parent)
{
    m_dirPickerToolchainPath->SetPath(GetToolchainPath());
}

EOSProjectWizard::~EOSProjectWizard() {}

void EOSProjectWizard::OnOKUI(wxUpdateUIEvent& event) { event.Enable(!GetName().IsEmpty() && !GetPath().IsEmpty()); }

wxString EOSProjectWizard::GetName() const
{
    wxString name = m_textCtrlName->GetValue();
    name.Trim().Trim(false);
    return name;
}

wxString EOSProjectWizard::GetPath() const
{
    wxString path = m_dirPickerPath->GetPath();
    path.Trim().Trim(false);
    return path;
}

void EOSProjectWizard::OnNameUpdated(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_staticTextActualPath->SetLabel(GetProjectFile());
}

void EOSProjectWizard::OnPathChanged(wxFileDirPickerEvent& event)
{
    wxUnusedVar(event);
    m_staticTextActualPath->SetLabel(GetProjectFile());
}

wxString EOSProjectWizard::GetProjectFile() const
{
    wxFileName projectFile(GetPath(), GetName() + ".project");
    if(m_checkBoxSeparateDir->IsChecked()) { projectFile.AppendDir(GetName()); }
    return projectFile.GetFullPath();
}

void EOSProjectWizard::OnCreateInSeparateDir(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_staticTextActualPath->SetLabel(GetProjectFile());
}

wxString EOSProjectWizard::GetToolchainPath()
{
    CompilerPtr compiler = BuildSettingsConfigST::Get()->GetCompiler("eosio");
    if(!compiler) {
        // Try to locate the compiler
        CompilerLocatorEosCDT cdt;
        if(cdt.Locate()) { compiler = cdt.GetCompilers()[0]; }
    }

    if(!compiler) { return ""; }
    wxFileName cxx = compiler->GetTool("CXX");
    cxx = wxReadLink(cxx);
    if (cxx.GetDirCount()) { cxx.RemoveLastDir(); } // remove the "bin" folder
    return cxx.GetPath();
}

EOSProjectData EOSProjectWizard::GetData() const
{
    EOSProjectData d;
    d.SetName(GetName());
    d.SetPath(wxFileName(GetProjectFile()).GetPath());
    d.SetToolchainPath(m_dirPickerToolchainPath->GetPath());
    return d;
}
