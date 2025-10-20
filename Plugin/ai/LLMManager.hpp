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

static const wxString kDefaultSettings = R"#({
  "_version": 1.0,
  "endpoints": {},
  "history_size": 50,
  "keep_alive": "24h",
  "log_level": "warn",
  "mcp_servers": {},
  "server_timeout": {
    "connect_msecs": 500,
    "read_msecs": 300000,
    "write_msecs": 300000
  },
  "stream": true
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
    ChatOptions options{assistant::ChatOptions::kDefault};
    wxEvtHandler* owner{nullptr};
    std::shared_ptr<CancellationToken> cancellation_token;
    /// An optional collector object, if provided it wil be deleted by this class
    ResponseCollector* collector{nullptr};

    inline wxEvtHandler* GetEventSink() { return collector ? collector : owner; }
};

/**
 * @brief Singleton manager class for handling LLM (Large Language Model) operations.
 *
 * <p>This class provides a centralized interface for interacting with LLM systems,
 * managing background worker threads, and handling chat requests. It supports
 * both single and batch prompt processing, configuration management, and plugin
 * integration.</p>
 *
 * <p>All interactions with the LLM system should be performed through the singleton
 * instance obtained via {@link GetInstance()}.</p>
 */
class WXDLLIMPEXP_SDK Manager : public wxEvtHandler
{
public:
    /**
     * @brief Returns the singleton instance of the {@code Manager}.
     *
     * <p>All interactions with the LLM system should be performed through the instance
     * obtained from this method.</p>
     *
     * @return the global {@code Manager} instance
     */
    static Manager& GetInstance();

    /**
     * @brief Sends a single prompt to the currently available LLM and starts processing it.
     *
     * <p>The caller receives asynchronous events via the supplied {@link wxEvtHandler}
     * to report progress and completion. The {@link CancellationToken} can be used
     * to cancel the request, and {@link ChatOptions} configures the request
     * behaviour (e.g. streaming, temperature, etc.).</p>
     *
     * @param owner         the event handler that will receive the result events
     * @param prompt        the text prompt to send to the LLM
     * @param cancel_token  token that can be signaled to cancel the request
     * @param options       additional options controlling the chat behaviour
     */
    void Chat(wxEvtHandler* owner,
              const wxString& prompt,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options);

    /**
     * @brief Sends a single prompt to the LLM while events are collected by a
     * {@link ResponseCollector}.  This overload is useful when the caller prefers
     * a dedicated collector to aggregate responses instead of handling events
     * directly.
     *
     * @param collector     collector object that will receive the response events
     * @param prompt        the text prompt to send to the LLM
     * @param cancel_token  token that can be signaled to cancel the request
     * @param options       additional options controlling the chat behaviour
     */
    void Chat(ResponseCollector* collector,
              const wxString& prompt,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options);

    /**
     * @brief Sends multiple prompts to the LLM.  Each prompt in the array is processed
     * sequentially (or concurrently depending on the LLM implementation) and
     * responses are reported via the {@link wxEvtHandler} supplied in {@code owner}.
     *
     * @param owner         the event handler that will receive the result events
     * @param prompts       collection of prompts to send
     * @param cancel_token  token that can be signaled to cancel the request
     * @param options       additional options controlling the chat behaviour
     */
    void Chat(wxEvtHandler* owner,
              const wxArrayString& prompts,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options);

    /**
     * @brief Sends multiple prompts to the LLM while events are collected by a
     * {@link ResponseCollector}.  This is analogous to the {@link #Chat(wxEvtHandler*, const wxArrayString&,
     * std::shared_ptr<CancellationToken>, ChatOptions)} overload.
     *
     * @param collector     collector object that will receive the response events
     * @param prompts       collection of prompts to send
     * @param cancel_token  token that can be signaled to cancel the request
     * @param options       additional options controlling the chat behaviour
     */
    void Chat(ResponseCollector* collector,
              const wxArrayString& prompts,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options);

    /**
     * @brief Indicates whether an LLM is currently available for chat operations.
     *
     * @return {@code true} if an LLM can be used; {@code false} otherwise
     */
    inline bool IsAvailable() { return true; }

    /**
     * @brief Loads the list of available LLM models in a background thread and
     * notifies the supplied event handler when the operation completes.
     *
     * @param owner the event handler that will receive a notification once
     *              the model list has been loaded
     */
    void LoadModels(wxEvtHandler* owner);

    /**
     * @brief Sets the list of supported LLM models for the current configuration.
     *
     * @param models collection of model identifiers to enable
     */
    void SetModels(const wxArrayString& models);

    /**
     * @brief Retrieves the current list of enabled LLM models.
     *
     * @return the collection of model identifiers currently in use
     */
    wxArrayString GetModels() const;

    /**
     * @brief Clears all stored chat history from the internal state.
     *
     * <p>After this call, subsequent chat requests will start with an empty
     * conversation context.</p>
     */
    void ClearHistory();

    /**
     * @brief Stops the background worker thread if it is running.  The worker will
     * finish any currently queued work before exiting.
     */
    void Stop();

    /**
     * @brief Starts the background worker thread, creating it if it does not already
     * exist.  The worker will begin processing queued tasks immediately.
     */
    void Start();

    /**
     * @brief Restarts the worker thread by stopping it and then starting it again.
     *
     * <p>This forces a fresh context for the LLM and discards any pending
     * requests.</p>
     */
    void Restart();

    /**
     * @brief Executes a user-provided tool function immediately.
     *
     * @param tool a callable object that encapsulates the tool logic
     */
    void RunTool(std::function<void()> tool) { tool(); }

    /**
     * @brief Provides mutable access to the current configuration.
     *
     * @return reference to the {@link Config} object managed by the {@code Manager}
     */
    Config& GetConfig() { return m_config; }

    /**
     * @brief Provides read-only access to the current configuration.
     *
     * @return constant reference to the {@link Config} object
     */
    const Config& GetConfig() const { return m_config; }

    /**
     * @brief Reloads the manager configuration from the supplied content.
     *
     * <p>If {@code config_content} is {@code std::nullopt}, the configuration
     * will be reloaded from the default location.  When {@code prompt} is {@code true},
     * the user may be prompted to confirm the reload.</p>
     *
     * @param config_content optional string containing the new configuration JSON
     * @param prompt          whether to prompt the user before applying changes
     * @return {@code true} if the configuration was successfully reloaded
     */
    bool ReloadConfig(std::optional<wxString> config_content, bool prompt = true);

    /**
     * @brief Indicates whether the background worker thread is currently busy
     * processing a request.
     *
     * @return {@code true} if the worker is busy; {@code false} otherwise
     */
    inline bool IsBusy() const { return m_worker_busy.load(); }

    /**
     * @brief Internal helper that removes a previously registered {@link ResponseCollector}
     * from the manager.  This method should not be called by library users.
     *
     * @param collector the collector to be removed
     */
    void DeleteCollector(ResponseCollector* collector);

    /**
     * @brief Retrieves the function table used by plugins to expose LLM capabilities.
     *
     * @return reference to the plugin {@link FunctionTable}
     */
    FunctionTable& GetPluginFunctionTable() { return m_plugin_functions; }

    /**
     * @brief Enumerates all LLM endpoints that are currently available to the manager.
     *
     * @return collection of endpoint identifiers
     */
    wxArrayString ListEndpoints();

    /**
     * @brief Retrieves the currently active LLM endpoint, if any.
     *
     * @return optional endpoint identifier; {@code std::nullopt} if none is active
     */
    std::optional<wxString> GetActiveEndpoint();

    /**
     * @brief Sets the active LLM endpoint to the specified value.
     *
     * @param endpoint identifier of the endpoint to activate
     * @return {@code true} if the endpoint was successfully set
     */
    bool SetActiveEndpoint(const wxString& endpoint);

    /**
     * @brief Adds a new LLM endpoint to the manager's internal list.
     *
     * @param d data describing the new endpoint
     */
    void AddNewEndpoint(const llm::EndpointData& d);

    /**
     * @brief Opens the current settings file in the default editor configured
     * for the application.  This allows the user to edit configuration
     * parameters directly.
     */
    void OpenSettingsFileInEditor();

    /**
     * @brief Enables or disables all functions in the client's function table.
     *
     * This method enables or disables all functions by calling EnableAll on the
     * client's function table. If no client is available, the method returns without
     * performing any action.
     *
     * @param b true to enable all functions, false to disable all functions
     */
    void EnableAllFunctions(bool b);

    /**
     * @brief Enables or disables a function by name in the client's function table.
     *
     * This method retrieves the function table from the client and enables or disables
     * the specified function. If no client is available, the method returns early without
     * performing any action.
     *
     * @param name The name of the function to enable or disable
     * @param b True to enable the function, false to disable it
     */
    void EnableFunctionByName(const wxString& name, bool b);

private:
    Manager();
    ~Manager();

    clStatusOr<wxString> CreateOrOpenConfig();

    bool WriteConfigFile(llm::json j);
    void HandleConfigFileUpdated();

    void PostTask(ThreadTask task);
    void WorkerMain();
    void PushThreadWork(ThreadTask work) { m_queue.Post(std::move(work)); }
    void CleanupAfterWorkerExit();
    assistant::Config MakeConfig();
    void OnFileSaved(clCommandEvent& event);
    std::optional<llm::json> GetConfigAsJSON();

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

/**
 * @brief RAII helper that temporarily disables all LLM functions.
 *
 * Constructing an instance of {@code FunctionsDisabler} disables all
 * functions via {@code llm::Manager::GetInstance().EnableAllFunctions(false)}.
 * When the instance goes out of scope, the destructor re‑enables all functions
 * by calling {@code llm::Manager::GetInstance().EnableAllFunctions(true)}.
 *
 * This class is intended for use when a section of code requires the
 * LLM manager to operate with all functions disabled, ensuring that
 * the original state is restored automatically when the object is destroyed.
 *
 * @note The implementation relies on the singleton {@code llm::Manager}
 *       and assumes thread‑safety of the EnableAllFunctions method.
 *
 * @code
 * {
 *     FunctionsDisabler disabler; // disables all functions
 *     // critical section
 * } // functions re-enabled automatically
 * @endcode
 */
struct WXDLLIMPEXP_SDK FunctionsDisabler {
    FunctionsDisabler() { llm::Manager::GetInstance().EnableAllFunctions(false); }
    ~FunctionsDisabler() { llm::Manager::GetInstance().EnableAllFunctions(true); }
};

} // namespace llm
