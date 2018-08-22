#include "DockerSettingsDlg.h"
#include "clDockerSettings.h"

DockerSettingsDlg::DockerSettingsDlg(wxWindow* parent)
    : DockerSettingsBaseDlg(parent)
{
    clDockerSettings settings;
    settings.Load();
    m_filePickerDocker->SetPath(settings.GetDocker().GetFullPath());
    m_filePickerDockerCompose->SetPath(settings.GetDockerCompose().GetFullPath());
}

DockerSettingsDlg::~DockerSettingsDlg() {}

void DockerSettingsDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    clDockerSettings settings;
    settings.Load();
    settings.SetDocker(m_filePickerDocker->GetPath());
    settings.SetDockerCompose(m_filePickerDockerCompose->GetPath());
    settings.Save();
}
