// clang-format off
#include <wx/app.h>
// clang-format on

#include "OllamaClient.hpp"

#include "Tools.hpp"
#include "event_notifier.h"
#include "ollama/config.hpp"

#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

OllamaEvent::OllamaEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

wxDEFINE_EVENT(wxEVT_OLLAMA_THINKING, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_CHAT_STARTED, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_CHAT_DONE, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_CHAT_OUTPUT, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_LIST_MODELS, OllamaEvent);
wxDEFINE_EVENT(wxEVT_OLLAMA_LOG, OllamaEvent);

#ifdef __WXMSW__
#include <windows.h>
#endif

OllamaClient::OllamaClient()
{
    Clear();
    Startup();
}

OllamaClient::~OllamaClient()
{
    Task shutdown{.kind = TaskKind::kShutdown};
    m_queue.Post(shutdown);
    m_thread->join();
    m_thread.reset();
}

void OllamaClient::Startup()
{
    if (m_thread) {
        return;
    }
    m_thread = std::make_unique<std::thread>([this]() { WorkerThreadMain(); });
}

void OllamaClient::Shutdown()
{
    if (m_thread) {
        // Post a shutdown request and wait for its completion
        Task shutdown{.kind = TaskKind::kShutdown};
        m_queue.Post(shutdown);
        m_thread->join();
        m_thread.reset();
        // Clear any remaining messages from the queue
        m_queue.Clear();
        m_client.Reset();
    }
}

void OllamaClient::WorkerThreadMain()
{
    auto& function_table = m_client.GetFunctionTable();
    ollama::PopulateBuildInFunctions(function_table);
    m_client.AddSystemMessage("Your name is CodeLite");

    while (true) {
        Task t;
        m_processingRequest.store(false, std::memory_order_relaxed);
        if (m_queue.ReceiveTimeout(10, t) != wxMSGQUEUE_NO_ERROR) {
            continue;
        }
        m_processingRequest.store(true, std::memory_order_relaxed);
        switch (t.kind) {
        case TaskKind::kChat: {

            OllamaEvent chat_start{wxEVT_OLLAMA_CHAT_STARTED};
            chat_start.SetEventObject(this);
            EventNotifier::Get()->AddPendingEvent(chat_start);

            m_client.Chat(
                t.content.ToStdString(wxConvUTF8),
                [this](std::string msg, ollama::Reason reason, bool thinking) {
                    // Translate the callback into wxWidgets event
                    OllamaEvent event{wxEVT_OLLAMA_CHAT_OUTPUT};
                    event.SetStringRaw(std::move(msg));
                    event.SetEventObject(this);
                    event.SetReason(reason);
                    event.SetThinking(thinking);
                    EventNotifier::Get()->AddPendingEvent(event);

                    switch (reason) {
                    case ollama::Reason::kDone:
                    case ollama::Reason::kFatalError: {
                        OllamaEvent chat_end{wxEVT_OLLAMA_CHAT_DONE};
                        chat_end.SetEventObject(this);
                        EventNotifier::Get()->AddPendingEvent(chat_end);
                    } break;
                    default:
                        break;
                    }
                },
                t.model.ToStdString(wxConvUTF8));
        } break;
        case TaskKind::kReloadConfig: {
            auto config = ollama::Config::FromContent(t.content.ToStdString(wxConvUTF8));
            if (config.has_value()) {
                m_client.ApplyConfig(&config.value());
            }
        } break;
        case TaskKind::kShutdown:
            m_processingRequest.store(false, std::memory_order_relaxed);
            return;
        case TaskKind::kListModels: {
            auto models = m_client.List();
            wxArrayString m;
            m.reserve(models.size());
            for (const auto& model : models) {
                m.Add(model);
            }

            OllamaEvent event_models{wxEVT_OLLAMA_LIST_MODELS};
            event_models.SetModels(m);
            EventNotifier::Get()->AddPendingEvent(event_models);
        } break;
        }
    }
}

void OllamaClient::Send(wxString prompt, wxString model)
{
    Task task{.kind = TaskKind::kChat, .content = std::move(prompt), .model = model};
    m_queue.Post(std::move(task));
}

void OllamaClient::Interrupt()
{
    // this is the only method which we allow the other thread to access the m_client
    m_client.Interrupt();
}

void OllamaClient::GetModels() const
{
    Task task{.kind = TaskKind::kListModels};
    m_queue.Post(std::move(task));
}

void OllamaClient::Clear()
{
    Shutdown();
    Startup();
}

void OllamaClient::ReloadConfig(const wxString& configContent)
{
    Task task{.kind = TaskKind::kReloadConfig, .content = configContent};
    m_queue.Post(std::move(task));
}

void OllamaClient::SetLogSink(std::function<void(ollama::LogLevel, std::string)> log_sink)
{
    ollama::SetLogSink(std::move(log_sink));
}
