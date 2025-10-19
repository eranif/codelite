#include "GitReleaseNotesGenerationDlg.h"

#include "ai/LLMManager.hpp"

GitReleaseNotesGenerationDlg::GitReleaseNotesGenerationDlg(wxWindow* parent)
    : GitReleaseNotesGenerationBaseDlg(parent)
{
    UpdateEndpoints();

    GetSizer()->Fit(this);
    m_textCtrlFirstCommit->CallAfter(&wxTextCtrl::SetFocus);
    CenterOnParent();
}

GitReleaseNotesGenerationDlg::~GitReleaseNotesGenerationDlg() {}

void GitReleaseNotesGenerationDlg::OnButtonokUpdateUi(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlFirstCommit->GetValue().IsEmpty() && !m_textCtrlSecondCommit->GetValue().IsEmpty());
}

void GitReleaseNotesGenerationDlg::OnChoiceProviders(wxCommandEvent& event)
{
    wxUnusedVar(event);
    llm::Manager::GetInstance().SetActiveEndpoint(m_choiceProviders->GetStringSelection());
    CallAfter(&GitReleaseNotesGenerationDlg::UpdateEndpoints);
}

void GitReleaseNotesGenerationDlg::OnChoiceProvidersUpdateUI(wxUpdateUIEvent& event) {}

void GitReleaseNotesGenerationDlg::UpdateEndpoints()
{
    auto endpoints = llm::Manager::GetInstance().ListEndpoints();
    m_choiceProviders->Clear();
    m_choiceProviders->Append(endpoints);

    if (!endpoints.empty()) {
        auto active_endpoint = llm::Manager::GetInstance().GetActiveEndpoint();
        if (active_endpoint.has_value()) {
            int where = m_choiceProviders->FindString(active_endpoint.value());
            m_choiceProviders->SetSelection(where == wxNOT_FOUND ? 0 : where);
        }
    }
}