#pragma once

#include "UI.hpp"
#include "ai/Common.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ConfirmDialog : public ConfirmDialogBase
{
public:
    ConfirmDialog(wxWindow* parent);
    ~ConfirmDialog() override;

    llm::UserAnswer GetAnswer() const { return m_answer; }
    inline void SetFirstLine(const wxString& text) { m_staticTextLine1->SetLabel(text); }
    inline void SetSecondLine(const wxString& text) { m_staticTextLine2->SetLabel(text); }

protected:
    void OnAllow(wxCommandEvent& event) override;
    void OnDontAllow(wxCommandEvent& event) override;
    void OnTrust(wxCommandEvent& event) override;

    llm::UserAnswer m_answer{llm::UserAnswer::kNo};
};
