#include "phprefactoringdlg.h"
#include "phprefactoringoptions.h"
#include "windowattrmanager.h"

PHPRefactoringDlg::PHPRefactoringDlg(wxWindow* parent)
    : PHPRefactoringBaseDlg(parent)
{
    // center the dialog
    InitDialog();
    SetName("PHPRefactoringDlg");
    WindowAttrManager::Load(this);
    CentreOnParent();
}

PHPRefactoringDlg::~PHPRefactoringDlg() {}

void PHPRefactoringDlg::InitDialog()
{
    PHPRefactoringOptions settings;
    settings.Load();

    m_filePickerPhprefactoringPhar->SetPath(settings.GetPhprefactoringPhar());
    m_checkBoxSkipPreview->SetValue(settings.GetSkipPreview());
}
