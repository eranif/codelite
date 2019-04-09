#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H
#include "wxcrafter.h"

class SettingsDlg : public SettingsDlgBase
{
public:
    SettingsDlg(wxWindow* parent);
    virtual ~SettingsDlg();
    
    void Load();
    void Save();
};
#endif // SETTINGSDLG_H
