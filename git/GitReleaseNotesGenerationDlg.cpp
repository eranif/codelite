#include "GitReleaseNotesGenerationDlg.h"

#include "ai/LLMManager.hpp"

GitReleaseNotesGenerationDlg::GitReleaseNotesGenerationDlg(wxWindow* parent)
    : GitReleaseNotesGenerationBaseDlg(parent)
{
    m_choiceModels->Append(llm::Manager::GetInstance().GetModels());
    m_choiceModels->SetStringSelection(llm::Manager::GetInstance().GetActiveModel());
    GetSizer()->Fit(this);
    m_textCtrlFirstCommit->CallAfter(&wxTextCtrl::SetFocus);
}

GitReleaseNotesGenerationDlg::~GitReleaseNotesGenerationDlg() {}

void GitReleaseNotesGenerationDlg::OnButtonokUpdateUi(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlFirstCommit->GetValue().IsEmpty() && !m_textCtrlSecondCommit->GetValue().IsEmpty());
}

void GitReleaseNotesGenerationDlg::OnChoicemodelsUpdateUi(wxUpdateUIEvent& event)
{
    event.Enable(m_choiceModels->GetCount() > 1);
}
