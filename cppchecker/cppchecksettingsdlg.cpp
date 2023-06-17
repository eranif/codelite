#include "cppchecksettingsdlg.h"

#include "cl_config.h"
#include "globals.h"

CppCheckSettingsDialog::CppCheckSettingsDialog(wxWindow* parent)
    : CppCheckSettingsDialogBase(parent)
{
    m_stcCommand->SetText(clConfig::Get().Read("cppcheck/command", CPPCHECK_DEFAULT_COMMAND));
    ::clSetDialogBestSizeAndPosition(this);
    m_stcCommand->CallAfter(&wxStyledTextCtrl::SetFocus);
}

CppCheckSettingsDialog::~CppCheckSettingsDialog()
{
    clConfig::Get().Write("cppcheck/command", m_stcCommand->GetText());
}
