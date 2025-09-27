#pragma once

#include "JSON.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/timer.h>

namespace llm
{
struct WXDLLIMPEXP_SDK FunctionResult {
    bool isOk{true};
    std::string text;
};

inline FunctionResult CreateErr(const wxString& text)
{
    FunctionResult res{.isOk = false, .text = text.ToStdString(wxConvUTF8)};
    return res;
}

inline FunctionResult CreateOk(const wxString& text)
{
    FunctionResult res{.isOk = true, .text = text.ToStdString(wxConvUTF8)};
    return res;
}

struct WXDLLIMPEXP_SDK FunctionArg {
    /// The argument name
    std::string name;
    /// The argument description.
    std::string desc;
    /// The type of the argument (boolean, number, string etc.)
    std::string type;
};

using FunctionCall = std::function<FunctionResult(const JSONItem& args)>;
struct WXDLLIMPEXP_SDK Function {
    /// The function name.
    std::string m_name;
    /// The function description.
    std::string m_desc;
    /// Name + description of the required params for this function.
    std::vector<FunctionArg> m_params;
    /// The function callback.
    FunctionCall m_func{nullptr};
    Function() = delete;
    Function(const std::string& name, const std::string& desc, std::vector<FunctionArg> params, FunctionCall func_call)
        : m_name(name)
        , m_desc(desc)
        , m_params(std::move(params))
        , m_func(std::move(func_call))
    {
    }
};

/// The proper way to construct LLM function.
struct WXDLLIMPEXP_SDK FunctionBuilder {
public:
    FunctionBuilder(const std::string& name)
        : m_name(name)
    {
    }

    FunctionBuilder& SetDesc(const std::string& desc)
    {
        m_desc = desc;
        return *this;
    }

    FunctionBuilder& AddParam(const std::string& name, const std::string& desc, const std::string& type)
    {
        FunctionArg arg{.name = name, .desc = desc, .type = type};
        m_params.push_back(std::move(arg));
        return *this;
    }

    FunctionBuilder& SetCallback(FunctionCall cb)
    {
        m_func = std::move(cb);
        return *this;
    }

    Function Build()
    {
        Function func{m_name, m_desc, std::move(m_params), std::move(m_func)};
        return func;
    }

private:
    /// The function name.
    std::string m_name;
    /// The function description.
    std::string m_desc;
    /// Name + description of the required params for this function.
    std::vector<FunctionArg> m_params;
    /// The function callback.
    FunctionCall m_func{nullptr};
};

#define LLM_ASSIGN_ARG_OR_RETURN_ERR(expr, args, ArgName, ArgType)  \
    if (!args.hasNamedObject(ArgName)) {                            \
        std::stringstream ss;                                       \
        ss << "Missing mandatory param: " << ArgName;               \
        ::llm::FunctionResult res{.isOk = false, .text = ss.str()}; \
        return res;                                                 \
    }                                                               \
    expr = args[ArgName].GetValue<ArgType>();

#define LLM_STRINGIFY(x) #x
#define LLM_TOSTRING(x) LLM_STRINGIFY(x)
#define LLM_CHECK_OR_RETURN_ERR(cond)                                       \
    if (!(cond)) {                                                          \
        std::stringstream ss;                                               \
        ss << "Function argument failed condition. " << LLM_TOSTRING(cond); \
        ::llm::FunctionResult res{.isOk = false, .text = ss.str()};         \
        return res;                                                         \
    }

class WXDLLIMPEXP_SDK Manager : public wxEvtHandler
{
public:
    enum ResponseFlags {
        kNone = 0,
        kThinking = (1 << 0),
        kError = (1 << 1),
        kCompleted = (1 << 2),
        kAborted = (1 << 3),
    };

    enum ChatOptions {
        kDefault = 0,
        kNoTools = (1 << 0),
        kClearHistory = (1 << 2),
    };

    using ResponseCB = std::function<void(const std::string& message, size_t flags)>;

    static Manager& GetInstance();

    /// Send prompt to the available LLM. Return a unique ID to be used when querying for the response.
    /// If no LLM is available, return `std::nullopt`.
    std::optional<uint64_t>
    Chat(const wxString& prompt, ResponseCB cb, size_t options, const wxString& model = wxEmptyString);

    /// Similar to chat, but instead of accepting a single prompt it accepts a prompt template and context array.
    /// For every context entry in the "prompt_context_arr", a new prompt is constructed and fed to the LLM model.
    /// If response_cb is provided, every generated token is also passed to that callback.
    /// Supported macros in the "prompt_template" are:
    /// {{CONTEXT}}
    void Chat(const wxString& prompt_template,
              const wxArrayString& prompt_context_arr,
              ResponseCB response_cb,
              size_t options,
              const wxString& model = wxEmptyString);

    /// Return true if the LLM is available.
    inline bool IsAvailable() const { return m_isAvailable; }

    bool IsProcessing(uint64_t request_id) const
    {
        auto iter = std::find_if(m_requetstQueue.begin(), m_requetstQueue.end(), [request_id](const auto& p) {
            return p.first == request_id;
        });

        return iter != m_requetstQueue.end();
    }

    /// Remove request from the processing queue.
    void Cancel(uint64_t request_id);

    bool RegisterFunction(Function func);
    void UnregisterFunction(const std::string& funcname);

    /// Call `cb` for each function. This function consumes the registered function so after it completes,
    /// `m_functions.empty() == true`.
    void ConsumeFunctions(std::function<void(Function func)> cb);

    /// Return list of the available models.
    inline wxArrayString GetModels() const
    {
        std::unique_lock lk{m_models_mutex};
        return m_models;
    }

    /// Set the available models. This function is called from the ChatAI plugin if it is loaded.
    inline void SetModels(const wxArrayString& models, const wxString activeModel)
    {
        std::unique_lock lk{m_models_mutex};
        m_models = models;
        m_activeModel = activeModel;
    }

    inline wxString GetActiveModel() const
    {
        std::unique_lock lk{m_models_mutex};
        return m_activeModel;
    }

    void RestartClient();

private:
    Manager();
    ~Manager();

    void OnResponse(clLLMEvent& event);
    void OnResponseError(clLLMEvent& event);
    void OnResponseAborted(clLLMEvent& event);
    void OnTimer(wxTimerEvent& e);

    bool m_isAvailable{false};
    wxTimer m_timer;
    std::vector<std::pair<uint64_t, ResponseCB>> m_requetstQueue;
    std::unordered_map<wxString, Function> m_functions;
    wxArrayString m_models;
    wxString m_activeModel;
    mutable std::mutex m_models_mutex;
};
} // namespace llm
