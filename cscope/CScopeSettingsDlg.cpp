#include "CScopeSettingsDlg.h"
#include "windowattrmanager.h"
#include "csscopeconfdata.h"
#include "editor_config.h"

CScopeSettingsDlg::CScopeSettingsDlg(wxWindow* parent)
    : CScopeSettingsDlgBase(parent)
{
    CScopeConfData settings;
    EditorConfigST::Get()->ReadObject("CscopeSettings", &settings);
    
    m_filePickerCScopeExe->SetPath(settings.GetCscopeExe());
    WindowAttrManager::Load(this, "CScopeSettingsDlg");
}

CScopeSettingsDlg::~CScopeSettingsDlg()
{
    WindowAttrManager::Save(this, "CScopeSettingsDlg");
}

