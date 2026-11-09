#pragma once

#include "UI.hpp"

class DockerSettingsDlg : public DockerSettingsBaseDlg
{
public:
    DockerSettingsDlg(wxWindow* parent);
    virtual ~DockerSettingsDlg() = default;

protected:
    virtual void OnOK(wxCommandEvent& event);
};
