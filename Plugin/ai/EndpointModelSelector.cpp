#include "EndpointModelSelector.hpp"

#include "ai/LLMManager.hpp"

#include <wx/sizer.h>

namespace
{
// Reference text for calculating control width
const wxString LONG_MODEL_NAME = "claude-sonnet-4-5-1234567890";

void SetWindowVariantSmall(wxWindow* ctrl)
{
#ifdef __WXMAC__
    ctrl->SetWindowVariant(wxWINDOW_VARIANT_MINI);
#else
    ctrl->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif
}

} // namespace

EndpointModelSelector::EndpointModelSelector(wxAuiToolBar* parent)
    : m_parent{parent}
{
    int control_width = CalculateControlWidth(LONG_MODEL_NAME);
    m_choiceEndpoints = new wxChoice(parent, wxID_ANY, wxDefaultPosition, wxSize(control_width, -1));
    m_choiceModels = new wxChoice(parent, wxID_ANY, wxDefaultPosition, wxSize(control_width, -1));

    m_parent->AddControl(m_choiceEndpoints);
    m_parent->AddControl(m_choiceModels);
    m_choiceModels->SetToolTip(_("Choose the model to use for this endpoint"));
    m_choiceEndpoints->SetToolTip(_("Choose the endpoint to use"));

    SetWindowVariantSmall(m_choiceEndpoints);
    SetWindowVariantSmall(m_choiceModels);

    m_choiceModels->Bind(wxEVT_CHOICE, &EndpointModelSelector::OnModelChanged, this);
    m_choiceEndpoints->Bind(wxEVT_CHOICE, &EndpointModelSelector::OnEndpointChanged, this);
    m_choiceEndpoints->Bind(wxEVT_UPDATE_UI, &EndpointModelSelector::OnBusyUI, this);
    m_choiceModels->Bind(wxEVT_UPDATE_UI, &EndpointModelSelector::OnBusyUI, this);
    llm::Manager::GetInstance().Bind(wxEVT_LLM_CONFIG_UPDATED, &EndpointModelSelector::OnLLMConfigUpdate, this);
    llm::Manager::GetInstance().Bind(wxEVT_LLM_STARTED, &EndpointModelSelector::OnLLMConfigUpdate, this);
    CallAfter(&EndpointModelSelector::UpdateChoices);
}

EndpointModelSelector::~EndpointModelSelector()
{
    llm::Manager::GetInstance().Unbind(wxEVT_LLM_CONFIG_UPDATED, &EndpointModelSelector::OnLLMConfigUpdate, this);
    llm::Manager::GetInstance().Unbind(wxEVT_LLM_STARTED, &EndpointModelSelector::OnLLMConfigUpdate, this);
}

void EndpointModelSelector::OnLLMConfigUpdate(clLLMEvent& event)
{
    event.Skip();
    UpdateChoices();
}

int EndpointModelSelector::CalculateControlWidth(const wxString& reference_text) const
{
    if (!m_parent) {
        return wxNOT_FOUND;
    }
    return m_parent->GetTextExtent(reference_text).GetWidth();
}

void EndpointModelSelector::UpdateModelsForEndpoint(const wxString& endpoint)
{
    m_choiceModels->Clear();
    auto result = llm::Manager::GetInstance().GetEndpointModels(endpoint);
    if (result.has_value()) {
        m_choiceModels->Append(result.value().second);
        m_choiceModels->SetStringSelection(result.value().first);
    }
}

void EndpointModelSelector::UpdateChoices()
{
    m_choiceEndpoints->Clear();
    m_choiceModels->Clear();

    m_choiceEndpoints->Append(llm::Manager::GetInstance().ListEndpoints());
    auto active_endpoint = llm::Manager::GetInstance().GetActiveEndpoint();
    if (active_endpoint.has_value()) {
        m_choiceEndpoints->SetStringSelection(active_endpoint.value());
        UpdateModelsForEndpoint(active_endpoint.value());
    }
}

void EndpointModelSelector::OnModelChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // Update the model in the configuration file.
    llm::Manager::GetInstance().SetEndpointModel(
        m_choiceEndpoints->GetStringSelection(), m_choiceModels->GetStringSelection());
}

void EndpointModelSelector::OnEndpointChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    llm::Manager::GetInstance().SetActiveEndpoint(m_choiceEndpoints->GetStringSelection());
    UpdateModelsForEndpoint(m_choiceEndpoints->GetStringSelection());
}

void EndpointModelSelector::OnBusyUI(wxUpdateUIEvent& event)
{
    event.Enable(llm::Manager::GetInstance().IsAvailable() && !llm::Manager::GetInstance().IsBusy());
}
