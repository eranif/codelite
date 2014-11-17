#include "PHPProjectSetupDlg.h"
#include "windowattrmanager.h"
#include "php_configuration_data.h"

PHPProjectSetupDlg::PHPProjectSetupDlg(wxWindow* parent)
    : PHPProjectSetupDlgBase(parent)
{
    PHPConfigurationData conf;
    conf.Load();
    m_filePickerPhpExe->SetPath(conf.GetPhpExe());
    WindowAttrManager::Load(this, "PHPProjectSetupDlgBase");
}

PHPProjectSetupDlg::~PHPProjectSetupDlg()
{
    WindowAttrManager::Save(this, "PHPProjectSetupDlgBase");
}

