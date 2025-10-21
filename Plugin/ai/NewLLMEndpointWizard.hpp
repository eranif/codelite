#pragma once

#include "UI.hpp"
#include "ai/LLMManager.hpp"

#include <wx/richtooltip.h>

class WXDLLIMPEXP_SDK NewLLMEndpointWizard : public NewLLMEndpointWizardBase
{
public:
    NewLLMEndpointWizard(wxWindow* parent);
    ~NewLLMEndpointWizard() override;

    llm::EndpointData GetData() const;

protected:
    void OnApiKeyUI(wxUpdateUIEvent& event) override;
    void OnMaxTokensUI(wxUpdateUIEvent& event) override;
    void OnContextSizeUI(wxUpdateUIEvent& event) override;
    void OnFinished(wxWizardEvent& event) override;
    void OnPageChanging(wxWizardEvent& event) override;
    void OnProviderChanged(wxCommandEvent& event) override;
    void ShowTipFor(const wxString& message, wxWindow* control);

private:
    std::unique_ptr<wxRichToolTip> m_tooltip{nullptr};
};
