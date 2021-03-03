#include "CopyCompilerSettingsDlg.h"
#include "workspace.h"

CopyCompilerSettingsDlg::CopyCompilerSettingsDlg(wxWindow* parent)
    : CopyCompilerSettingsDlgBase(parent)
{
    wxArrayString projects;
    clCxxWorkspaceST::Get()->GetProjectList(projects);

    projects.Sort([](const wxString& a, const wxString& b) -> int { return a.CmpNoCase(b); });
    m_choiceProjects->Append(projects);

    wxString savedProject, savedName;
    savedProject = clConfig::Get().Read("CopyCompilerSettingsDlg/Project", savedProject);
    savedName = clConfig::Get().Read("CopyCompilerSettingsDlg/Name", savedName);
    if(!projects.empty()) {
        int where = wxNOT_FOUND;
        if(!savedProject.empty()) {
            where = m_choiceConfigurations->FindString(savedProject);
        }

        m_choiceProjects->SetSelection(where == wxNOT_FOUND ? 0 : where);
        DoUpdateConfigurations(savedName);
    }
    GetSizer()->Fit(this);
}

CopyCompilerSettingsDlg::~CopyCompilerSettingsDlg()
{
    clConfig::Get().Write("CopyCompilerSettingsDlg/Project", m_choiceProjects->GetStringSelection());
    clConfig::Get().Write("CopyCompilerSettingsDlg/Name", m_choiceConfigurations->GetStringSelection());
}

void CopyCompilerSettingsDlg::DoUpdateConfigurations(const wxString& configToSelect)
{
    wxString selectedProject = m_choiceProjects->GetStringSelection();
    auto p = clCxxWorkspaceST::Get()->GetProject(selectedProject);
    CHECK_PTR_RET(p);

    wxArrayString configurations;
    ProjectSettingsCookie cookie;
    auto settings = p->GetSettings();
    auto config = settings->GetFirstBuildConfiguration(cookie);
    while(config) {
        configurations.Add(config->GetName());
        config = settings->GetNextBuildConfiguration(cookie);
    }

    configurations.Sort([](const wxString& a, const wxString& b) -> int { return a.CmpNoCase(b); });
    m_choiceConfigurations->Append(configurations);

    if(!configurations.empty()) {
        int where = wxNOT_FOUND;
        if(!configToSelect.empty()) {
            where = m_choiceConfigurations->FindString(configToSelect);
        }
        m_choiceConfigurations->SetSelection(where == wxNOT_FOUND ? 0 : where);
    }
}

void CopyCompilerSettingsDlg::GetCopyFrom(wxString& project, wxString& config)
{
    project = m_choiceProjects->GetStringSelection();
    config = m_choiceConfigurations->GetStringSelection();
}
