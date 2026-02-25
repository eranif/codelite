#include "ConfirmDialog.hpp"

ConfirmDialog::ConfirmDialog(wxWindow* parent)
    : ConfirmDialogBase(parent)
{
    if (GetParent()) {
        CenterOnParent();
    }
}

ConfirmDialog::~ConfirmDialog() {}

void ConfirmDialog::OnAllow(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_answer = llm::UserAnswer::kYes;
    EndModal(wxID_OK);
}

void ConfirmDialog::OnDontAllow(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_answer = llm::UserAnswer::kNo;
    EndModal(wxID_CANCEL);
}

void ConfirmDialog::OnTrust(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_answer = llm::UserAnswer::kTrust;
    EndModal(wxID_OK);
}
