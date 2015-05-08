#ifndef HELPPLUGINSETTINGSDLG_H
#define HELPPLUGINSETTINGSDLG_H
#include "HelpPluginUI.h"

class HelpPluginSettingsDlg : public HelpPluginSettingsDlgBase
{
    bool m_modified;
public:
    HelpPluginSettingsDlg(wxWindow* parent);
    virtual ~HelpPluginSettingsDlg();

protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnDocsetChanged(wxPropertyGridEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // HELPPLUGINSETTINGSDLG_H
