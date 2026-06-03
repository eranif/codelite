#ifndef EDITORCONFIGSETTINGSDLG_H
#define EDITORCONFIGSETTINGSDLG_H
#include "EditorConfigUI.hpp"

class EditorConfigSettingsDlg : public EditorConfigSettingsDlgBase
{
public:
    EditorConfigSettingsDlg(wxWindow* parent);
    virtual ~EditorConfigSettingsDlg() = default;

    bool IsPluginEnabled() const { return m_checkBoxEnable->IsChecked(); }
};
#endif // EDITORCONFIGSETTINGSDLG_H
