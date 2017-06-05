#include "phprefactoringdlg.h"
#include "phprefactoringoptions.h"
#include "windowattrmanager.h"

PHPRefactoringDlg::PHPRefactoringDlg(wxWindow* parent)
    : PHPRefactoringBaseDlg(parent)
{
    // center the dialog
    Centre();

    InitDialog();

    SetName("PHPRefactoringDlg");
    WindowAttrManager::Load(this);
}

PHPRefactoringDlg::~PHPRefactoringDlg()
{
}

void PHPRefactoringDlg::InitDialog()
{
    PHPRefactoringOptions settings;
    settings.Load();

    m_filePickerPhprefactoringPhar->SetPath(settings.GetPhprefactoringPhar());
}
