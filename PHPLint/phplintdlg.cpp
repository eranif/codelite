#include "phplintdlg.h"
#include "lintoptions.h"
#include "windowattrmanager.h"

PHPLintDlg::PHPLintDlg(wxWindow* parent)
    : PHPLintBaseDlg(parent)
{
    // center the dialog
    Centre();

    InitDialog();

    SetName("PHPLintDlg");
    WindowAttrManager::Load(this);
}

PHPLintDlg::~PHPLintDlg() {}

void PHPLintDlg::InitDialog()
{
    LintOptions settings;
    settings.Load();

    m_filePickerPhpcsPhar->SetPath(settings.GetPhpcsPhar());
    m_filePickerPhpmdPhar->SetPath(settings.GetPhpmdPhar());
    m_filePickerPhpmdRules->SetPath(settings.GetPhpmdRules());
    m_filePickerPhpstanPhar->SetPath(settings.GetPhpstanPhar());

    // General Options
    m_checkBoxLintOnLoad->SetValue(settings.IsLintOnFileLoad());
    m_checkBoxLintOnSave->SetValue(settings.IsLintOnFileSave());
}
