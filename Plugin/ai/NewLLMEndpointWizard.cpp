#include "NewLLMEndpointWizard.hpp"

#include "clSystemSettings.h"

#include <regex>
#include <wx/richtooltip.h>

namespace
{
const wxString kProviderOllamaLocal = "Ollama (Local)";
const wxString kProviderOllamaCloud = "Ollama (Cloud)";
const wxString kProviderAnthropic = "Anthropic";

bool IsValidURL(const std::string& url)
{
    const std::regex url_pattern(R"(^(https?|ftp):\/\/[^\s/$.?#].[^\s]*$)");
    return std::regex_match(url, url_pattern);
}

} // namespace

NewLLMEndpointWizard::NewLLMEndpointWizard(wxWindow* parent) : NewLLMEndpointWizardBase(parent)
{
    m_banner108->SetGradient(clSystemSettings::GetDefaultPanelColour(), clSystemSettings::GetDefaultPanelColour());
    m_banner112->SetGradient(clSystemSettings::GetDefaultPanelColour(), clSystemSettings::GetDefaultPanelColour());

    int where = m_choiceProviders->FindString(kProviderOllamaLocal);
    if (where != wxNOT_FOUND) {
        m_choiceProviders->SetSelection(where);
        m_textCtrlBaseURL->ChangeValue("http://127.0.0.1:11434");
    }
}

NewLLMEndpointWizard::~NewLLMEndpointWizard() {}

void NewLLMEndpointWizard::OnProviderChanged(wxCommandEvent& event)
{
    wxString provider = m_choiceProviders->GetStringSelection();
    if (provider == kProviderAnthropic) {
        m_textCtrlBaseURL->ChangeValue("https://api.anthropic.com");
    } else if (provider == kProviderOllamaCloud) {
        m_textCtrlBaseURL->ChangeValue("https://ollama.com");
    } else if (provider == kProviderOllamaLocal) {
        m_textCtrlBaseURL->ChangeValue("http://127.0.0.1:11434");
    }
}

void NewLLMEndpointWizard::OnPageChanging(wxWizardEvent& event)
{
    event.Skip();
    if (!event.GetDirection()) {
        return;
    }

    if (event.GetPage() == m_wizardPageSettings) {
        // Check that all settings were populated.

        bool is_ok = IsValidURL(m_textCtrlBaseURL->GetValue().ToStdString(wxConvUTF8));
        if (!is_ok) {
            ShowTipFor(_("Invalid URL"), m_textCtrlBaseURL);
            event.Veto();
            event.Skip(false);
            return;
        }

        if (m_textCtrlModel->IsEmpty()) {
            ShowTipFor(_("Please choose a model"), m_textCtrlModel);
            event.Veto();
            event.Skip(false);
            return;
        }
    } else if (event.GetPage() == m_wizardPageAPI) {
        // API key is not needed for Ollama Local
        wxString provider = m_choiceProviders->GetStringSelection();
        if (provider == kProviderOllamaLocal) {
            return;
        }

        if (m_textCtrlAPIKey->IsEmpty()) {
            wxString message;
            message << _("API Key is required for provider: ") << provider;
            ShowTipFor(message, m_choiceProviders);
            event.Veto();
            event.Skip(false);
            return;
        }
    }
}

void NewLLMEndpointWizard::OnFinished(wxWizardEvent& event) {}

void NewLLMEndpointWizard::ShowTipFor(const wxString& message, wxWindow* control)
{
    m_tooltip = std::make_unique<wxRichToolTip>(_("Failed Validation"), message);
    m_tooltip->SetTimeout(3000);
    m_tooltip->SetIcon(wxICON_WARNING);
    m_tooltip->ShowFor(control);
}

void NewLLMEndpointWizard::OnContextSizeUI(wxUpdateUIEvent& event)
{
    wxString provider = m_choiceProviders->GetStringSelection();
    event.Enable(provider == kProviderOllamaLocal || provider == kProviderOllamaCloud);
}

llm::EndpointData NewLLMEndpointWizard::GetData() const
{
    wxString provider = m_choiceProviders->GetStringSelection();
    if (provider == kProviderOllamaLocal || provider == kProviderOllamaCloud) {
        provider = "ollama";
    } else {
        provider = "anthropic";
    }

    llm::EndpointData data{.provider = provider.ToStdString(wxConvUTF8),
                           .url = m_textCtrlBaseURL->GetValue().ToStdString(wxConvUTF8),
                           .model = m_textCtrlModel->GetValue().ToStdString(wxConvUTF8),
                           .context_size = m_spinCtrlContextSizeKB->GetValue() * 1024,
                           .api_key = m_textCtrlAPIKey->GetValue().ToStdString(wxConvUTF8)};
    return data;
}
