// clang-format off
#include <wx/app.h>
// clang-format on

#include "OllamaClient.hpp"

#include "clTempFile.hpp"
#include "event_notifier.h"
#include "ollama/config.hpp"

#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

OllamaEvent::OllamaEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

wxDEFINE_EVENT(wxEVT_OLLAMA_THINKING, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_CHAT_DONE, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_OUTPUT, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_LIST_MODELS, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_LOG, OllamaEvent);

#ifdef __WXMSW__
#include <windows.h>
#endif

namespace
{
const wxString PromptFile = "ChatAI.prompt";
const wxString CHATAI_PROMPT_STRING = "Ask me anything...";
} // namespace

OllamaClient::OllamaClient()
    : m_ollama(ollama::Manager::GetInstance())
{
}

OllamaClient::~OllamaClient() {}

bool OllamaClient::IsOk() const { return m_ollama.IsRunning(); }

void OllamaClient::Send(const wxString& prompt, const wxString& model)
{
    if (m_processingRequest) {
        return;
    }

    // Notify that
    m_processingRequest = true;
    clCommandEvent thinking{ wxEVT_OLLAMA_THINKING };
    thinking.SetEventObject(this);
    EventNotifier::Get()->AddPendingEvent(thinking);
    m_ollama.SetPreferCPU(true);
    m_ollama.AsyncChat(
        prompt.ToStdString(),
        [this](std::string msg, ollama::Reason reason) {
            // Translate the callback into wxWidgets event
            OllamaEvent event{ wxEVT_OLLAMA_OUTPUT };
            event.SetOutput(std::move(msg));
            event.SetEventObject(this);
            event.SetReason(reason);
            EventNotifier::Get()->AddPendingEvent(event);

            switch (reason) {
            case ollama::Reason::kDone:
            case ollama::Reason::kFatalError: {
                m_processingRequest = false;
                OllamaEvent chat_end{ wxEVT_OLLAMA_CHAT_DONE };
                chat_end.SetEventObject(this);
                EventNotifier::Get()->AddPendingEvent(chat_end);
            } break;
            default:
                break;
            }
        },
        model.ToStdString());
}

void OllamaClient::Interrupt() {}

void OllamaClient::GetModels() const
{
    std::thread thr([this]() {
        auto models = m_ollama.List();
        wxArrayString m;
        m.reserve(models.size());
        for (const auto& model : models) {
            m.Add(model);
        }

        OllamaEvent event_models{ wxEVT_OLLAMA_LIST_MODELS };
        event_models.SetModels(m);
        EventNotifier::Get()->AddPendingEvent(event_models);
    });
    thr.detach();
}

void OllamaClient::Clear() { m_ollama.Reset(); }

void OllamaClient::ReloadConfig(const wxString& configContent)
{
    auto config = ollama::Config::FromContent(configContent.ToStdString());
    if (config.has_value()) {
        m_ollama.ApplyConfig(&config.value());
    }
}

void OllamaClient::SetLogSink(std::function<void(ollama::LogLevel, std::string)> log_sink)
{
    ollama::SetLogSink(std::move(log_sink));
}
