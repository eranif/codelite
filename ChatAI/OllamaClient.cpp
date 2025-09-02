// clang-format off
#include <wx/app.h>
// clang-format on

#include "OllamaClient.hpp"

#include "event_notifier.h"

#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

OllamaEvent::OllamaEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

wxDEFINE_EVENT(wxEVT_OLLAMA_THINKING, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_CHAT_DONE, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_OUTPUT, OllamaEvent);

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
    if (!IsRunning()) {
        // TODO: prompt the user to start ollama
        return;
    }

    if (m_processingRequest) {
        return;
    }

    // Notify that
    m_processingRequest = true;
    clCommandEvent thinking{ wxEVT_OLLAMA_THINKING };
    thinking.SetEventObject(this);
    EventNotifier::Get()->AddPendingEvent(thinking);

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

wxArrayString OllamaClient::GetModels() const
{
    if (!IsRunning()) {
        return {};
    }

    auto models = m_ollama.List();
    wxArrayString m;
    m.reserve(models.size());
    for (const auto& model : models) {
        m.Add(model);
    }
    return m;
}