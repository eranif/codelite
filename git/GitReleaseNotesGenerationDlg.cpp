#include "GitReleaseNotesGenerationDlg.h"

#include "ai/LLMManager.hpp"

GitReleaseNotesGenerationDlg::GitReleaseNotesGenerationDlg(wxWindow* parent)
    : GitReleaseNotesGenerationBaseDlg(parent)
{
    auto models = llm::Manager::GetInstance().GetModels();
    m_choiceModels->Append(models);

    if (!models.empty()) {
        auto active_model = llm::Manager::GetInstance().GetConfig().GetSelectedModel();
        int where = m_choiceModels->FindString(active_model);
        m_choiceModels->SetSelection(where == wxNOT_FOUND ? 0 : where);
    }

    GetSizer()->Fit(this);
    m_textCtrlFirstCommit->CallAfter(&wxTextCtrl::SetFocus);
    CenterOnParent();
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
