#pragma once

#include "JSON.h"
#include "ai/Common.hpp"
#include "ai/Config.hpp"
#include "ai/LLMEvents.hpp"
#include "ai/ProgressToken.hpp"
#include "ai/ResponseCollector.hpp"
#include "ai/Tools.hpp"
#include "assistant/client_base.hpp"
#include "assistant/function.hpp"
#include "assistant/ollama_client.hpp"
#include "clResult.hpp"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <wx/event.h>
#include <wx/msgqueue.h>
#include <wx/string.h>
#include <wx/timer.h>

constexpr const char* kAssistantConfigFile = "assistant.json";

static const wxString kDefaultSettings = R"#(
{
  "history_size": 50,
  "mcp_servers": {},
  "log_level": "warn",
  "stream": true,
  "keep_alive": "24h",
  "max_tokens": 1024,
  "server_timeout": {
      "connect_msecs": 500,
      "read_msecs": 300000,
      "write_msecs": 300000
  },
  "endpoints": {
    "http://127.0.0.1:11434": {
      "active": true,
      "http_headers": {
        "Host": "127.0.0.1"
      },
      "type": "ollama"
    }
  },
  "models": {
    "default": {
      "options": {
        "num_ctx": 16384,
        "temperature": 0
      },
      "think_end_tag": "</think>",
      "think_start_tag": "</think>"
    }
  }
}
)#";

namespace llm
{
using assistant::AddFlagSet;
using assistant::ChatOptions;
using assistant::FunctionBase;
using assistant::FunctionBuilder;
using assistant::FunctionResult;
using assistant::FunctionTable;
using assistant::json;
using assistant::OnResponseCallback;
using assistant::Reason;

template <typename T>
inline clStatusOr<T> CheckType(const llm::json& j, const std::string& name)
{
    if (!j.contains(name)) {
        std::stringstream ss;
        ss << "Missing mandatory param: " << name;
        return StatusNotFound(ss.str());
    }
    try {
        auto v = j[name].get<T>();
        return v;
    } catch ([[maybe_unused]] const std::exception& e) {
        return StatusInavalidArgument("Mandatory param has wrong type");
    }
}

#define LLM_CONCAT_IMPL(a, b) a##b
#define LLM_CONCAT(a, b) LLM_CONCAT_IMPL(a, b)
#define LLM_UNIQUE_VAR(name) LLM_CONCAT(name, __LINE__)

#define LLM_ASSIGN_ARG_OR_RETURN_ERR(Decl, Args, ArgName, ArgType)                                      \
    auto LLM_UNIQUE_VAR(__result) = ::llm::CheckType<ArgType>(Args, ArgName);                           \
    if (!LLM_UNIQUE_VAR(__result).ok()) {                                                               \
        ::llm::FunctionResult res{                                                                      \
            .isError = true, .text = LLM_UNIQUE_VAR(__result).error_message().ToStdString(wxConvUTF8)}; \
        return res;                                                                                     \
    }                                                                                                   \
    Decl = LLM_UNIQUE_VAR(__result).value();

#define LLM_STRINGIFY(x) #x
#define LLM_TOSTRING(x) LLM_STRINGIFY(x)
#define LLM_CHECK_OR_RETURN_ERR(cond)                                       \
    if (!(cond)) {                                                          \
        std::stringstream ss;                                               \
        ss << "Function argument failed condition. " << LLM_TOSTRING(cond); \
        ::llm::FunctionResult res{.isError = true, .text = ss.str()};       \
        return res;                                                         \
    }

struct WXDLLIMPEXP_SDK EndpointData {
    std::string provider;
    std::string url;
    std::string model;
    std::optional<size_t> context_size;
    std::optional<std::string> api_key;
};

struct WXDLLIMPEXP_SDK ThreadTask {
    std::vector<std::string> prompt_array;
    std::string model;
    ChatOptions options{assistant::ChatOptions::kDefault};
    wxEvtHandler* owner{nullptr};
    std::shared_ptr<CancellationToken> cancellation_token;
    /// An optional collector object, if provided it wil be deleted by this class
    ResponseCollector* collector{nullptr};

    inline wxEvtHandler* GetEventSink() { return collector ? collector : owner; }
};

class WXDLLIMPEXP_SDK Manager : public wxEvtHandler
{
public:
    static Manager& GetInstance();

    /// Send prompt to the available LLM. Return a unique ID to be used when
    /// querying for the response. If no LLM is available, return
    /// `std::nullopt`.
    void Chat(wxEvtHandler* owner,
              const wxString& prompt,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options,
              const wxString& model = wxEmptyString);

    /// As before, but events are connected to a "ReponseCollector" object
    void Chat(ResponseCollector* collector,
              const wxString& prompt,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options,
              const wxString& model = wxEmptyString);

    /// Similar to chat, but instead of accepting a single prompt it accepts multiple
    /// prompts.
    void Chat(wxEvtHandler* owner,
              const wxArrayString& prompts,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options,
              const wxString& model = wxEmptyString);

    /// Similar to chat, but instead of accepting a single prompt it accepts multiple
    /// prompts.
    void Chat(ResponseCollector* collector,
              const wxArrayString& prompts,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options,
              const wxString& model = wxEmptyString);

    /// Return true if the LLM is available.
    inline bool IsAvailable() { return true; }

    /// Load the list of a models in a background thread.
    void LoadModels(wxEvtHandler* owner);

    void SetModels(const wxArrayString& models);
    wxArrayString GetModels() const;

    /// Clear the chat history.
    void ClearHistory();

    void Stop();
    void Start();
    void Restart();

    void RunTool(std::function<void()> tool) { tool(); }
    Config& GetConfig() { return m_config; }
    const Config& GetConfig() const { return m_config; }

    bool ReloadConfig(const wxString& config_content, bool prompt = true);

    inline bool IsBusy() const { return m_worker_busy.load(); }

    // Should not be called by users.
    void DeleteCollector(ResponseCollector* collector);

    /// Return the function table that should be used by plugins.
    FunctionTable& GetPluginFunctionTable() { return m_plugin_functions; }

    std::optional<wxString> ChooseModel(bool use_default);

    void AddNewEndpoint(const llm::EndpointData& d);

private:
    Manager();
    ~Manager();

    void PostTask(ThreadTask task);
    void WorkerMain();
    void PushThreadWork(ThreadTask work) { m_queue.Post(std::move(work)); }
    void CleanupAfterWorkerExit();
    assistant::Config MakeConfig();
    mutable std::mutex m_models_mutex;
    wxArrayString m_models GUARDED_BY(m_models_mutex);

    std::unique_ptr<std::thread> m_worker_thread;
    std::shared_ptr<assistant::ClientBase> m_client;

    Config m_config;
    assistant::Config m_default_config;
    assistant::Config m_client_config;
    wxMessageQueue<ThreadTask> m_queue;
    std::atomic_bool m_worker_thread_running{false};
    std::atomic_bool m_worker_busy{false};
    FunctionTable m_plugin_functions;
};

} // namespace llm
