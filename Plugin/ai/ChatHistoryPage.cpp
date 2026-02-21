#include "ChatHistoryPage.hpp"

#include "ChatHistoryDialog.hpp"
#include "ai/LLMManager.hpp"

#include <vector>

ChatHistoryPage::ChatHistoryPage(wxChoicebook* parent, ChatHistoryDialog* dlg, const wxString& endpoint)
    : ChatHistoryPageBase(parent)
    , m_endpoint{endpoint}
    , m_dialog{dlg}
{
    m_coversations.clear();
    auto hist = llm::Manager::GetInstance().GetConfig().GetHistory(m_endpoint);
    for (const auto& conv : hist.conversations) {
        auto label_opt = conv.GetConversationLabel();
        if (!label_opt.has_value()) {
            continue;
        }
        wxString label = wxString::FromUTF8(label_opt.value());
        wxVector<wxVariant> cols;
        cols.push_back(label);
        auto p = std::make_shared<llm::Conversation>(conv);
        m_dvListCtrlPrompts->AppendItem(cols, reinterpret_cast<wxUIntPtr>(p.get()));
        m_coversations.push_back(p); // Keep a copy to keep the `p` pointer valid.
    }
}

ChatHistoryPage::~ChatHistoryPage() {}

void ChatHistoryPage::Clear()
{
    m_dvListCtrlPrompts->DeleteAllItems();
    m_coversations.clear();

    llm::Manager::GetInstance().GetConfig().SetHistory(m_endpoint, {});
    llm::Manager::GetInstance().GetConfig().Save();
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
    llm::Manager::GetInstance().GetConfig().SetHistory(m_endpoint, GetHistory());
    llm::Manager::GetInstance().GetConfig().Save();
}

llm::ChatHistory ChatHistoryPage::GetHistory() const
{
    llm::ChatHistory v;
    v.conversations.reserve(m_dvListCtrlPrompts->GetItemCount());
    for (auto i = 0; i < m_dvListCtrlPrompts->GetItemCount(); ++i) {
        v.conversations.push_back(
            *reinterpret_cast<llm::Conversation*>(m_dvListCtrlPrompts->GetItemData(m_dvListCtrlPrompts->RowToItem(i))));
    }
    return v;
}

std::optional<llm::Conversation> ChatHistoryPage::GetSelection() const
{
    wxDataViewItemArray sels;
    m_dvListCtrlPrompts->GetSelections(sels);

    if (sels.size() != 1) {
        return std::nullopt;
    }

    auto cd = reinterpret_cast<llm::Conversation*>(m_dvListCtrlPrompts->GetItemData(sels.Item(0)));
    if (!cd) {
        return std::nullopt;
    }
    return *cd;
}
