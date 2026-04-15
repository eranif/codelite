#pragma once

#include "UI.hpp"
#include "ai/ChatHistoryPage.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ChatHistoryDialog : public ChatHistoryDialogBase
{
public:
    ChatHistoryDialog(wxWindow* parent);
    ~ChatHistoryDialog() override;

protected:
    void OnClear(wxCommandEvent& event) override;
    void OnClearUI(wxUpdateUIEvent& event) override;
    void OnDelete(wxCommandEvent& event) override;
    void OnDeleteUI(wxUpdateUIEvent& event) override;

    ChatHistoryPage* GetActivePage();

private:
    friend class ChatHistoryPage;
};
