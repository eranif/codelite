#include "GitGetTwoFieldsDlg.h"

GitGetTwoFieldsDlg::GitGetTwoFieldsDlg(wxWindow* parent)
    : GitGetTwoCommitsBaseDlg(parent)
{
}

GitGetTwoFieldsDlg::~GitGetTwoFieldsDlg() {}

void GitGetTwoFieldsDlg::OnButtonokUpdateUi(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlFirstCommit->GetValue().IsEmpty() && !m_textCtrlSecondCommit->GetValue().IsEmpty());
}
