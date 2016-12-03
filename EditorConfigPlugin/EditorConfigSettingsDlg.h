#ifndef EDITORCONFIGSETTINGSDLG_H
#define EDITORCONFIGSETTINGSDLG_H
#include "EditorConfigUI.h"

class EditorConfigSettingsDlg : public EditorConfigSettingsDlgBase
{
public:
    EditorConfigSettingsDlg(wxWindow* parent);
    virtual ~EditorConfigSettingsDlg();
    
    bool IsPluginEnabled() const { return m_checkBoxEnable->IsChecked(); }
};
#endif // EDITORCONFIGSETTINGSDLG_H
