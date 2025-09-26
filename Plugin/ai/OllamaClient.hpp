#pragma once

#include "ai/Config.hpp"
#include "ai/Events.hpp"
#include "ollama/ollama.hpp"

#include <thread>
#include <wx/msgqueue.h>
#include <wx/timer.h>

namespace llm
{
enum class TaskKind {
    kChat,
    kReloadConfig,
    kLoadPluginFunctions,
    kListModels,
    kShutdown,
    kClearHistory,
};

enum ChatOptions {
    kDefault = 0,
    kNoTools = (1 << 0),
    kClearHistory = (1 << 1),
};
struct Task {
    TaskKind kind{TaskKind::kChat};
    wxString content; // depending on the task this can be the prompt or the configuration content.
    wxString model;   // the model to use
    wxEvtHandler* owner{nullptr};
    size_t options{0};
};

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

class OllamaClient : public wxEvtHandler
{
public:
    OllamaClient();
    ~OllamaClient();

    void Send(wxString prompt, wxString model = wxEmptyString, ChatOptions options = ChatOptions::kDefault);
    void Send(wxEvtHandler* owner,
              wxString prompt,
              wxString model = wxEmptyString,
              ChatOptions options = ChatOptions::kDefault);

    void Interrupt();
    bool IsBusy() { return m_processingRequest.load(std::memory_order_relaxed); }
    bool IsRunning() { return m_client.IsRunning(); }
    void GetModels();
    void Clear();
    void ReloadConfig(const wxString& configContent);
    void SetLogSink(std::function<void(ollama::LogLevel, std::string)> log_sink);
    void OnInitDone();
    Config& GetConfig() { return m_config; }
    const Config& GetConfig() const { return m_config; }

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
    Config m_config;
};
} // namespace llm