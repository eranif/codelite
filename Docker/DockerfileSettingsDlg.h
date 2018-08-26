#ifndef DOCKERFILESETTINGSDLG_H
#define DOCKERFILESETTINGSDLG_H
#include "UI.h"
#include "clDockerWorkspaceSettings.h"

class DockerfileSettingsDlg : public DockerfileSettingsDlgBase
{
    clDockerfile& m_info;

public:
    DockerfileSettingsDlg(wxWindow* parent, clDockerfile& info);
    virtual ~DockerfileSettingsDlg();

protected:
    virtual void OnOK(wxCommandEvent& event);
};
#endif // DOCKERFILESETTINGSDLG_H
