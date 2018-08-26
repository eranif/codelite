#ifndef DOCKERSETTINGSDLG_H
#define DOCKERSETTINGSDLG_H

#include "UI.h"

class DockerSettingsDlg : public DockerSettingsBaseDlg
{
public:
    DockerSettingsDlg(wxWindow* parent);
    virtual ~DockerSettingsDlg();

protected:
    virtual void OnOK(wxCommandEvent& event);
};
#endif // DOCKERSETTINGSDLG_H
