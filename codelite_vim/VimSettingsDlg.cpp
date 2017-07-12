#include "VimSettingsDlg.h"
#include "VimSettings.h"

VimSettingsDlg::VimSettingsDlg(wxWindow* parent)
    : VimSettingsDlgBase(parent)
{
    VimSettings settings;
    settings.Load();
    m_checkBoxEnabled->SetValue(settings.IsEnabled());
}

VimSettingsDlg::~VimSettingsDlg()
{
}

