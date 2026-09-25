#pragma once

#include "CustomControls/TextGenerationPreviewFrame.hpp"
#include "UnicodeSymbols.hpp"
#include "ai/ChatAI.hpp"
#include "ai/Config.hpp"
#include "ai/HistoryManager.hpp"
#include "ai/ProgressToken.hpp"
#include "ai/ResponseCollector.hpp"
#include "ai/Tools.hpp"
#include "ai/ToolsUtils.hpp"
#include "assistant/client/client_base.hpp"
#include "assistant/function.hpp"
#include "clModuleLogger.hpp"
#include "clResult.hpp"
#include "cl_command_event.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <wx/event.h>
#include <wx/msgqueue.h>
#include <wx/string.h>
#include <wx/timer.h>

constexpr const char* kAssistantConfigFile = "assistant.json";

INITIALISE_MODULE_LOG(LLOG, "LLM", "assistant.log")
#define LLOG_ERROR() LLOG().SetCurrentLogLevel(FileLogger::Error) << LLOG().Prefix()
#define LLOG_DEBUG() LLOG().SetCurrentLogLevel(FileLogger::Dbg) << LLOG().Prefix()
#define LLOG_TRACE() LLOG().SetCurrentLogLevel(FileLogger::Trace) << LLOG().Prefix()
#define LLOG_WARN() LLOG().SetCurrentLogLevel(FileLogger::Warning) << LLOG().Prefix()
#define LLOG_INFO() LLOG().SetCurrentLogLevel(FileLogger::Info) << LLOG().Prefix()

namespace llm
{
using assistant::AddFlagSet;
using assistant::ChatOptions;
using assistant::FunctionBase;
using assistant::FunctionBuilder;
using assistant::FunctionResult;
using assistant::FunctionTable;
using assistant::IsFlagSet;
using assistant::OnResponseCallback;
using assistant::Reason;
using assistant::Usage;

enum class PathMatch {
    kNone = 0,
    kIsPath = (1 << 0),
    kExactMatchOnly = (1 << 1),
};

/**
 * @brief Reads a mandatory, typed argument from a tool-call JSON object.
 *
 * @tparam T The C++ type the argument is expected to convert to.
 * @param j The JSON object holding the tool arguments.
 * @param name The key of the argument to read.
 *
 * @return The converted value on success; a NotFound status if @p name is missing, or an InvalidArgument
 *         status if the value cannot be converted to @p T.
 */
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
        return StatusInvalidArgument("Mandatory param has wrong type");
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
    std::string client_type;
    std::string url;
    std::string model;
    std::vector<std::string> models; // possible models for this endpoint.
    std::optional<size_t> context_size;
    std::optional<std::string> api_key;
    std::optional<size_t> max_tokens;
};

struct WXDLLIMPEXP_SDK LocalMcp {
    std::string name;
    std::vector<std::string> command;
    std::map<std::string, std::string> env;
};

struct WXDLLIMPEXP_SDK SSEMcp {
    std::string name;
    std::string base_url;
    std::string endpoint;
    std::string auth_token;
    std::map<std::string, std::string> headers;
};

/**
 * @brief Holds optional callbacks that are invoked once a queued chat task finishes.
 *
 * Exactly one of the two callbacks is run per task: the success callback when the request completed normally,
 * the error callback when it was cancelled or ended with an error. Both are dispatched on the main thread.
 */
struct WXDLLIMPEXP_SDK CompletionHandler {
public:
    CompletionHandler() = default;
    ~CompletionHandler() = default;

    /** @brief Runs the success callback on the main thread. No-op if none was set. */
    void RunSuccessCallback();

    /** @brief Runs the error callback on the main thread. No-op if none was set. */
    void RunErrorCallback();

    /** @brief Sets the callback to run when the request completes successfully. */
    void SetSuccessCallback(std::function<void()> cb) { m_successCallback = std::move(cb); }

    /** @brief Sets the callback to run when the request is cancelled or fails. */
    void SetErrorCallback(std::function<void()> cb) { m_errorCallback = std::move(cb); }

private:
    std::function<void()> m_successCallback{nullptr};
    std::function<void()> m_errorCallback{nullptr};
};

struct WXDLLIMPEXP_SDK ThreadTask {
    std::string prompt;
    ChatOptions options{assistant::ChatOptions::kDefault};
    wxEvtHandler* owner{nullptr};
    std::shared_ptr<CancellationToken> cancellation_token;
    /// An optional collector object, if provided it wil be deleted by this class
    ResponseCollector* collector{nullptr};
    std::shared_ptr<CompletionHandler> completion_handler{nullptr};

    /** @brief Returns the object that receives this task's events: the collector if set, otherwise the owner. */
    wxEvtHandler* GetEventSink() { return collector ? collector : owner; }
};

struct WXDLLIMPEXP_SDK TokenUsage {
    size_t context_size{0};
    size_t used{0};

    /**
     * @brief Returns how much of the context window is used, as a whole-number percentage.
     *
     * @return 0 if either value is zero, 100 if @c used reaches or exceeds @c context_size, otherwise the
     *         truncated percentage.
     */
    size_t GetPercentage() const
    {
        if (context_size == 0 || used == 0) {
            return 0;
        }

        if (used >= context_size) {
            return 100;
        }
        return static_cast<size_t>((static_cast<double>(used) / static_cast<double>(context_size)) * 100);
    }
};

//
// Type of providers.
constexpr const char* kClientTypeAnthropic = "anthropic";
constexpr const char* kClientTypeOllama = "ollama";
constexpr const char* kClientTypeOpenAI = "openai";
constexpr const char* kClientTypeMoonshotAI = "moonshotai";

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
     * @brief Initialises the manager. Must be called once during application start-up.
     *
     * Creates the chat UI object, subscribes to workspace and file-saved events, and arranges for the LLM client
     * to be created and started only once all plugins are loaded (on the application's init-done event).
     * Until that event fires, methods that require a running client are no-ops.
     */
    void Initialise();

    /**
     * @brief Sends a single prompt to the currently available LLM and starts processing it.
     *
     * <p>The caller receives asynchronous events via the supplied {@link wxEvtHandler}
     * to report progress and completion. The {@link CancellationToken} can be used
     * to cancel the request, and {@link ChatOptions} configures the request
     * behaviour (e.g. streaming, temperature, etc.).</p>
     *
     * Placeholders in @p prompt (e.g. `{{current_selection}}`, see GetAvailablePlaceHolders()) are expanded
     * before the request is queued. If the worker thread is not running, it is restarted first.
     *
     * @param owner         the event handler that will receive the result events
     * @param prompt        the text prompt to send to the LLM
     * @param cancel_token  token that can be signaled to cancel the request
     * @param options       additional options controlling the chat behaviour
     * @param completion_handler optional callbacks run on the main thread when the request succeeds or fails
     */
    void Chat(wxEvtHandler* owner,
              const wxString& prompt,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options,
              std::shared_ptr<CompletionHandler> completion_handler = nullptr);

    /**
     * @brief Trims older tool responses from the client's conversation history to free context space.
     *
     * Keeps the most recent tool responses and drops the rest, then lowers the manager's running token
     * estimate by the amount reclaimed.
     *
     * @return The number of tokens reclaimed, or 0 if there is no active client.
     */
    size_t Compact();

    /**
     * @brief Runs a Standard Operating Procedure (SOP) by sending it to the LLM as a prompt.
     *
     * Wraps @p prompt in an instruction to execute it as an SOP, lists each entry of @p params as an input
     * parameter (`- name = value`), and tells the model to fall back to the SOP's own defaults for any
     * parameter that is missing. The resulting task is queued like a regular Chat() request.
     *
     * @param owner         the event handler that will receive the result events
     * @param prompt        the SOP text
     * @param params        (name, value) pairs passed to the SOP as input parameters
     * @param cancel_token  token that can be signaled to cancel the request
     * @param options       additional options controlling the chat behaviour
     * @param completion_handler optional callbacks run on the main thread when the request succeeds or fails
     */
    void RunSOP(wxEvtHandler* owner,
                const wxString& prompt,
                const std::vector<std::pair<wxString, wxString>>& params,
                std::shared_ptr<CancellationToken> cancel_token,
                ChatOptions options,
                std::shared_ptr<CompletionHandler> completion_handler = nullptr);

    /**
     * @brief Sends a single prompt to the LLM while events are collected by a
     * {@link ResponseCollector}.  This overload is useful when the caller prefers
     * a dedicated collector to aggregate responses instead of handling events
     * directly.
     *
     * The manager takes ownership of @p collector and deletes it (on the main thread) once the task is done.
     *
     * @param collector     collector object that will receive the response events
     * @param prompt        the text prompt to send to the LLM
     * @param cancel_token  token that can be signaled to cancel the request
     * @param options       additional options controlling the chat behaviour
     * @param completion_handler optional callbacks run on the main thread when the request succeeds or fails
     */
    void Chat(ResponseCollector* collector,
              const wxString& prompt,
              std::shared_ptr<CancellationToken> cancel_token,
              ChatOptions options,
              std::shared_ptr<CompletionHandler> completion_handler = nullptr);

    /**
     * @brief Shows the Chat AI window (docked pane or floating frame) and focuses its input box.
     *
     * @param prompt optional text; if not empty, it is submitted to the chat window as a new chat message
     */
    void ShowChatWindow(const wxString& prompt = wxEmptyString);

    /**
     * @brief Indicates whether an LLM endpoint is configured and active.
     *
     * This checks the loaded configuration, not whether the client or worker thread is currently running.
     *
     * @return {@code true} if there is an active endpoint; {@code false} otherwise
     */
    bool IsAvailable();

    /**
     * @brief Clears the conversation history of the active client.
     *
     * <p>Also resets the last-request usage and the running token counter. After this call, subsequent chat
     * requests start with an empty conversation. System messages are not affected (see
     * ResetSystemMessagesToDefaults()). No-op if there is no active client.</p>
     */
    void ClearHistory();

    /**
     * @brief Adds or replaces one entry in the manager's local system-message registry, keyed by @p msgId.
     *
     * Entries are stored in an unordered map, so multiple independent sources (e.g. the built-in agentic-loop
     * message and the user's own system prompt) can each own a slot and update it without clobbering the others.
     * If @p msgId already has an entry, it is replaced.
     *
     * This call only updates the manager's local state — it does not touch the active client and has no effect
     * on an in-progress conversation until CommitSystemMessage() is called.
     *
     * @param msg The system message content for this entry.
     * @param msgId A caller-chosen identifier used to find/replace this entry later. Reuse the same ID to update
     *              a message you previously added, rather than accumulating duplicates under new IDs.
     */
    void AddSystemMessage(const wxString& msg, const wxString& msgId);

    /**
     * @brief Removes one entry from the manager's local system-message registry.
     *
     * Does nothing if @p msgId is not in the registry. Like AddSystemMessage(), this only affects local state;
     * call CommitSystemMessage() to make the change visible to the model.
     *
     * @param msgId The identifier the entry was added with.
     */
    void DeleteSystemMessage(const wxString& msgId);

    /**
     * @brief Removes every entry from the manager's local system-message registry.
     *
     * Like AddSystemMessage(), this only affects local state. The client's active system messages are left
     * untouched until CommitSystemMessage() is called.
     */
    void ClearSystemMessages();

    /**
     * @brief Re-applies the user's persisted system prompt to the running client.
     *
     * Reads the current value of Config::GetSystemPrompt(), stores it in the local registry under a fixed,
     * reserved ID (replacing any previous value for that ID), and immediately calls CommitSystemMessage() so
     * the change takes effect on the active client right away. Unlike AddSystemMessage()/ClearSystemMessages(),
     * this method does not require a separate commit call.
     *
     * Call this after the user edits their system prompt (e.g. via the System Prompt dialog) while a session
     * is already running, so the new prompt applies without restarting the client.
     */
    void UpdateUserSystemPrompt();

    /**
     * @brief Resets the manager's system-message registry back to its standing defaults and commits them.
     *
     * Clears every entry, then re-adds the ones that are derived from current application state rather than
     * accumulated during the session: the built-in agentic-loop instruction, the open workspace's `AGENTS.md`/
     * `CLAUDE.md` content (if any), and the user's persisted system prompt — the same set Start() seeds a new
     * client with. Files added ad hoc via AddFilesToContext() are intentionally NOT restored, since they have
     * no fixed source to re-derive them from.
     *
     * Call this whenever the conversation is reset but the client itself keeps running (e.g. "Clear Session"),
     * so standing context isn't silently lost until the next full restart.
     */
    void ResetSystemMessagesToDefaults();

    /**
     * @brief Pushes the manager's local system-message registry to the active client.
     *
     * Concatenates the content of every entry in the registry (in unspecified order, one per line) into a
     * single string, clears the client's existing system messages, then adds the concatenated string back as
     * one system message. No-op if there is no active client.
     *
     * Call this after one or more AddSystemMessage()/ClearSystemMessages() calls to make the accumulated
     * changes visible to the model.
     */
    void CommitSystemMessage();

    /**
     * @brief Snapshots the client's current history into a Conversation object.
     *
     * @param conversation_text The rendered conversation text (as shown in the chat window).
     * @param label A short, human-readable label for the conversation.
     *
     * @return A Conversation holding the client's message history, @p conversation_text and @p label, or
     *         std::nullopt if there is no active client.
     */
    std::optional<llm::Conversation> NewConversation(const wxString& conversation_text, const wxString& label) const;

    /**
     * @brief Replaces the active client's message history with the one stored in @p conversation.
     *
     * The history is forwarded as-is, without validation. No-op if there is no active client.
     *
     * @param conversation The conversation whose messages should become the client's history.
     */
    void LoadConversation(const llm::Conversation& conversation);

    /**
     * @brief Stops the LLM client and its background worker thread.
     *
     * Signals all termination flags handed out by NewTerminationFlag(), interrupts the client, drains the task
     * queue (each pending task is completed with a "LLM is going down" message), detaches the worker thread so
     * an in-flight request cannot block the UI, and releases the client. Fires wxEVT_LLM_STOPPED when done.
     * No-op if the manager was not initialised or there is no active client.
     */
    void Stop();

    /**
     * @brief Creates and starts the LLM client and its background worker thread.
     *
     * Builds a client from the current configuration (or uses @p client if provided), registers the built-in,
     * plugin and MCP tools, applies the saved per-tool enabled states, pricing and caching policy, seeds the
     * system messages via ResetSystemMessagesToDefaults(), and launches the worker thread. Fires
     * wxEVT_LLM_STARTED when done. No-op until Initialise() has completed its start-up sequence.
     *
     * @param client optional, pre-built client to use instead of creating one from the configuration
     */
    void Start(std::shared_ptr<assistant::ClientBase> client = nullptr);

    /**
     * @brief Restarts the client by calling Stop() and then Start().
     *
     * <p>This forces a fresh context for the LLM; pending requests are completed with a "going down" message
     * and the conversation history is discarded together with the old client.</p>
     */
    void Restart();

    /**
     * @brief Provides mutable access to the current configuration.
     *
     * @return reference to the {@link Config} object managed by the {@code Manager}
     */
    Config& GetConfig() { return m_config; }

    /**
     * @brief Provides read-only access to the store of saved conversations.
     *
     * @return constant reference to the {@link HistoryStore}
     */
    const HistoryStore& GetHistoryStore() const { return m_history; }

    /**
     * @brief Saves the current conversation in the history store of the active endpoint.
     *
     * Builds a Conversation from the client's history, @p conversation_text and @p label (see
     * NewConversation()), and stores it under the currently active endpoint. The operation fails if there is
     * no active endpoint, if the conversation cannot be created, or if the history store rejects the record.
     *
     * @param conversation_text The rendered conversation text to persist.
     * @param label A short, human-readable label for the conversation.
     *
     * @return true if the conversation was stored; false otherwise. This function does not throw.
     */
    bool StoreCurrentConverstation(const wxString& conversation_text, const wxString& label);

    /**
     * @brief Provides read-only access to the current configuration.
     *
     * @return constant reference to the {@link Config} object
     */
    const Config& GetConfig() const { return m_config; }

    /**
     * @brief Reloads the LLM configuration and restarts the client with it.
     *
     * <p>If {@code config_content} is {@code std::nullopt}, the configuration is read from the assistant
     * settings file. The content is validated first; on success the running client is stopped and a new one
     * is created and started from the new configuration.</p>
     *
     * @param config_content optional string containing the new configuration JSON
     * @param prompt          whether to ask the user to confirm before the client is restarted
     * @return {@code true} if the configuration was reloaded and the new client started; {@code false} if the
     *         user declined, the content is invalid, or the client could not be created
     */
    bool ReloadConfig(std::optional<wxString> config_content, bool prompt = true);

    /**
     * @brief Indicates whether the background worker thread is currently busy
     * processing a request.
     *
     * @return {@code true} if the worker is busy; {@code false} otherwise
     */
    bool IsBusy() const { return m_worker_busy.load(); }

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
     * @brief Enumerates all LLM endpoints that are configured.
     *
     * @return the URLs of all configured endpoints (an endpoint's URL is its identifier)
     */
    wxArrayString ListEndpoints();

    /**
     * @brief Retrieves the currently active LLM endpoint, if any.
     *
     * @return the active endpoint's URL; {@code std::nullopt} if none is active
     */
    std::optional<wxString> GetActiveEndpoint() const;

    /**
     * @brief Makes the specified endpoint the active one.
     *
     * Marks @p endpoint as active (and every other endpoint as inactive) in the configuration file, writes the
     * file and reloads the configuration, which restarts the client.
     *
     * @param endpoint identifier (URL) of the endpoint to activate
     * @return {@code true} if the endpoint exists and the configuration was updated; {@code false} otherwise
     */
    bool SetActiveEndpoint(const wxString& endpoint);

    /**
     * @brief Sets the active model for a specified endpoint and updates the configuration.
     *
     * This method updates the endpoint's current model setting in the JSON configuration
     * and ensures the model is included in the endpoint's list of available models. If the
     * model is not already in the list, it is appended. The updated configuration is then
     * written to the configuration file.
     *
     * @param endpoint The name or identifier of the endpoint to configure (as wxString).
     * @param model The model name to set as active for the endpoint (as wxString).
     *
     * @return void This function does not return a value.
     *
     * @throws None Exceptions are caught internally; failures result in silent early return.
     *
     * @note If GetConfigAsJSON() fails or returns an empty optional, the function returns
     *       immediately without making changes. Similarly, if a std::exception is thrown
     *       during JSON manipulation or file writing, it is caught and the function exits
     *       silently.
     */
    void SetEndpointModel(const wxString& endpoint, const wxString& model);

    /**
     * @brief Return the list of possible models for the input endpoint.
     *
     * @param endpoint identifier of the endpoint.
     * @return pair of {active-model, all-models} for the endpoint.
     */
    std::optional<std::pair<wxString, wxArrayString>> GetEndpointModels(const wxString& endpoint);

    /**
     * @brief Adds a new LLM endpoint to the configuration file.
     *
     * Replaces any existing endpoint with the same URL, fills in provider-specific settings (e.g. the auth
     * headers) based on the endpoint's client type, makes it active if no other endpoint is, then writes the
     * file and reloads the configuration. Errors are caught and logged.
     *
     * @param d data describing the new endpoint
     */
    void AddNewEndpoint(const llm::EndpointData& d);

    /**
     * @brief Adds a new local MCP (Model Context Protocol) server configuration to the manager.
     *
     * The SSE overload below does the same for a remote server that is reached over SSE (base URL, endpoint,
     * auth token and headers) instead of a local command.
     *
     * This method reads the current configuration as JSON, ensures an "mcp_servers" section exists,
     * adds or updates the MCP server entry with the provided details (name, command, environment),
     * writes the updated configuration to disk, and triggers a configuration reload if successful.
     * Any errors during the process are caught and logged.
     *
     * @param d A LocalMcp object containing the MCP server configuration details including name,
     *          command to execute, and environment variables.
     *
     * @return void This function does not return a value.
     *
     * @throws Does not throw exceptions; all std::exception instances are caught internally and
     *         logged via clERROR().
     *
     * @see HandleConfigFileUpdated()
     * @see WriteConfigFile()
     * @see GetConfigAsJSON()
     */
    void AddNewMcp(const llm::LocalMcp& d);
    void AddNewMcp(const llm::SSEMcp& d);

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

    /**
     * @brief Returns a list of all registered function names and their enabled states.
     *
     * This method retrieves all functions from the client's function table and returns
     * a vector of pairs containing each function's name and its current enabled state.
     *
     * @return A vector of pairs (name, enabled) for all registered functions.
     */
    std::vector<std::pair<wxString, bool>> GetAllFunctions() const;

    /**
     * @brief Shows a text generation dialog and streams AI-generated text to a preview frame.
     *
     * This method initiates an asynchronous chat session with the AI assistant, displays a progress
     * indicator in the preview frame, and streams the generated text as it arrives. The preview frame
     * is automatically shown and updated with progress states (thinking, working, ready) throughout
     * the generation process.
     *
     * @param prompt The input prompt string to send to the AI assistant for text generation.
     * @param chat_options Optional ChatOptions to customize the chat behavior. If not provided,
     *                     defaults to kNoTools and kNoHistory flags.
     * @param preview_kind The kind of content being generated; selects how the preview frame is initialised.
     * @param completion_callback A function to be called when text generation completes or is cancelled.
     *                            May be null/empty.
     *
     * @return void This function does not return a value.
     *
     * @note The function creates a ResponseCollector that must be managed by the Chat method.
     *       State change callbacks are expected to run on the main UI thread.
     *
     * @warning If StateChangingCB is invoked from a non-main thread, a warning is logged and
     *          the callback returns early without updating the UI.
     *
     * @see TextGenerationPreviewFrame
     * @see assistant::ChatOptions
     * @see llm::ResponseCollector
     * @see Manager::Chat
     */
    void ShowTextGenerationDialog(const wxString& prompt,
                                  std::optional<assistant::ChatOptions> chat_options,
                                  PreviewKind preview_kind,
                                  std::function<void()> completion_callback = nullptr);

    /**
     * @brief Returns the placeholder tokens (e.g. `{{current_selection}}`, `{{workspace_path}}`) that are
     * expanded in prompts before they are sent to the LLM.
     */
    const std::vector<wxString>& GetAvailablePlaceHolders() const;

    /**
     * @brief Returns the Chat AI window.
     *
     * @param ensure_visibile if true, the window is made visible before being returned
     * @return the chat window, or nullptr if the manager has not been initialised
     */
    ChatAIWindow* GetChatWindow(bool ensure_visibile)
    {
        CHECK_PTR_RET_NULL(m_chatAI.get());
        return m_chatAI->GetChatWindow(ensure_visibile);
    }

    /**
     * @brief Returns the cost of the last request, or 0.0 if there is no active client.
     */
    double GetLastRequestCost() const
    {
        CHECK_COND_RET_VAL(m_client, 0.0);
        return m_client->GetLastRequestCost();
    }

    /**
     * @brief Returns the accumulated cost of all requests made by the active client, or 0.0 if there is none.
     */
    double GetTotalCost() const
    {
        CHECK_COND_RET_VAL(m_client, 0.0);
        return m_client->GetTotalCost();
    }

    /**
     * @brief Returns the client's total token usage against its context window.
     *
     * @return the usage, or std::nullopt if there is no active client or the client reports no usage stats
     */
    std::optional<llm::TokenUsage> GetUsage() const
    {
        CHECK_COND_RET_VAL(m_client, std::nullopt);
        auto usage = m_client->GetTokenUsageStats();
        if (!usage) {
            return std::nullopt;
        }

        return llm::TokenUsage{
            .context_size = usage->context_size,
            .used = static_cast<size_t>(usage->total_tokens_used),
        };
    }

    /**
     * @brief Retrieves the last request usage.
     *
     * @return the usage of the last request, or std::nullopt if there is no active client
     */
    std::optional<Usage> GetLastRequestUsage() const
    {
        CHECK_COND_RET_VAL(m_client, std::nullopt);
        return m_client->GetLastRequestUsage();
    }

    /**
     * @brief Returns the name of the model used by the active client, or std::nullopt if there is none.
     */
    std::optional<wxString> GetModelName() const
    {
        CHECK_COND_RET_VAL(m_client, std::nullopt);
        return m_client->GetModel();
    }

    /**
     * @brief Returns the manager's running estimate of the tokens currently used in the context window.
     *
     * The counter grows as messages pass through the worker, and is reduced by compaction and reset by
     * ClearHistory(). Returns 0 if there is no active client.
     */
    size_t GetContextUsedTokens() const
    {
        CHECK_COND_RET_VAL(m_client, 0);
        return m_tokens;
    }

    /**
     * @brief Returns the context window size (in tokens) of the active client, or 0 if there is none.
     */
    size_t GetTotalContextSize() const
    {
        CHECK_COND_RET_VAL(m_client, 0);
        return m_client->GetContextSize();
    }

    /**
     * @brief Returns true if the active client has pricing information for its model (so costs can be shown).
     */
    bool HasPricing() const { return m_client && m_client->GetPricing().has_value(); }

    /**
     * @brief Asks the user a yes/no/trust question in the chat window and blocks until they answer.
     *
     * Meant to be called from the worker thread (e.g. when the model wants to run a tool). The question is
     * printed in the chat window and the calling thread waits, polling so that it can abort if the client is
     * being stopped or restarted. The reply is interpreted (case-insensitively) as:
     * - "yes" / "y" / "ok": allow the call.
     * - "no" / "n": deny the call ("Permission denied").
     * - "trust" / "t": allow the call and run @p on_trust_cb on the main thread.
     * - anything else: deny the call, and pass the user's text back as the reason.
     *
     * @param text The question text to display to the user.
     * @param on_trust_cb Optional callback run on the main thread when the user answers "trust", typically to
     *                    persist the trust decision.
     *
     * @return A CanInvokeToolResult telling the caller whether it may proceed, and why not if it may not.
     */
    CanInvokeToolResult PromptUserYesNoTrustQuestion(const wxString& text, std::function<void()> on_trust_cb = nullptr);

    /**
     * @brief Sets the caching policy for the assistant client.
     *
     * This method configures the caching behavior by delegating to the underlying
     * client. The function performs a null-pointer check on the client before setting
     * the policy and returns early if the client is not initialized.
     *
     * @param policy The caching policy to be applied to the assistant client.
     */
    void SetCachingPolicy(llm::CachePolicy policy);

    /**
     * @brief Returns the caching policy of the active client, or CachePolicy::kNone if there is no client.
     */
    llm::CachePolicy GetCachingPolicy() const
    {
        if (!m_client) {
            return llm::CachePolicy::kNone;
        }
        return m_client->GetCachingPolicy();
    }

    /**
     * @brief Prints a message with an icon to the chat window.
     *
     * The message is prefixed with the symbol of @p icon and appended on the main thread, on a new line if the
     * current chat text does not end with one. Safe to call from any thread. No-op if there is no active
     * client or chat window.
     *
     * @param msg The message to be printed in the chat window.
     * @param icon The type of icon to display alongside the message.
     *
     * @see IconType
     */
    void PrintMessage(const wxString& msg, IconType icon);

    using PromptFuture = std::shared_ptr<std::future<std::string>>;
    using PromptPromise = std::shared_ptr<std::promise<std::string>>;

    /**
     * @brief Prints a question in the chat window and returns a future for the user's typed answer.
     *
     * On the main thread, the message is appended to the chat window (prefixed with the symbol of @p icon), the
     * prompt panel is shown, and a promise is queued on the chat window. The returned future becomes ready when
     * the user submits an answer. Safe to call from any thread.
     *
     * @param msg The message text to append to the chat window.
     * @param icon The icon type used to generate the string prefix shown before the message.
     *
     * @return A shared future that receives the user's answer, or a null pointer if there is no active client or
     *         chat window.
     */
    PromptFuture PromptUser(const wxString& msg, IconType icon);

    /**
     * @brief Creates and registers a new termination flag for task cancellation.
     *
     * Allocates a new atomic boolean flag initialized to false, registers it in the
     * manager's internal list of termination flags, and returns a shared pointer to it.
     * Thread-safe: uses internal mutex to protect the termination flags container.
     *
     * @return std::shared_ptr<std::atomic_bool> A shared pointer to a newly created
     *         atomic boolean flag, initialized to false. The flag can be used to
     *         signal termination to tasks managed by this Manager instance.
     *
     * @throws May throw std::bad_alloc if memory allocation fails, or any exception
     *         thrown by std::vector::push_back during container expansion.
     */
    std::shared_ptr<std::atomic_bool> NewTerminationFlag();

    /**
     * @brief Removes a termination flag from the manager's collection.
     *
     * This method safely removes the specified termination flag from the internal
     * collection by comparing raw pointer addresses. The operation is thread-safe
     * and uses a mutex to protect the shared collection.
     *
     * @param flag A shared pointer to the atomic boolean termination flag to be
     *             removed from the manager's collection. The flag is identified
     *             by comparing the raw pointer address rather than the shared
     *             pointer itself.
     *
     * @return Nothing.
     *
     * @note This method locks m_termination_flags_mutex during execution to ensure
     *       thread-safe modification of the termination flags collection.
     */
    void DeleteTerminationFlag(std::shared_ptr<std::atomic_bool> flag);

    /**
     * @brief Event handler for the "generate documentation comment" command.
     *
     * Finds the symbol nearest to the caret in the active editor, builds a prompt from the configured
     * comment-generation template and the symbol's source text, and streams the model's answer into a preview
     * frame. Shows a warning if the file cannot be parsed or there is not enough context.
     */
    void OnGenerateDocString(wxCommandEvent& event);

    /**
     * @brief Returns true while Stop() is in progress (used to abort blocking waits, e.g. user prompts).
     */
    bool IsClientStopping() const { return m_clientStopInProgress.load(); }

    /**
     * @brief Checks whether a tool is trusted for the given file path.
     *
     * This method delegates the trust decision to the configuration and validates the
     * file path against each trusted pattern. Pattern matching supports three forms:
     * "*" matches all paths, a trailing "*" performs a prefix match, and any other
     * value requires an exact path match.
     *
     * @param toolname The tool name to validate.
     * @param path The file path to compare against the trusted patterns.
     * @param flags Optional matching flags: PathMatch::kIsPath normalises @p path before comparing;
     *              PathMatch::kExactMatchOnly ignores the "*" and prefix forms and only accepts exact matches.
     *
     * @return bool True if the tool is trusted for the specified path; otherwise false.
     */
    bool
    CheckIfPathIsAllowedForTool(const wxString& toolname, const wxString& path, PathMatch flags = PathMatch::kNone);
    /**
     * @brief Checks whether all commands in a shell command string are allowed for a given tool.
     *
     * The command string is split into individual shell commands, and only the main executable
     * from each command is validated against the tool's allowed path rules. If any command is
     * empty or any executable is not permitted, the function returns false.
     *
     * @param toolname const wxString& Name of the tool requesting the command check.
     * @param cmd const wxString& Full shell command string to validate.
     * @param working_directory const wxString& Working directory associated with the command (currently unused).
     * @return bool true if every extracted command is allowed; false otherwise.
     */
    bool CheckIfShellCommandAllowed(const wxString& toolname, const wxString& cmd, const wxString& working_directory);

    /**
     * @brief Shows a dialog to choose a trust level for a tool.
     *
     * Displays a modal ToolTrustLevelDlg configured with the provided options and
     * prompt text for the specified tool name. If the dialog is accepted and a
     * value is selected, the chosen value and checkbox state are returned.
     *
     * @param toolname const wxString& The name of the tool to display in the dialog message.
     * @param options const std::vector<std::pair<wxString, wxString>>& The available trust-level options, as pairs of
     * display text and values.
     *
     * @return std::optional<std::pair<wxString, bool>> Returns the selected value and the checked state if the dialog
     * is accepted and a value is available; otherwise returns std::nullopt.
     *
     * @throws None. This function does not throw exceptions; it reports cancellation or missing selection by returning
     * std::nullopt.
     */
    std::optional<std::pair<wxString, bool>>
    ShowTrustLevelDialog(const wxString& toolname, const std::vector<std::pair<wxString, wxString>>& options);

    /**
     * @brief Adds Markdown files from a list of paths to the LLM context.
     *
     * Filters @p files to ".md" files and reads each one (over SFTP if the workspace is remote). Each file's
     * content is added to the system-message registry, keyed by its normalised full path so that files sharing a
     * name in different directories do not overwrite each other, and the registry is then committed to the
     * client. Finally, a success message listing the added files is printed in the chat window. Returns without
     * doing anything if no file could be read.
     *
     * These entries are not restored by ResetSystemMessagesToDefaults(), so they are dropped on "Clear Session".
     *
     * @param files wxArrayString The file paths to consider for addition to the context.
     *               Only files with a ".md" extension are processed.
     */
    void AddFilesToContext(const wxArrayString& files);

    /**
     * @brief a wrapper around OnFileViewFileContextMenu to allow external caller to invoke the event handler.
     */
    void UpdateFileContextMenu(clContextMenuEvent& event) { OnFileViewFileContextMenu(event); }

private:
    Manager() = default;
    ~Manager() override;

    /**
     * @brief Second-phase initialisation, run on the application's init-done event.
     *
     * Creates the (hidden) text-generation preview frame, registers the "generate comment" accelerator and
     * binds the editor and file-view context-menu handlers.
     */
    void CompleteInitialisation();

    /**
     * @brief Locates the assistant configuration file, creating or resetting it if needed.
     *
     * If the file is missing or cannot be parsed, or its version does not match the expected one, the old file
     * is backed up (as `<name>.old`) and replaced with the default configuration.
     *
     * @return the full path of the configuration file, or an IO error status if the default file could not be written
     */
    clStatusOr<wxString> CreateOrOpenConfig();

    /**
     * @brief Checks that a configuration can be parsed by the assistant library.
     *
     * @param content the configuration JSON to validate; if not provided, the content of the assistant
     *                configuration file is used
     * @return OK if the content is valid, NotFound if the file could not be read, or a ParseError status
     */
    clStatus ValidateConfigFile(std::optional<wxString> content = std::nullopt) const;

    /**
     * @brief Accounts for the tokens of a message and compacts the history when the threshold is reached.
     *
     * Adds the token count of @p msg to the running estimate (unless @p usingTempHistory is true, since such
     * requests do not affect the shared history). If the client's auto-compact threshold (0 disables it) is
     * reached, older tool responses are trimmed and wxEVT_LLM_AUTO_COMPACTED is fired. Called from the worker
     * thread.
     *
     * @param m_client the client whose history should be compacted
     * @param msg the message that just passed through the client
     * @param usingTempHistory true if the current request uses a temporary (no-history) context
     */
    void CompactIfNeeded(std::shared_ptr<assistant::ClientBase> m_client,
                         const std::string& msg,
                         bool usingTempHistory = false);

    /**
     * @brief Replaces placeholder tokens in a ThreadTask's prompt with their corresponding runtime values.
     *
     * This method substitutes templated placeholders (e.g., {{current_selection}}, {{current_file_fullpath}})
     * in the task's prompt with actual values obtained from the active editor and workspace context. If
     * context is unavailable (e.g., no active editor), placeholders are replaced with empty strings or
     * fallback values.
     *
     * @param task A reference to a ThreadTask object whose prompt field will be modified in place.
     *
     * @return void This function does not return a value.
     *
     * @note Supported placeholders include:
     *       - {{current_selection}}: Selected text in the active editor
     *       - {{current_file_fullpath}}: Full path of the current file (local or remote)
     *       - {{current_file_ext}}: File extension of the current file
     *       - {{current_file_dir}}: Directory containing the current file
     *       - {{current_file_name}}: Base name of the current file
     *       - {{current_file_lang}}: Programming language of the current file
     *       - {{current_file_content}}: Full text content of the current file
     *       - {{workspace_path}}: Root path of the current workspace
     *       - {{temp_dir}}: System temporary directory path
     *
     * @see ThreadTask
     * @see clGetManager()
     * @see clWorkspaceManager
     */
    void ReplacePlaceHolders(ThreadTask& task);

    /**
     * @brief Writes @p j (pretty-printed) to the assistant configuration file and reloads it in the editor.
     *
     * @return true on success; false if the file could not be written
     */
    bool WriteConfigFile(llm::json j);

    /** @brief Reloads the assistant configuration (restarting the client) and fires wxEVT_LLM_CONFIG_UPDATED. */
    void HandleConfigFileUpdated();

    /** @brief Reloads the global settings file (Config) and fires wxEVT_LLM_GLOBAL_CONFIG_UPDATED. */
    void HandleGlobalConfigFileUpdated();

    /** @brief Adds the "AI-Powered Options" sub-menu to the editor's context menu, if an LLM is available. */
    void OnEditorContextMenu(clContextMenuEvent& event);

    /** @brief Adds the "AI Options" sub-menu (add to context) to the file-view context menu when Markdown files are selected. */
    void OnFileViewFileContextMenu(clContextMenuEvent& event);

    /**
     * @brief Prepares a task and hands it to the worker thread.
     *
     * Restarts the worker if it is no longer running, expands the placeholders in the prompt, then queues the task.
     */
    void PostTask(ThreadTask task);

    /**
     * @brief Entry point of the worker thread.
     *
     * Pulls tasks from the queue and runs each one on the client, translating the client's callbacks into
     * wxEVT_LLM_* events sent to the task's event sink, until the client is interrupted or a fatal exception
     * occurs. Fires the worker busy/idle events around every task.
     */
    void WorkerMain();

    /** @brief Queues a task directly on the worker thread, without any preparation (see PostTask()). */
    void PushThreadWork(ThreadTask work) { m_queue.Post(std::move(work)); }

    /**
     * @brief Drains the task queue and detaches the worker thread. Main thread only.
     *
     * Every pending task is completed with a "LLM is going down" message. A fresh "is running" flag is created
     * so a new worker can be started.
     */
    void CleanupAfterWorkerExit();

    /**
     * @brief Detaches (rather than joins) the worker thread so an in-flight request cannot block the UI.
     *
     * @return true always
     */
    bool DetachWorkerThread();

    /**
     * @brief Builds the assistant library configuration from the assistant configuration file.
     *
     * Falls back to the built-in default configuration if the file cannot be opened or (on the main thread)
     * cannot be parsed. On first use, also routes the assistant library's log output to the LLM log.
     */
    assistant::Config MakeConfig();

    /** @brief Reloads the LLM configuration when the assistant config file, or the global settings file, is saved. */
    void OnFileSaved(clCommandEvent& event);

    /** @brief Loads the workspace's AGENTS.md / CLAUDE.md into the system messages when a workspace is opened. */
    void OnWorkspaceOpened(clWorkspaceEvent& event);

    /** @brief Removes the workspace's AGENTS.md / CLAUDE.md from the system messages when the workspace is closed. */
    void OnWorkspaceClosed(clWorkspaceEvent& event);

    /**
     * @brief Adds the open workspace's project instructions to the local system-message registry.
     *
     * Reads `AGENTS.md`, or `CLAUDE.md` if there is no `AGENTS.md`, and stores its content under the file's name.
     * Does nothing if neither file exists. Only affects local state; call CommitSystemMessage() afterwards.
     */
    void LoadWorkspaceContenxtFiles();

    /**
     * @brief Reads the assistant configuration file and parses it as JSON.
     *
     * Uses the default configuration if the file cannot be read.
     *
     * @return the parsed JSON, or std::nullopt if it cannot be parsed
     */
    std::optional<llm::json> GetConfigAsJSON();

    /**
     * @brief Callback invoked by the client before it runs a tool.
     *
     * Allows the call right away if the tool is fully trusted; otherwise asks the user (see
     * PromptUserYesNoTrustQuestion()) and, if they answer "trust", records the tool as trusted.
     */
    static CanInvokeToolResult CanRunTool(const std::string& tool_name, assistant::json args);

    std::unique_ptr<std::thread> m_worker_thread;
    std::shared_ptr<assistant::ClientBase> m_client;

    Config m_config;
    HistoryStore m_history;
    assistant::Config m_default_config;
    assistant::Config m_client_config;
    wxMessageQueue<ThreadTask> m_queue;
    std::shared_ptr<std::atomic_bool> m_worker_thread_running;
    std::atomic_bool m_worker_busy{false};
    FunctionTable m_plugin_functions;
    std::unique_ptr<ChatAI> m_chatAI{nullptr};
    std::vector<std::shared_ptr<std::atomic_bool>> m_termination_flags;
    mutable std::mutex m_mutex;
    std::atomic_bool m_initialise_called{false};
    TextGenerationPreviewFrame* m_commentGenerationView{nullptr};
    std::atomic_bool m_clientStopInProgress{false};
    std::atomic_size_t m_tokens{0};
    std::unordered_map<wxString, wxString> m_systemMessages;
};

/**
 * @brief RAII wrapper for automatic termination flag lifecycle management.
 *
 * This class provides automatic cleanup of termination flags created via
 * {@code llm::Manager::NewTerminationFlag()}. When constructed, it creates
 * and registers a new termination flag. When destroyed (going out of scope),
 * it automatically removes the flag from the manager's collection.
 *
 * This ensures that termination flags are properly cleaned up even in the
 * presence of exceptions or early returns, preventing memory leaks and
 * ensuring consistent resource management.
 *
 * @note The termination flag can be accessed via the {@code GetFlag()} method
 *       to check or set the termination state during the object's lifetime.
 *
 * @code
 * {
 *     TerminationFlagGuard guard; // creates and registers flag
 *     auto flag = guard.GetFlag();
 *     // ... use flag for task cancellation ...
 * } // flag is automatically removed from manager
 * @endcode
 *
 * @see llm::Manager::NewTerminationFlag()
 * @see llm::Manager::DeleteTerminationFlag()
 */
struct WXDLLIMPEXP_SDK TerminationFlagGuard {
    std::shared_ptr<std::atomic_bool> flag;
    TerminationFlagGuard()
        : flag(llm::Manager::GetInstance().NewTerminationFlag())
    {
    }
    ~TerminationFlagGuard() { llm::Manager::GetInstance().DeleteTerminationFlag(flag); }
    bool IsSet() const { return flag->load(); }
    std::shared_ptr<std::atomic_bool> GetFlag() const { return flag; }
};

/**
 * @brief RAII helper that temporarily disables all LLM functions.
 *
 * Constructing an instance of {@code FunctionsDisabler} disables all
 * functions via {@code llm::Manager::GetInstance().EnableAllFunctions(false)}.
 * When the instance goes out of scope, the destructor re-enables all functions
 * by calling {@code llm::Manager::GetInstance().EnableAllFunctions(true)}.
 *
 * This class is intended for use when a section of code requires the
 * LLM manager to operate with all functions disabled, ensuring that
 * the original state is restored automatically when the object is destroyed.
 *
 * @note The implementation relies on the singleton {@code llm::Manager}
 *       and assumes thread-safety of the EnableAllFunctions method.
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
