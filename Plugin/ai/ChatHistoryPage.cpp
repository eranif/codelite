#include "ChatHistoryPage.hpp"

#include "ChatHistoryDialog.hpp"
#include "ai/LLMManager.hpp"

#include <vector>

ChatHistoryPage::ChatHistoryPage(wxChoicebook* parent, const wxString& endpoint)
    : ChatHistoryPageBase(parent)
    , m_endpoint{endpoint}
{
    auto entries = llm::Manager::GetInstance().GetHistoryStore().List(endpoint);
    for (const auto& entry : entries) {
        m_chats.insert({entry.label, entry});
    }

    for (const auto& [label, _] : m_chats) {
        wxVector<wxVariant> cols;
        cols.push_back(label);
        m_dvListCtrlPrompts->AppendItem(cols);
    }
}

ChatHistoryPage::~ChatHistoryPage() {}

void ChatHistoryPage::Clear()
{
    m_dvListCtrlPrompts->DeleteAllItems();
    m_chats.clear();
    llm::Manager::GetInstance().GetHistoryStore().DeleteAll(m_endpoint);
}

void ChatHistoryPage::DeleteSelections()
{
    wxDataViewItemArray items;
    m_dvListCtrlPrompts->GetSelections(items);
    if (items.size() != 1) {
        return;
    }

    auto item = items[0];
    size_t row = m_dvListCtrlPrompts->ItemToRow(item);
    wxString label = m_dvListCtrlPrompts->GetTextValue(row, 0);
    m_dvListCtrlPrompts->DeleteItem(row);

    // Update the history
    auto iter = m_chats.find(label);
    if (iter != m_chats.end()) {
        llm::Manager::GetInstance().GetHistoryStore().Delete(m_endpoint, iter->second);
        m_chats.erase(iter);
    }
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
