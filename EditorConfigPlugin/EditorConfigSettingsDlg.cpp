#include "EditorConfigSettingsDlg.h"

#include "EditorConfigSettings.h"
#include "cl_config.h"

EditorConfigSettingsDlg::EditorConfigSettingsDlg(wxWindow* parent)
    : EditorConfigSettingsDlgBase(parent)
{
    EditorConfigSettings settings;
    settings.Load();
    m_checkBoxEnable->SetValue(settings.IsEnabled());
}
