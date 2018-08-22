#include "DockerfileSettingsDlg.h"

DockerfileSettingsDlg::DockerfileSettingsDlg(wxWindow* parent, clDockerfile& info)
    : DockerfileSettingsDlgBase(parent)
    , m_info(info)
{
    m_textCtrlBuildOptions->ChangeValue(m_info.GetBuildOptions());
    m_textCtrlRunOptions->ChangeValue(m_info.GetRunOptions());
    CentreOnParent();
}

DockerfileSettingsDlg::~DockerfileSettingsDlg() {}

void DockerfileSettingsDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    m_info.SetBuildOptions(m_textCtrlBuildOptions->GetValue());
    m_info.SetRunOptions(m_textCtrlRunOptions->GetValue());
}
