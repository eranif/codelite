#pragma once

#include "ChatAIConfig.hpp"
#include "ChatAIEvents.hpp"
#include "ollama/ollama.hpp"

#include <thread>
#include <wx/msgqueue.h>

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
        wxEvtHandler* owner{nullptr};
    };

    OllamaClient();
    virtual ~OllamaClient();

    ChatAIConfig& GetConfig() { return m_config; }
    void Send(wxString prompt, wxString model = wxEmptyString);
    void Send(wxEvtHandler* owner, wxString prompt, wxString model = wxEmptyString);

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
    void OnRunTool(LLMEvent& event);

    ollama::Manager m_client;
    ChatAIConfig m_config;
    std::unique_ptr<std::thread> m_thread;
    mutable wxMessageQueue<Task> m_queue;
    std::atomic_bool m_processingRequest{false};
};
