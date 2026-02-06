#include "ChatHistoryDialog.hpp"

#include "ai/LLMManager.hpp"

ChatHistoryDialog::ChatHistoryDialog(wxWindow* parent)
    : ChatHistoryDialogBase(parent)
{
    auto history = llm::Manager::GetInstance().GetConfig().GetHistory();
    for (const auto& conversation : history) {
        if (conversation.empty()) {
            continue;
        }
        auto res = llm::Config::GetConversationLabel(conversation);
        if (!res.has_value()) {
            continue;
        }

        auto label = wxString::FromUTF8(res.value());
        wxVector<wxVariant> cols;
        cols.push_back(label);
        auto p = std::make_shared<llm::Conversation>(conversation);
        m_dvListCtrlPrompts->AppendItem(cols, reinterpret_cast<wxUIntPtr>(p.get()));
        m_coversations.push_back(p); // Keep a copy to keep the `p` pointer valid.
    }
    m_dvListCtrlPrompts->CallAfter(&wxDataViewListCtrl::SetFocus);
}

ChatHistoryDialog::~ChatHistoryDialog() {}

void ChatHistoryDialog::OnClear(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dvListCtrlPrompts->DeleteAllItems();

    // Update the history.
    llm::Manager::GetInstance().GetConfig().SetHistory({});
    llm::Manager::GetInstance().GetConfig().Save();
}

void ChatHistoryDialog::OnClearUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrlPrompts->GetItemCount() > 0); }
void ChatHistoryDialog::OnDelete(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrlPrompts->GetSelections(items);
    if (items.empty()) {
        return;
    }
    std::vector<size_t> lines;
    lines.reserve(items.size());
    for (const auto& item : items) {
        lines.push_back(m_dvListCtrlPrompts->ItemToRow(item));
    }

    // Sort in descending order
    std::sort(lines.begin(), lines.end(), std::greater<size_t>());

    for (auto line : lines) {
        m_dvListCtrlPrompts->DeleteItem(line);
    }

    // Update the history.
    llm::Manager::GetInstance().GetConfig().SetHistory(GetHistory());
    llm::Manager::GetInstance().GetConfig().Save();
}

void ChatHistoryDialog::OnDeleteUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlPrompts->GetSelectedItemsCount() > 0);
}

void ChatHistoryDialog::OnInsert(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrlPrompts->GetSelections(items);

    if (items.empty()) {
        return;
    }

    SetSelectionAndEndModal(items[0]);
}

void ChatHistoryDialog::OnInsertUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlPrompts->GetSelectedItemsCount() == 1);
}

void ChatHistoryDialog::OnItemActivated(wxDataViewEvent& event) { SetSelectionAndEndModal(event.GetItem()); }

void ChatHistoryDialog::SetSelectionAndEndModal(const wxDataViewItem& item)
{
    if (!item.IsOk()) {
        return;
    }

    auto cd = reinterpret_cast<llm::Conversation*>(m_dvListCtrlPrompts->GetItemData(item));
    if (!cd) {
        clERROR() << "History entry does not have client data associated with it." << endl;
        EndModal(wxID_CANCEL);
        return;
    }

    SetSelectedPrompt(*cd);
    EndModal(wxID_OK);
}

llm::ChatHistory ChatHistoryDialog::GetHistory() const
{
    llm::ChatHistory v;
    v.reserve(m_dvListCtrlPrompts->GetItemCount());
    for (auto i = 0; i < m_dvListCtrlPrompts->GetItemCount(); ++i) {
        v.push_back(
            *reinterpret_cast<llm::Conversation*>(m_dvListCtrlPrompts->GetItemData(m_dvListCtrlPrompts->RowToItem(i))));
    }
    return v;
}
