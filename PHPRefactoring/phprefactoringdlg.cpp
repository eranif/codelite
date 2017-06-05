#include "phprefactoringdlg.h"
#include "phprefactoringoptions.h"

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
    PHPRefactoringOptions settings;
    settings.Load();

    m_filePickerPhprefactoringPhar->SetPath(settings.GetPhprefactoringPhar());
}
