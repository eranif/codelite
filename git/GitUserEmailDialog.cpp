#include "GitUserEmailDialog.h"

GitUserEmailDialog::GitUserEmailDialog(wxWindow* parent)
    : GitUserEmailDialogBase(parent)
{
}

void GitUserEmailDialog::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlEmail->IsEmpty() && !m_textCtrlName->IsEmpty());
}
