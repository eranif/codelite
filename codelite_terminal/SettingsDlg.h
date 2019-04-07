#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H
#include "wxcrafter.h"

class SettingsDlg : public SettingsDlgBase
{
public:
    SettingsDlg(wxWindow* parent);
    virtual ~SettingsDlg();
protected:
    virtual void OnBGColour(wxColourPickerEvent& event);
    virtual void OnFGColour(wxColourPickerEvent& event);
    virtual void OnFontSelected(wxFontPickerEvent& event);
};
#endif // SETTINGSDLG_H
