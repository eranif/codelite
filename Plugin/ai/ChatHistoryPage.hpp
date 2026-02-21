#pragma once

#include "UI.hpp"
#include "ai/Config.hpp"
#include "codelite_exports.h"

class ChatHistoryDialog;
class WXDLLIMPEXP_SDK ChatHistoryPage : public ChatHistoryPageBase
{
public:
    explicit ChatHistoryPage(wxChoicebook* parent, ChatHistoryDialog* dlg, const wxString& endpoint);
    ~ChatHistoryPage() override;

    wxDataViewListCtrl* GetListView() { return m_dvListCtrlPrompts; }
    void Clear();
    void DeleteSelections();
    llm::ChatHistory GetHistory() const;
    std::optional<llm::Conversation> GetSelection() const;

protected:
    void OnItemActivated(wxDataViewEvent& event) override;

private:
    wxString m_endpoint;
    std::vector<std::shared_ptr<llm::Conversation>> m_coversations;
    ChatHistoryDialog* m_dialog{nullptr};
};
