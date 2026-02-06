#pragma once

#include "UI.hpp"
#include "ai/Config.hpp"
#include "codelite_exports.h"

#include <vector>

class WXDLLIMPEXP_SDK ChatHistoryDialog : public ChatHistoryDialogBase
{
public:
    ChatHistoryDialog(wxWindow* parent);
    ~ChatHistoryDialog() override;

    inline void SetSelectedPrompt(const llm::Conversation& conversation)
    {
        this->m_selectedConversation = conversation;
    }
    inline const llm::Conversation& GetSelectedConversation() const { return m_selectedConversation; }

protected:
    void OnInsert(wxCommandEvent& event) override;
    void OnInsertUI(wxUpdateUIEvent& event) override;
    void OnItemActivated(wxDataViewEvent& event) override;
    void OnClear(wxCommandEvent& event) override;
    void OnClearUI(wxUpdateUIEvent& event) override;
    void OnDelete(wxCommandEvent& event) override;
    void OnDeleteUI(wxUpdateUIEvent& event) override;

    void SetSelectionAndEndModal(const wxDataViewItem& item);
    llm::ChatHistory GetHistory() const;

private:
    llm::Conversation m_selectedConversation;
    std::vector<std::shared_ptr<llm::Conversation>> m_coversations;
};
