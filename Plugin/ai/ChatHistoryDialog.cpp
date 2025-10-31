#include "ChatHistoryDialog.hpp"

#include "ai/LLMManager.hpp"

ChatHistoryDialog::ChatHistoryDialog(wxWindow* parent) : ChatHistoryDialogBase(parent)
{
    auto prompts = llm::Manager::GetInstance().GetConfig().GetHistory();
    for (const auto& prompt : prompts) {
        wxVector<wxVariant> cols;
        cols.push_back(prompt);
        auto p = std::make_shared<wxString>(prompt);
        m_dvListCtrlPrompts->AppendItem(cols, reinterpret_cast<wxUIntPtr>(p.get()));
        m_prompts.push_back(p);
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
    for (const auto& item : items) { lines.push_back(m_dvListCtrlPrompts->ItemToRow(item)); }

    // Sort in descending order
    std::sort(lines.begin(), lines.end(), std::greater<size_t>());

    for (auto line : lines) { m_dvListCtrlPrompts->DeleteItem(line); }

    // Update the history.
    llm::Manager::GetInstance().GetConfig().SetHistory(GetPrompts());
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

    wxVariant value;
    m_dvListCtrlPrompts->GetValue(value, m_dvListCtrlPrompts->ItemToRow(item), 0);

    SetSelectedPrompt(value.GetString());
    EndModal(wxID_OK);
}

wxArrayString ChatHistoryDialog::GetPrompts() const
{
    wxArrayString prompts;
    prompts.reserve(m_dvListCtrlPrompts->GetItemCount());
    for (auto i = 0; i < m_dvListCtrlPrompts->GetItemCount(); ++i) {
        prompts.push_back(
            *reinterpret_cast<wxString*>(m_dvListCtrlPrompts->GetItemData(m_dvListCtrlPrompts->RowToItem(i))));
    }
    return prompts;
}
