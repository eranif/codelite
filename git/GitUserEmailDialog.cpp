#include "GitUserEmailDialog.h"

GitUserEmailDialog::GitUserEmailDialog(wxWindow* parent)
    : GitUserEmailDialogBase(parent)
{
}

GitUserEmailDialog::~GitUserEmailDialog() {}

void GitUserEmailDialog::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlEmail->IsEmpty() && !m_textCtrlName->IsEmpty());
}
