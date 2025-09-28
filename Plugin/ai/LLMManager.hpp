#pragma once

#include "JSON.h"
#include "ai/Common.hpp"
#include "ai/Config.hpp"
#include "ai/LLMEvents.hpp"
#include "ai/ProgressToken.hpp"
#include "ai/ResponseCollector.hpp"
#include "ai/Tools.hpp"
#include "clResult.hpp"
#include "ollama/client.hpp"
#include "ollama/client_base.hpp"
#include "ollama/function.hpp"

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

namespace llm
{
using ollama::AddFlagSet;
using ollama::ChatOptions;
using ollama::FunctionBase;
using ollama::FunctionBuilder;
using ollama::FunctionResult;
using ollama::FunctionTable;
using ollama::json;
using ollama::OnResponseCallback;
using ollama::Reason;

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
        wxUnusedVar(v);
        return StatusOk();
    } catch ([[maybe_unused]] const std::exception& e) {
        return StatusInavalidArgument("Mandatory param has wrong type");
    }
}

#define LLM_ASSIGN_ARG_OR_RETURN_ERR(expr, args, ArgName, ArgType)                                          \
    expr;                                                                                                   \
    if (auto status = ::llm::CheckType<ArgType>(args, ArgName); !status.ok()) {                             \
        ::llm::FunctionResult res{.isError = true, .text = status.error_message().ToStdString(wxConvUTF8)}; \
        return res;                                                                                         \
    } else {                                                                                                \
        expr = status.value();                                                                              \
    }

#define LLM_STRINGIFY(x) #x
#define LLM_TOSTRING(x) LLM_STRINGIFY(x)
#define LLM_CHECK_OR_RETURN_ERR(cond)                                       \
    if (!(cond)) {                                                          \
        std::stringstream ss;                                               \
        ss << "Function argument failed condition. " << LLM_TOSTRING(cond); \
        ::llm::FunctionResult res{.isError = true, .text = ss.str()};       \
        return res;                                                         \
    }

struct WXDLLIMPEXP_SDK ThreadTask {
    std::vector<std::string> prompt_array;
    std::string model;
    ChatOptions options{ollama::ChatOptions::kDefault};
    wxEvtHandler* owner{nullptr};
    std::shared_ptr<CancellationToken> cancellation_token;
    /// An optional collector object, if provided it wil be deleted by this class
    ResponseCollector* collector{nullptr};

    inline wxEvtHandler* GetEventSink() { return collector ? collector : owner; }
};

class WXDLLIMPEXP_SDK Client : public ollama::Client
{
public:
    Client(const std::string& url, const std::unordered_map<std::string, std::string>& headers)
        : ollama::Client(url, headers)
    {
    }
    ~Client() override = default;

    std::unique_ptr<ollama::Client> NewClient();
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

    /// Similar to chat, but instead of accepting a single prompt it accepts a
    /// prompt template and context array. For every context entry in the
    /// "prompt_context_arr", a new prompt is constructed and fed to the LLM
    /// model. If response_cb is provided, every generated token is also passed
    /// to that callback. Supported macros in the "prompt_template" are:
    /// {{CONTEXT}}
    void Chat(wxEvtHandler* owner,
              const wxString& prompt_template,
              const wxArrayString& prompt_context_arr,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options,
              const wxString& model = wxEmptyString);

    /// As before, but events are connected to a "ReponseCollector" object
    void Chat(ResponseCollector* collector,
              const wxString& prompt_template,
              const wxArrayString& prompt_context_arr,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options,
              const wxString& model = wxEmptyString);

    /// Return true if the LLM is available.
    inline bool IsAvailable() { return m_client->IsRunning(); }

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

private:
    Manager();
    ~Manager();

    void PostTask(ThreadTask task);
    void WorkerMain();
    void PushThreadWork(ThreadTask work) { m_queue.Post(std::move(work)); }
    void CleanupAfterWorkerExit();

    mutable std::mutex m_models_mutex;
    wxArrayString m_models GUARDED_BY(m_models_mutex);

    std::unique_ptr<std::thread> m_worker_thread;
    std::shared_ptr<llm::Client> m_client;

    Config m_config;
    std::optional<ollama::Config> m_client_config;
    wxMessageQueue<ThreadTask> m_queue;
    std::atomic_bool m_worker_thread_running{false};
    std::atomic_bool m_worker_busy{false};
    FunctionTable m_plugin_functions;
};

} // namespace llm
