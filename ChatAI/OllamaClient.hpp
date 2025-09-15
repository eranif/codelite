#pragma once

#include "ChatAIConfig.hpp"
#include "cl_command_event.h"
#include "ollama/ollama.hpp"

#include <thread>
#include <wx/msgqueue.h>

// --------------------------------------------------------------
// Ollama event
// --------------------------------------------------------------
class OllamaEvent : public clCommandEvent
{
public:
    OllamaEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    OllamaEvent(const OllamaEvent&) = default;
    OllamaEvent& operator=(const OllamaEvent&) = delete;
    ~OllamaEvent() override = default;
    wxEvent* Clone() const override { return new OllamaEvent(*this); }

    inline ollama::Reason GetReason() const { return m_reason; }
    inline void SetReason(ollama::Reason reason) { m_reason = reason; }

    inline void SetOutput(std::string message) { this->m_message = std::move(message); }
    inline const std::string& GetOutput() const { return m_message; }

    inline void SetModels(const wxArrayString& models) { this->m_models = models; }
    inline const wxArrayString& GetModels() const { return m_models; }

    inline void SetLogLevel(const ollama::LogLevel& logLevel) { m_logLevel = logLevel; }
    inline const ollama::LogLevel& GetLogLevel() const { return m_logLevel; }

    inline bool IsThinking() const { return m_thinking; }
    inline void SetThinking(bool b) { m_thinking = b; }

    inline void RunCallback()
    {
        if (m_callback) {
            m_callback();
        }
    }

    inline void SetCallback(std::function<void()> cb) { m_callback = std::move(cb); }

private:
    ollama::Reason m_reason{ollama::Reason::kDone};
    std::string m_message;
    wxArrayString m_models;
    ollama::LogLevel m_logLevel{ollama::LogLevel::kInfo};
    std::function<void()> m_callback{nullptr};
    bool m_thinking{false};
};

using OllamaEventFunction = void (wxEvtHandler::*)(OllamaEvent&);
#define OllamaEventHandler(func) wxEVENT_HANDLER_CAST(OllamaEventFunction, func)

class OllamaClient : public wxEvtHandler
{
public:
    enum class TaskKind {
        kChat,
        kReloadConfig,
        kListModels,
        kShutdown,
    };

    struct Task {
        TaskKind kind{TaskKind::kChat};
        wxString content; // depending on the task this can be the prompt or the configuration content.
        wxString model;   // the model to use
    };

    OllamaClient();
    virtual ~OllamaClient();

    ChatAIConfig& GetConfig() { return m_config; }
    void Send(wxString prompt, wxString model = wxEmptyString);
    void Interrupt();
    bool IsBusy() const { return m_processingRequest.load(std::memory_order_relaxed); }
    void GetModels() const;
    void Clear();
    void ReloadConfig(const wxString& configContent);
    void SetLogSink(std::function<void(ollama::LogLevel, std::string)> log_sink);

private:
    void WorkerThreadMain();
    void Shutdown();
    void Startup();
    void OnRunTool(OllamaEvent& event);

    ollama::Manager m_client;
    ChatAIConfig m_config;
    std::unique_ptr<std::thread> m_thread;
    mutable wxMessageQueue<Task> m_queue;
    std::atomic_bool m_processingRequest{false};
};

wxDECLARE_EVENT(wxEVT_OLLAMA_THINKING, OllamaEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_CHAT_STARTED, OllamaEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_CHAT_DONE, OllamaEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_CHAT_OUTPUT, OllamaEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_LIST_MODELS, OllamaEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_LOG, OllamaEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_RUN_TOOL, OllamaEvent);
