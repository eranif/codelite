#pragma once

#include "ChatAIConfig.hpp"
#include "ChatAIEvents.hpp"
#include "LLMClientBase.hpp"
#include "ollama/ollama.hpp"

#include <thread>
#include <wx/msgqueue.h>
#include <wx/timer.h>

class OllamaClient : public LLMClientBase
{
public:
    OllamaClient();
    ~OllamaClient() override;

    void Send(wxString prompt, wxString model = wxEmptyString) override;
    void Send(wxEvtHandler* owner, wxString prompt, wxString model = wxEmptyString) override;

    void Interrupt() override;
    bool IsBusy() override { return m_processingRequest.load(std::memory_order_relaxed); }
    bool IsRunning() override { return m_client.IsRunning(); }
    void GetModels() override;
    void Clear() override;
    void ReloadConfig(const wxString& configContent) override;
    void SetLogSink(std::function<void(LLMLogLevel, std::string)> log_sink) override;

private:
    void WorkerThreadMain();
    void Shutdown();
    void Startup();
    void OnRunTool(LLMEvent& event);

    ollama::Client m_client;
    std::unique_ptr<std::thread> m_thread;
    mutable wxMessageQueue<Task> m_queue;
    std::atomic_bool m_processingRequest{false};
};
