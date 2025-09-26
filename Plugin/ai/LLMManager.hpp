#pragma once

#include "JSON.h"
#include "ai/Common.hpp"
#include "ai/Config.hpp"
#include "ai/LLMEvents.hpp"
#include "ai/ProgressToken.hpp"
#include "ai/Tools.hpp"
#include "clResult.hpp"
#include "ollama/client.hpp"
#include "ollama/client_base.hpp"
#include "ollama/function.hpp"

#include <algorithm>
#include <atomic>
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

    /// Return true if the LLM is available.
    inline bool IsAvailable() { return m_client->IsRunning(); }

    /// Return list of the available models.
    wxArrayString LoadModels();

    inline wxArrayString GetModels() const
    {
        std::scoped_lock lk{m_models_mutex};
        return m_models;
    }

    inline wxString GetActiveModel() const
    {
        std::scoped_lock lk{m_models_mutex};
        return m_activeModel;
    }

    inline void SetActiveModel(const wxString& model)
    {
        std::scoped_lock lk{m_models_mutex};
        m_activeModel = model;
    }

    void Stop();
    void Start();

    inline void Restart()
    {
        Stop();
        Start();
    }

    void RunTool(std::function<void()> tool) { tool(); }
    Config& GetConfig() { return m_config; }
    const Config& GetConfig() const { return m_config; }

    bool ReloadConfig(const wxString& config_content, bool prompt = true);
    FunctionTable& GetFunctionTable() { return m_client->GetFunctionTable(); }

private:
    Manager();
    ~Manager();

    struct ThreadTask {
        std::vector<std::string> prompt_array;
        std::string model;
        ChatOptions options{ollama::ChatOptions::kDefault};
        wxEvtHandler* owner{nullptr};
        std::shared_ptr<CancellationToken> cancellation_token;
    };
    void WorkerMain();
    void PushThreadWork(ThreadTask work) { m_queue.Post(std::move(work)); }

    mutable std::mutex m_models_mutex;
    wxArrayString m_models GUARDED_BY(m_models_mutex);
    wxString m_activeModel GUARDED_BY(m_models_mutex);

    std::unique_ptr<std::thread> m_worker_thread;
    std::shared_ptr<ollama::Client> m_client;

    Config m_config;
    wxMessageQueue<ThreadTask> m_queue;
};

} // namespace llm
