#pragma once

#include "JSON.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "ollama/function.hpp"
#include "ollama/ollama.hpp"

#include <algorithm>
#include <atomic>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/timer.h>

namespace llm
{

using ollama::ChatOptions;
using ollama::FunctionBase;
using ollama::FunctionBuilder;
using ollama::FunctionResult;
using ollama::FunctionTable;
using ollama::OnResponseCallback;
using ollama::Reason;

#define LLM_ASSIGN_ARG_OR_RETURN_ERR(expr, args, ArgName, ArgType)    \
    if (!args.hasNamedObject(ArgName)) {                              \
        std::stringstream ss;                                         \
        ss << "Missing mandatory param: " << ArgName;                 \
        ::llm::FunctionResult res{.isError = true, .text = ss.str()}; \
        return res;                                                   \
    }                                                                 \
    expr = args[ArgName].GetValue<ArgType>();

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
    enum ResponseFlags {
        kNone = 0,
        kThinking = (1 << 0),
        kError = (1 << 1),
        kCompleted = (1 << 2),
        kAborted = (1 << 3),
    };

    static Manager& GetInstance();

    /// Send prompt to the available LLM. Return a unique ID to be used when querying for the response.
    /// If no LLM is available, return `std::nullopt`.
    void Chat(const wxString& prompt, OnResponseCallback cb, size_t options, const wxString& model = wxEmptyString);

    /// Similar to chat, but instead of accepting a single prompt it accepts a prompt template and context array.
    /// For every context entry in the "prompt_context_arr", a new prompt is constructed and fed to the LLM model.
    /// If response_cb is provided, every generated token is also passed to that callback.
    /// Supported macros in the "prompt_template" are:
    /// {{CONTEXT}}
    void Chat(const wxString& prompt_template,
              const wxArrayString& prompt_context_arr,
              OnResponseCallback response_cb,
              size_t options,
              const wxString& model = wxEmptyString);

    /// Return true if the LLM is available.
    inline bool IsAvailable() { return m_client.IsRunning(); }

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

    std::unordered_map<wxString, std::shared_ptr<FunctionBase>> m_functions;
    wxArrayString m_models;
    wxString m_activeModel;
    mutable std::mutex m_models_mutex;
    ollama::Client m_client;
};
} // namespace llm
