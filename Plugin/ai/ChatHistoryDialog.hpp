#pragma once

#include "UI.hpp"
#include "ai/ChatHistoryPage.hpp"
#include "ai/HistoryManager.hpp"
#include "codelite_exports.h"

#include <optional>

class WXDLLIMPEXP_SDK ChatHistoryDialog : public ChatHistoryDialogBase
{
public:
    ChatHistoryDialog(wxWindow* parent);
    ~ChatHistoryDialog() override;
    inline std::optional<llm::Conversation> GetSelectedConversation() const { return m_selectedConversation; }

protected:
    void OnInsert(wxCommandEvent& event) override;
    void OnInsertUI(wxUpdateUIEvent& event) override;
    void OnClear(wxCommandEvent& event) override;
    void OnClearUI(wxUpdateUIEvent& event) override;
    void OnDelete(wxCommandEvent& event) override;
    void OnDeleteUI(wxUpdateUIEvent& event) override;
    void DoItemSelected();

    ChatHistoryPage* GetActivePage();

private:
    std::optional<llm::Conversation> m_selectedConversation{std::nullopt};
    friend class ChatHistoryPage;
};
