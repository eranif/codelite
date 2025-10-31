#pragma once

#include "UI.hpp"
#include "codelite_exports.h"

#include <memory>
#include <vector>

class WXDLLIMPEXP_SDK ChatHistoryDialog : public ChatHistoryDialogBase
{
public:
    ChatHistoryDialog(wxWindow* parent);
    ~ChatHistoryDialog() override;

    void SetSelectedPrompt(const wxString& selectedPrompt) { this->m_selectedPrompt = selectedPrompt; }
    const wxString& GetSelectedPrompt() const { return m_selectedPrompt; }

protected:
    void OnInsert(wxCommandEvent& event) override;
    void OnInsertUI(wxUpdateUIEvent& event) override;
    void OnItemActivated(wxDataViewEvent& event) override;
    void OnClear(wxCommandEvent& event) override;
    void OnClearUI(wxUpdateUIEvent& event) override;
    void OnDelete(wxCommandEvent& event) override;
    void OnDeleteUI(wxUpdateUIEvent& event) override;

    void SetSelectionAndEndModal(const wxDataViewItem& item);
    wxArrayString GetPrompts() const;

private:
    wxString m_selectedPrompt;
    std::vector<std::shared_ptr<wxString>> m_prompts;
};
