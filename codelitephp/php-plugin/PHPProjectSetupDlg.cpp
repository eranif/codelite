#include "PHPProjectSetupDlg.h"
#include "windowattrmanager.h"

PHPProjectSetupDlg::PHPProjectSetupDlg(wxWindow* parent)
    : PHPProjectSetupDlgBase(parent)
{
    WindowAttrManager::Load(this, "PHPProjectSetupDlgBase");
}

PHPProjectSetupDlg::~PHPProjectSetupDlg()
{
    WindowAttrManager::Save(this, "PHPProjectSetupDlgBase");
}

