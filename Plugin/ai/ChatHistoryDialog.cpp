#include "ChatHistoryDialog.hpp"

#include "ChatHistoryPage.hpp"
#include "ai/LLMManager.hpp"
#include "assistant/helpers.hpp"

ChatHistoryDialog::ChatHistoryDialog(wxWindow* parent)
    : ChatHistoryDialogBase(parent)
{
    auto active_endpoint = llm::Manager::GetInstance().GetActiveEndpoint();
    auto endpoints = llm::Manager::GetInstance().ListEndpoints();
    for (const auto& endpoint : endpoints) {
        ChatHistoryPage* page = new ChatHistoryPage(m_choicebook, this, endpoint);
        bool is_active_endpoint = active_endpoint.has_value() && active_endpoint.value() == endpoint;
        bool is_first = m_choicebook->GetPageCount() == 0;
        m_choicebook->AddPage(page, endpoint, is_active_endpoint || is_first);
    }
}

ChatHistoryDialog::~ChatHistoryDialog() {}

ChatHistoryPage* ChatHistoryDialog::GetActivePage()
{
    if (m_choicebook->GetSelection() == wxNOT_FOUND) {
        return nullptr;
    }
    return static_cast<ChatHistoryPage*>(m_choicebook->GetPage(m_choicebook->GetSelection()));
}

void ChatHistoryDialog::OnClear(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto page = GetActivePage();
    CHECK_PTR_RET(page);

    page->Clear();
}

void ChatHistoryDialog::OnClearUI(wxUpdateUIEvent& event)
{
    event.Enable(GetActivePage() && GetActivePage()->GetListView() && GetActivePage()->GetListView()->GetItemCount());
}

void ChatHistoryDialog::OnDelete(wxCommandEvent& event)
{
    auto page = GetActivePage();
    CHECK_PTR_RET(page);

    page->DeleteSelections();
}

void ChatHistoryDialog::OnDeleteUI(wxUpdateUIEvent& event)
{
    event.Enable(GetActivePage() && GetActivePage()->GetListView() &&
                 GetActivePage()->GetListView()->GetSelectedItemsCount());
}

void ChatHistoryDialog::OnInsert(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoItemSelected();
}

void ChatHistoryDialog::DoItemSelected()
{
    CHECK_PTR_RET(GetActivePage());

    auto s = GetActivePage()->GetSelection();
    if (s.has_value()) {
        SetSelectedPrompt(s.value());
        EndModal(wxID_OK);
    }
}

void ChatHistoryDialog::OnInsertUI(wxUpdateUIEvent& event)
{
    event.Enable(GetActivePage() && GetActivePage()->GetListView() &&
                 GetActivePage()->GetListView()->GetSelectedItemsCount() == 1);
}
