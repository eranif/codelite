#ifndef ZNSETTINGSDLG_H
#define ZNSETTINGSDLG_H

#include "zoom_navigator.h"

extern const wxEventType wxEVT_ZN_SETTINGS_UPDATED;

class znSettingsDlg : public znSettingsDlgBase
{
public:
    znSettingsDlg(wxWindow* parent);
    virtual ~znSettingsDlg();
protected:
    virtual void OnOK(wxCommandEvent& event);
};
#endif // ZNSETTINGSDLG_H
