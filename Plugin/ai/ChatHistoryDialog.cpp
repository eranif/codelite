#include "ChatHistoryDialog.hpp"

#include "ai/LLMManager.hpp"

namespace
{
/**
 * @brief Creates a truncated label string with ellipsis if the input text exceeds a specified size.
 *
 * If the input text is longer than or equal to the specified size, this function
 * truncates it and appends "..." to indicate truncation. Otherwise, the original
 * text is returned unchanged.
 *
 * @param text The input string to be processed into a label.
 * @param size The maximum length of the resulting label (default is 100). Must be
 *             at least 3 to accommodate the ellipsis when truncation occurs.
 *
 * @return wxString The resulting label, either truncated with "..." or the original text.
 */
wxString CreateLabel(const wxString& text, size_t size = 100)
{
    if (text.size() >= size) {
        return text.Mid(0, size - 3) << "...";
    }
    return text;
}
} // namespace

ChatHistoryDialog::ChatHistoryDialog(wxWindow* parent)
    : ChatHistoryDialogBase(parent)
{
    auto prompts = llm::Manager::GetInstance().GetConfig().GetHistory();
    for (auto& prompt : prompts) {
        prompt.Trim().Trim(false);
        wxVector<wxVariant> cols;
        // Ensure that each line in the table contains a single line (not multi-line)
        // as multi-line entries may not render correctly on some platforms.
        cols.push_back(CreateLabel(prompt));
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
    for (const auto& item : items) {
        lines.push_back(m_dvListCtrlPrompts->ItemToRow(item));
    }

    // Sort in descending order
    std::sort(lines.begin(), lines.end(), std::greater<size_t>());

    for (auto line : lines) {
        m_dvListCtrlPrompts->DeleteItem(line);
    }

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

    auto cd = reinterpret_cast<wxString*>(m_dvListCtrlPrompts->GetItemData(item));
    if (!cd) {
        clERROR() << "History entry does not have client data associated with it." << endl;
        EndModal(wxID_CANCEL);
        return;
    }

    SetSelectedPrompt(*cd);
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
