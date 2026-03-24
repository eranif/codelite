#include "ChatHistoryPage.hpp"

#include "ChatHistoryDialog.hpp"
#include "ai/LLMManager.hpp"

#include <vector>

ChatHistoryPage::ChatHistoryPage(wxChoicebook* parent, ChatHistoryDialog* dlg, const wxString& endpoint)
    : ChatHistoryPageBase(parent)
    , m_endpoint{endpoint}
    , m_dialog{dlg}
{
    auto labels = llm::Manager::GetInstance().GetHistoryStore().List(endpoint);
    for (const auto& label : labels) {
        wxVector<wxVariant> cols;
        cols.push_back(label);
        m_dvListCtrlPrompts->AppendItem(cols);
    }
}

ChatHistoryPage::~ChatHistoryPage() {}

void ChatHistoryPage::Clear()
{
    m_dvListCtrlPrompts->DeleteAllItems();
    llm::Manager::GetInstance().GetHistoryStore().DeleteAll(m_endpoint);
}

void ChatHistoryPage::OnItemActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    m_dialog->CallAfter(&ChatHistoryDialog::DoItemSelected);
}

void ChatHistoryPage::DeleteSelections()
{
    wxDataViewItemArray items;
    m_dvListCtrlPrompts->GetSelections(items);
    if (items.empty()) {
        return;
    }

    wxArrayString labels;
    std::vector<size_t> lines;
    lines.reserve(items.size());
    for (const auto& item : items) {
        size_t row = m_dvListCtrlPrompts->ItemToRow(item);
        labels.push_back(m_dvListCtrlPrompts->GetTextValue(row, 0));
        lines.push_back(row);
    }

    // Sort in descending order
    std::sort(lines.begin(), lines.end(), std::greater<size_t>());

    for (auto line : lines) {
        m_dvListCtrlPrompts->DeleteItem(line);
    }

    // Update the history.
    llm::Manager::GetInstance().GetHistoryStore().DeleteMulti(m_endpoint, labels);
}

std::optional<wxString> ChatHistoryPage::GetSelection() const
{
    wxDataViewItemArray sels;
    m_dvListCtrlPrompts->GetSelections(sels);

    if (sels.size() != 1) {
        return std::nullopt;
    }

    size_t row = m_dvListCtrlPrompts->ItemToRow(sels[0]);
    return m_dvListCtrlPrompts->GetTextValue(row, 0);
}
