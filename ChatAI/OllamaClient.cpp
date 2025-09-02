#include "OllamaClient.hpp"

#include "event_notifier.h"

#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

OllamaEvent::OllamaEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

wxDEFINE_EVENT(wxEVT_OLLAMA_THINKING, clCommandEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_READY, clCommandEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_OUTPUT, clCommandEvent);

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

void OllamaClient::Send(const wxString& prompt)
{
    if (!IsRunning() || m_processingRequest) {
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
            event.SetMessage(std::move(msg));
            event.SetEventObject(this);
            event.SetReason(reason);
            EventNotifier::Get()->AddPendingEvent(event);

            switch (reason) {
            case ollama::Reason::kDone:
            case ollama::Reason::kFatalError:
                m_processingRequest = false;
                break;
            default:
                break;
            }
        },
        m_model.ToStdString());
}

void OllamaClient::Interrupt() {}
