#ifndef DOCKERFILESETTINGSDLG_H
#define DOCKERFILESETTINGSDLG_H

#include "UI.h"
#include "clDockerBuildableFile.h"
#include "clDockerWorkspaceSettings.h"

class DockerfileSettingsDlg : public DockerfileSettingsDlgBase
{
    clDockerBuildableFile::Ptr_t m_info;

public:
    DockerfileSettingsDlg(wxWindow* parent, clDockerBuildableFile::Ptr_t info);
    virtual ~DockerfileSettingsDlg() = default;

protected:
    virtual void OnOK(wxCommandEvent& event);
};
#endif // DOCKERFILESETTINGSDLG_H
