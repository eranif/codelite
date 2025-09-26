#pragma once

#include "ChatAIConfig.hpp"
#include "ChatAIEvents.hpp"
#include "LLMClientBase.hpp"
#include "ollama/ollama.hpp"

#include <thread>
#include <wx/msgqueue.h>
#include <wx/timer.h>

class OllamaImpl : public ollama::Client
{
public:
    OllamaImpl(const std::string& url, const std::unordered_map<std::string, std::string>& headers)
        : ollama::Client(url, headers)
    {
    }
    ~OllamaImpl() override = default;

    /// Clear the history.
    inline void ClearHistory() { m_messages.clear(); }
    inline void ClearQueue() { m_queue.clear(); }
};

class OllamaClient : public LLMClientBase
{
public:
    OllamaClient();
    ~OllamaClient() override;

    void Send(wxString prompt, wxString model = wxEmptyString, ChatOptions options = ChatOptions::kDefault) override;
    void Send(wxEvtHandler* owner,
              wxString prompt,
              wxString model = wxEmptyString,
              ChatOptions options = ChatOptions::kDefault) override;

    void Interrupt() override;
    bool IsBusy() override { return m_processingRequest.load(std::memory_order_relaxed); }
    bool IsRunning() override { return m_client.IsRunning(); }
    void GetModels() override;
    void Clear() override;
    void ReloadConfig(const wxString& configContent) override;
    void SetLogSink(std::function<void(LLMLogLevel, std::string)> log_sink) override;
    void OnInitDone() override;

private:
    void WorkerThreadMain();
    void Shutdown();
    void Startup();
    void OnRunTool(LLMEvent& event);

    OllamaImpl m_client;
    std::unique_ptr<std::thread> m_thread;
    mutable wxMessageQueue<Task> m_queue;
    std::atomic_bool m_processingRequest{false};
    std::atomic_bool m_shutdown_flag{false};
};
