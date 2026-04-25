#include "NewLLMEndpointWizard.hpp"

#include <regex>
#include <wx/richtooltip.h>

namespace
{
const wxString kProviderAnthropic = "Anthropic";
const wxString kProviderOpenAI = "OpenAI";
const wxString kProviderMoonshotAI = "MoonshotAI";
const wxString kProviderOllamaCloud = "Ollama (Cloud)";
const wxString kProviderOllamaLocal = "Ollama (Local)";

bool IsValidURL(const std::string& url)
{
    const std::regex url_pattern(R"(^(https?|ftp):\/\/[^\s/$.?#].[^\s]*$)");
    return std::regex_match(url, url_pattern);
}

} // namespace

NewLLMEndpointWizard::NewLLMEndpointWizard(wxWindow* parent)
    : NewLLMEndpointWizardBase(parent)
{
    int where = m_choiceProviders->FindString(kProviderAnthropic);
    if (where != wxNOT_FOUND) {
        m_choiceProviders->SetSelection(where);
        m_textCtrlBaseURL->ChangeValue("https://api.anthropic.com");
        m_spinCtrlContextSizeKB->SetValue(200);
    }
}

NewLLMEndpointWizard::~NewLLMEndpointWizard() {}

void NewLLMEndpointWizard::OnProviderChanged(wxCommandEvent& event)
{
    wxString provider = m_choiceProviders->GetStringSelection();
    if (provider == kProviderAnthropic) {
        m_textCtrlBaseURL->ChangeValue("https://api.anthropic.com");
        m_spinCtrlContextSizeKB->SetValue(200);
        m_spinCtrlMaxTokensKB->SetValue(64);
    } else if (provider == kProviderOpenAI) {
        m_textCtrlBaseURL->ChangeValue("https://api.openai.com");
        m_spinCtrlContextSizeKB->SetValue(400);
        m_spinCtrlMaxTokensKB->SetValue(64);
    } else if (provider == kProviderOllamaCloud) {
        m_textCtrlBaseURL->ChangeValue("https://ollama.com");
        m_spinCtrlContextSizeKB->SetValue(32);
        m_spinCtrlMaxTokensKB->SetValue(32);
    } else if (provider == kProviderOllamaLocal) {
        m_textCtrlBaseURL->ChangeValue("http://127.0.0.1:11434");
        m_spinCtrlContextSizeKB->SetValue(4);
        m_spinCtrlMaxTokensKB->SetValue(4);
    } else if (provider == kProviderMoonshotAI) {
        m_textCtrlBaseURL->ChangeValue("https://api.moonshot.ai");
        m_spinCtrlContextSizeKB->SetValue(256);
        m_spinCtrlMaxTokensKB->SetValue(256);
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
    llm::EndpointData data;
    if (provider == kProviderOllamaLocal) {
        // Local ollama
        data = llm::EndpointData{
            .client_type = llm::kClientTypeOllama,
            .url = m_textCtrlBaseURL->GetValue().ToStdString(wxConvUTF8),
            .model = m_textCtrlModel->GetValue().ToStdString(wxConvUTF8),
            .context_size = m_spinCtrlContextSizeKB->GetValue() * 1024,
        };
    } else if (provider == kProviderOllamaCloud) {
        // Cloud ollama
        data = llm::EndpointData{
            .client_type = llm::kClientTypeOllama,
            .url = m_textCtrlBaseURL->GetValue().ToStdString(wxConvUTF8),
            .model = m_textCtrlModel->GetValue().ToStdString(wxConvUTF8),
            .context_size = m_spinCtrlContextSizeKB->GetValue() * 1024,
            .api_key = m_textCtrlAPIKey->GetValue().ToStdString(wxConvUTF8),
        };
    } else if (provider == kProviderMoonshotAI) {
        // MoonshotAI
        data = llm::EndpointData{
            .client_type = llm::kClientTypeMoonshotAI,
            .url = m_textCtrlBaseURL->GetValue().ToStdString(wxConvUTF8),
            .model = m_textCtrlModel->GetValue().ToStdString(wxConvUTF8),
            .context_size = m_spinCtrlContextSizeKB->GetValue() * 1024,
            .api_key = m_textCtrlAPIKey->GetValue().ToStdString(wxConvUTF8),
            .max_tokens = m_spinCtrlMaxTokensKB->GetValue() * 1024,
        };
    } else if (provider == kProviderAnthropic) {
        // Anthropic
        data = llm::EndpointData{
            .client_type = llm::kClientTypeAnthropic,
            .url = m_textCtrlBaseURL->GetValue().ToStdString(wxConvUTF8),
            .model = m_textCtrlModel->GetValue().ToStdString(wxConvUTF8),
            .api_key = m_textCtrlAPIKey->GetValue().ToStdString(wxConvUTF8),
            .max_tokens = m_spinCtrlMaxTokensKB->GetValue() * 1024,
        };
    } else {
        // OpenAI
        data = llm::EndpointData{
            .client_type = llm::kClientTypeOpenAI,
            .url = m_textCtrlBaseURL->GetValue().ToStdString(wxConvUTF8),
            .model = m_textCtrlModel->GetValue().ToStdString(wxConvUTF8),
            .api_key = m_textCtrlAPIKey->GetValue().ToStdString(wxConvUTF8),
            .max_tokens = m_spinCtrlMaxTokensKB->GetValue() * 1024,
        };
    }
    return data;
}

void NewLLMEndpointWizard::OnApiKeyUI(wxUpdateUIEvent& event)
{
    wxString provider = m_choiceProviders->GetStringSelection();
    event.Enable(provider == kProviderAnthropic || provider == kProviderOllamaCloud || provider == kProviderOpenAI ||
                 provider == kProviderMoonshotAI);
}

void NewLLMEndpointWizard::OnMaxTokensUI(wxUpdateUIEvent& event)
{
    wxString provider = m_choiceProviders->GetStringSelection();
    event.Enable(provider == kProviderAnthropic || provider == kProviderMoonshotAI);
}
