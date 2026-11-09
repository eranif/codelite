#pragma once
#include "EditorConfigUI.hpp"

class EditorConfigSettingsDlg : public EditorConfigSettingsDlgBase
{
public:
    EditorConfigSettingsDlg(wxWindow* parent);
    virtual ~EditorConfigSettingsDlg() = default;

    bool IsPluginEnabled() const { return m_checkBoxEnable->IsChecked(); }
};
