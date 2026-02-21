#include "LLMManager.hpp"

#include "FileManager.hpp"
#include "assistant/assistant.hpp"
#include "assistant/common/magic_enum.hpp"
#include "clWorkspaceManager.h"
#include "cl_command_event.h"
#include "cl_standard_paths.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"

#include <future>
#include <wx/msgdlg.h>
#include <wx/richmsgdlg.h>
#include <wx/xrc/xmlres.h>

namespace llm
{
namespace
{
constexpr float kConfigVersion = 1.0;
constexpr const char* kConfigVersionProperty = "_version";
constexpr std::string_view kClaudeSystemMessage = R"#()#";

/**
 * @brief RAII guard that restores an atomic boolean to a specified value upon destruction.
 *
 * This struct acquires a reference to an `std::atomic_bool` and remembers a target value
 * at construction time. When the `AtomiBoolLocker` is destroyed (goes out of scope), it
 * atomically stores the remembered value back into the boolean, ensuring cleanup even if
 * exceptions occur.
 *
 * @param b Reference to the `std::atomic_bool` to be managed.
 * @param v The value to restore to `b` when this locker is destroyed.
 *
 * @note This struct does not return a value; it is intended to be used as a scoped guard.
 * @note No exceptions are thrown by the constructor or destructor (atomic store is noexcept).
 *
 * @see std::atomic_bool
 * @see RAII (Resource Acquisition Is Initialization)
 */
struct AtomiBoolLocker {
    std::atomic_bool& bool_;
    bool value_;
    explicit AtomiBoolLocker(std::atomic_bool& b, bool v)
        : bool_{b}
        , value_{v}
    {
    }
    ~AtomiBoolLocker() { bool_.store(value_); }
};

wxString TruncateText(const wxString& text, size_t size = 100)
{
    if (text.size() >= size) {
        return text.Mid(0, size - 3) << "...";
    }
    return text;
}

static std::atomic_bool busy_cusrsor{false};
/**
 * @brief RAII wrapper for managing a wxBusyCursor instance with thread-safe activation.
 *
 * This struct ensures that a busy cursor (hourglass/wait cursor) is displayed only when
 * running on the main GUI thread and when no other BusyCursor is already active. The
 * cursor is automatically restored when the BusyCursor object goes out of scope.
 *
 * @note This struct relies on a global `busy_cusrsor` flag to prevent nested busy cursors.
 * @note The busy cursor is only activated if called from the main thread (checked via wxThread::IsMain()).
 *
 * @see wxBusyCursor
 */
struct BusyCursor {
    std::unique_ptr<wxBusyCursor> bc;
    BusyCursor()
    {
        if (wxThread::IsMain() && busy_cusrsor == false) {
            busy_cusrsor = true;
            bc = std::make_unique<wxBusyCursor>();
        }
    }
    ~BusyCursor()
    {
        if (bc) {
            busy_cusrsor = false;
        }
    }
};

struct TaskDropper {
    llm::ThreadTask& task;
    TaskDropper(llm::ThreadTask& t)
        : task{t}
    {
    }
    ~TaskDropper()
    {
        if (!task.collector) {
            return;
        }

        auto* manager = &llm::Manager::GetInstance();
        // Pass the collector to the main thread so it will deleted by the main thread.
        manager->CallAfter(&llm::Manager::DeleteCollector, task.collector);
        task.collector = nullptr;
    }
};

struct SharedState {
    /// used for detecting loops in the model.
    std::vector<std::string> last_tokens;
    size_t total_batch_count{1};
    size_t current_batch{1};
};

void NotifyDoneWithError(wxEvtHandler* owner, const std::string& message)
{
    CHECK_PTR_RET(owner);
    clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
    event.SetResponseRaw(message);
    event.SetIsError(true);
    owner->AddPendingEvent(event);
}

void NotifyRequestCancelled(wxEvtHandler* owner, const std::string& message)
{
    CHECK_PTR_RET(owner);
    clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
    event.SetResponseRaw(message);
    event.SetRequestCancelled(true);
    owner->AddPendingEvent(event);
}
} // namespace

constexpr const char* kPlaceHolderEditorSelection = "{{current_selection}}";
constexpr const char* kPlaceHolderCurrentFileFullPath = "{{current_file_fullpath}}";
constexpr const char* kPlaceHolderCurrentFileExt = "{{current_file_ext}}";
constexpr const char* kPlaceHolderCurrentFileDir = "{{current_file_dir}}";
constexpr const char* kPlaceHolderCurrentFileName = "{{current_file_name}}";
constexpr const char* kPlaceHolderCurrentFileLang = "{{current_file_lang}}";
constexpr const char* kPlaceHolderCurrentFileContent = "{{current_file_content}}";
constexpr const char* kPlaceHolderWorkspacePath = "{{workspace_path}}";
constexpr const char* kPlaceHolderTempDir = "{{temp_dir}}";

static std::vector<wxString> kPlaceHolders = {kPlaceHolderEditorSelection,
                                              kPlaceHolderCurrentFileFullPath,
                                              kPlaceHolderCurrentFileExt,
                                              kPlaceHolderCurrentFileDir,
                                              kPlaceHolderCurrentFileName,
                                              kPlaceHolderCurrentFileLang,
                                              kPlaceHolderCurrentFileContent,
                                              kPlaceHolderWorkspacePath,
                                              kPlaceHolderTempDir};

// ==---------------------
// Client
// ==---------------------

Manager& Manager::GetInstance()
{
    static Manager instance;
    return instance;
}

Manager::~Manager()
{
    Stop();
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &Manager::OnFileSaved, this);
}

void Manager::Initialise()
{
    m_chatAI = std::make_unique<ChatAI>();
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &Manager::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, [this](wxCommandEvent& e) {
        e.Skip();
        // Now that all the plugins are loaded, start the agent.
        Start();
    });
}

bool Manager::IsAvailable()
{
    // return true if we have an active endpoint.
    return GetActiveEndpoint().has_value();
}

void Manager::DeleteCollector(ResponseCollector* collector) { wxDELETE(collector); }

void Manager::WorkerMain()
{
    CHECK_PTR_RET(m_client);

    // Create our own copy to make sure that when reloading configuration, this instance
    // is un-affected.
    auto client = m_client;

    // Helper class to pass the collector to the main thread from the ThreadTask class
    // once it drops out of scope. This way we ensure that the collector will be destroyed
    // without pending events.
    clDEBUG() << "LLM Worker thread started" << endl;
    m_worker_thread_running.store(true);
    bool cont{true};
    while (cont && !client->IsInterrupted()) {
        m_worker_busy.store(false);
        ThreadTask task;
        auto res = m_queue.ReceiveTimeout(50, task);
        if (res == wxMSGQUEUE_TIMEOUT) {
            continue;
        }

        clDEBUG() << "Handling chat request:" << TruncateText(task.prompt_array[0]) << endl;
        m_worker_busy.store(true);
        TaskDropper dropper{task};

        // Process the the task
        bool saved_thinking_state{false};
        wxEvtHandler* owner = task.GetEventSink();

        clLLMEvent event_starting{wxEVT_LLM_CHAT_STARTED};
        owner->AddPendingEvent(event_starting);

        SharedState shared_state;
        shared_state.total_batch_count = task.prompt_array.size();
        shared_state.current_batch = 1;

        bool abort_loop{false};
        auto cancellation_token = task.cancellation_token;
        for (std::string prompt : task.prompt_array) {
            if (abort_loop) {
                break;
            }

            try {
                clDEBUG() << "Client (URL:" << client->GetUrl() << ", Model:" << client->GetModel()
                          << ") is processing the request" << endl;
                client->Chat(
                    std::move(prompt),
                    [client, cancellation_token, &abort_loop, &shared_state, &saved_thinking_state, owner](
                        std::string message, assistant::Reason reason, bool thinking) -> bool {
                        // Check various options that the chat was cancelled.
                        if (client->IsInterrupted() || (cancellation_token && cancellation_token->IsCancelled())) {
                            NotifyRequestCancelled(owner, "\n\n** Request cancelled by the user. **\n\n");
                            abort_loop = true;
                            return false;
                        }

                        if (saved_thinking_state != thinking) {
                            // we switched state
                            if (thinking) {
                                // the new state is "thinking"
                                clLLMEvent think_started{wxEVT_LLM_THINK_SATRTED};
                                owner->AddPendingEvent(think_started);
                            } else {
                                clLLMEvent think_ended{wxEVT_LLM_THINK_ENDED};
                                owner->AddPendingEvent(think_ended);
                            }
                        }

                        saved_thinking_state = thinking;
                        switch (reason) {
                        case assistant::Reason::kFatalError: {
                            NotifyDoneWithError(owner, message);
                            abort_loop = true;
                            return false;
                        } break;
                        case assistant::Reason::kDone: {
                            if (cancellation_token && !cancellation_token->Incr()) {
                                NotifyDoneWithError(owner, "\n\n** Maximum number of tokens reached. **\n\n");
                                abort_loop = true;
                                return false;
                            }
                            if (shared_state.current_batch == shared_state.total_batch_count) {
                                // No more batches to process, fire the DONE event.
                                clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
                                event.SetResponseRaw(message);
                                owner->AddPendingEvent(event);
                            } else {
                                // We have more batches to process, treat this as a
                                // normal "output" event.
                                shared_state.current_batch++;
                                clLLMEvent event{wxEVT_LLM_OUTPUT};
                                event.SetResponseRaw(message);
                                owner->AddPendingEvent(event);
                            }
                        } break;
                        case assistant::Reason::kLogNotice:
                            clDEBUG1() << message << endl;
                            break;
                        case assistant::Reason::kCancelled: {
                            NotifyRequestCancelled(owner, "\n\n** Request cancelled by caller. **\n\n");
                            abort_loop = true;
                            return false;
                        } break;
                        case assistant::Reason::kLogDebug:
                            clDEBUG() << message << endl;
                            break;
                        case assistant::Reason::kRequestCost:
                            clDEBUG() << message << endl;
                            break;
                        case assistant::Reason::kPartialResult: {
                            clLLMEvent event{wxEVT_LLM_OUTPUT};
                            event.SetResponseRaw(message);
                            owner->AddPendingEvent(event);
                            if (cancellation_token && !cancellation_token->Incr()) {
                                NotifyDoneWithError(owner, "\n\n** Maximum number of tokens reached. **\n\n");
                                abort_loop = true;
                                return false;
                            }
                        } break;
                        }
                        // continue
                        return true;
                    },
                    task.options);
            } catch (std::exception& e) {
                clERROR() << "LLM worker that got an exception." << e.what() << endl;
                wxString errmsg;
                errmsg << "\n\n** Request ended with an error. " << e.what() << " **\n\n";
                NotifyDoneWithError(owner, errmsg.ToStdString(wxConvUTF8));
                cont = false;
                break;
            }
        } // Prompt loop
    } // Main Loop
    clDEBUG() << "LLM Worker thread started - exiting." << endl;
    m_worker_thread_running.store(false);
    m_worker_busy.store(false);
}

void Manager::CleanupAfterWorkerExit()
{
    // Pull all items from the queue and notify about "Stop"
    if (!wxThread::IsMain()) {
        clERROR() << "CleanupAfterWorkerExit can only be called from the main thread" << endl;
        return;
    }

    ThreadTask w;
    while (m_queue.ReceiveTimeout(1, w) == wxMSGQUEUE_NO_ERROR) {
        TaskDropper dropper{w};
        clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
        event.SetResponseRaw("LLM is going down");
        w.GetEventSink()->AddPendingEvent(event);
    }

    m_queue.Clear();

    if (m_worker_thread) {
        m_worker_thread->join();
        m_worker_thread.reset();
    }
}

void Manager::PostTask(ThreadTask task)
{
    if (!m_worker_thread_running.load()) {
        clDEBUG() << "Restarting worker thread." << endl;
        CleanupAfterWorkerExit();
        Start();
    }

    ReplacePlaceHolders(task);
    m_queue.Post(std::move(task));
}

void Manager::ReplacePlaceHolders(ThreadTask& task)
{
    const std::vector<std::function<void(wxString&)>> kManipulators = {
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderEditorSelection, wxEmptyString);
                return;
            }
            prompt.Replace(kPlaceHolderEditorSelection, editor->GetSelection());
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileFullPath, wxEmptyString);
                return;
            }
            prompt.Replace(kPlaceHolderCurrentFileFullPath, editor->GetRemotePathOrLocal());
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileExt, wxEmptyString);
                return;
            }
            prompt.Replace(kPlaceHolderCurrentFileExt, editor->GetFileName().GetExt());
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileDir, ::wxGetCwd());
                return;
            }

            wxString dir;
            if (editor->IsRemoteFile()) {
                dir = editor->GetRemotePathOrLocal().BeforeLast('/');
            } else {
                dir = editor->GetFileName().GetPath();
            }
            prompt.Replace(kPlaceHolderCurrentFileDir, dir);
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileName, wxEmptyString);
                return;
            }

            wxString fullname;
            if (editor->IsRemoteFile()) {
                fullname = editor->GetRemotePathOrLocal().AfterLast('/');
            } else {
                fullname = editor->GetFileName().GetFullName();
            }
            prompt.Replace(kPlaceHolderCurrentFileName, fullname);
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileLang, wxEmptyString);
                return;
            }

            auto lang = FileExtManager::GetLanguageFromType(FileExtManager::GetType(editor->GetRemotePathOrLocal()));
            lang = lang.Lower();
            lang.Replace(" ", "");
            prompt.Replace(kPlaceHolderCurrentFileLang, lang);
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileContent, wxEmptyString);
                return;
            }
            prompt.Replace(kPlaceHolderCurrentFileContent, editor->GetEditorText());
        },
        [](wxString& prompt) {
            if (!clWorkspaceManager::Get().IsWorkspaceOpened()) {
                prompt.Replace(kPlaceHolderWorkspacePath, ::wxGetCwd());
                return;
            }
            prompt.Replace(kPlaceHolderWorkspacePath, clWorkspaceManager::Get().GetWorkspace()->GetDir());
        },
        [](wxString& prompt) { prompt.Replace(kPlaceHolderTempDir, clStandardPaths::Get().GetTempDir()); }};

    // Apply the manipulators on the prompt
    for (auto& p : task.prompt_array) {
        wxString prompt = wxString::FromUTF8(p);
        for (auto& manipulator : kManipulators) {
            manipulator(prompt);
            p = prompt.ToStdString(wxConvUTF8);
        }
    }
}

void Manager::Chat(wxEvtHandler* owner,
                   const wxArrayString& prompts,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    // Post 1 job with multiple prompts.
    ThreadTask task{.options = options, .owner = owner, .cancellation_token = cancel_token};
    for (const auto& prompt : prompts) {
        task.prompt_array.push_back(prompt.ToStdString(wxConvUTF8));
    }
    PostTask(std::move(task));
}

void Manager::Chat(ResponseCollector* collector,
                   const wxArrayString& prompts,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    // Post 1 job with multiple prompts.
    ThreadTask task{.options = options, .cancellation_token = cancel_token, .collector = collector};
    for (const auto& prompt : prompts) {
        task.prompt_array.push_back(prompt.ToStdString(wxConvUTF8));
    }
    PostTask(std::move(task));
}

void Manager::Chat(wxEvtHandler* owner,
                   const wxString& prompt,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    ThreadTask task{.prompt_array = {prompt.ToStdString(wxConvUTF8)},
                    .options = options,
                    .owner = owner,
                    .cancellation_token = cancel_token};
    PostTask(std::move(task));
}

void Manager::Chat(ResponseCollector* collector,
                   const wxString& prompt,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    ThreadTask task{.prompt_array = {prompt.ToStdString(wxConvUTF8)},
                    .options = options,
                    .cancellation_token = cancel_token,
                    .collector = collector};
    PostTask(std::move(task));
}

std::once_flag config_create_once_flag;
assistant::Config Manager::MakeConfig()
{
    std::call_once(config_create_once_flag, [this]() {
        // Should never fail.
        m_default_config = assistant::Config::FromContent(kDefaultSettings.ToStdString(wxConvUTF8)).value();

        // Redirect the library logs to our logging machinery.
        assistant::SetLogSink([](assistant::LogLevel level, std::string msg) {
            switch (level) {
            case assistant::LogLevel::kError:
                clERROR() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            case assistant::LogLevel::kWarning:
                clWARNING() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            case assistant::LogLevel::kInfo:
            case assistant::LogLevel::kDebug:
            case assistant::LogLevel::kTrace:
                clDEBUG1() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            }
        });
    });

    auto res = CreateOrOpenConfig();
    if (!res.ok()) {
        return m_default_config;
    }

    assistant::Config config =
        assistant::Config::FromFile(res.value().ToStdString(wxConvUTF8)).value_or(m_default_config);
    return config;
}

void Manager::Restart()
{
    Stop();
    Start();
}

void Manager::Stop()
{
    auto bc = std::make_unique<BusyCursor>();
    CHECK_PTR_RET(m_client);

    m_client->Interrupt();
    CleanupAfterWorkerExit();
    m_client.reset();

    clLLMEvent stop_event{wxEVT_LLM_STOPPED};
    stop_event.SetEventObject(this);
    AddPendingEvent(stop_event);
}

static std::unordered_set<std::string> allowed_tools;

bool Manager::CanRunTool(const std::string& tool_name)
{
    if (allowed_tools.contains(tool_name)) {
        return true;
    }

    wxString message;
    message << _("The model wants to run the tool: \"") << tool_name << _("\". Continue?");
    auto result = llm::Manager::GetInstance().PromptUserYesNoTrustQuestion(message, 30);
    if (!result.ok()) {
        return false;
    }

    switch (result.value()) {
    case llm::UserAnswer::kNo:
        return false;
    case llm::UserAnswer::kYes:
        return true;
    case llm::UserAnswer::kTrust:
        allowed_tools.insert(tool_name);
        return true;
    default:
        return false;
    }
}

void Manager::Start(std::shared_ptr<assistant::ClientBase> client)
{
    auto bc = std::make_unique<BusyCursor>();
    m_client_config = MakeConfig();

    // MakeClient that accepts a Config object can not fail.
    bool is_claude =
        m_client_config.GetEndpoint() && m_client_config.GetEndpoint()->type_ == assistant::EndpointKind::anthropic;
    m_client = client == nullptr ? assistant::MakeClient(m_client_config).value_or(nullptr) : client;
    CHECK_PTR_RET(m_client);

    // Initialise the function table.
    PopulateBuiltInFunctions(m_client->GetFunctionTable());
    m_client->GetFunctionTable().Merge(GetPluginFunctionTable());

    // Add external MCP tools.
    m_client->GetFunctionTable().ReloadMCPServers(&m_client_config);

    // Check if this is a stock model and associate the pricing for it.
    auto active_endpoint = m_client_config.GetEndpoint();
    if (active_endpoint) {
        auto pricing = assistant::FindPricing(active_endpoint->model_);
        if (pricing.has_value()) {
            m_client->SetPricing(pricing.value());
        } else {
            // TODO: add support for user's custom pricing.
        }
    }

    m_client->SetTookInvokeCallback(&Manager::CanRunTool);

    // Start the worker thread
    m_worker_thread = std::make_unique<std::thread>([this]() { WorkerMain(); });

    clLLMEvent start_event{wxEVT_LLM_STARTED};
    start_event.SetEventObject(this);
    AddPendingEvent(start_event);
}

bool Manager::ReloadConfig(std::optional<wxString> config_content, bool prompt)
{
    if (prompt && ::wxMessageBox(_("Reloading the configuration will restart "
                                   "the LLM client.\nContinue?"),
                                 "CodeLite",
                                 wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER) != wxYES) {
        return false;
    }

    // First stop the running instance
    Stop();

    wxString content;
    if (config_content.has_value()) {
        content = std::move(config_content.value());
    } else {
        const WriteOptions opts{.ignore_workspace = true};
        content = FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts).value_or(kDefaultSettings);
    }

    auto conf = assistant::Config::FromContent(content.ToStdString(wxConvUTF8));
    if (conf.has_value()) {
        m_client_config = std::move(conf.value());
        // Now we can replace the client instance
        auto client = assistant::MakeClient(m_client_config).value_or(nullptr);
        if (!client) {
            clERROR() << "Could not create new LLM client!" << endl;
            return false;
        }

        // Start the new client
        Start(client);
        return true;
    }
    return false;
}

void Manager::ClearHistory()
{
    CHECK_PTR_RET(m_client);
    m_client->ClearHistoryMessages();
}

llm::Conversation Manager::GetConversation() const
{
    if (!m_client) {
        return {};
    }
    return m_client->GetHistory();
}

void Manager::LoadConversation(const llm::Conversation& conversation)
{
    CHECK_PTR_RET(m_client);
    m_client->SetHistory(conversation.messages);
}

wxArrayString Manager::ListEndpoints()
{
    const auto& endpoints = m_client_config.GetEndpoints();
    wxArrayString result;
    result.reserve(endpoints.size());
    for (auto ep : endpoints) {
        result.Add(ep->url_);
    }
    return result;
}

bool Manager::SetActiveEndpoint(const wxString& endpoint)
{
    ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), false);
    if (!j.contains("endpoints")) {
        return false;
    }

    auto endpoints = j["endpoints"];
    bool endpoint_found{false};
    auto endpoints_items = endpoints.items();
    llm::json new_endpoints;
    for (auto& [name, val] : endpoints_items) {
        if (!endpoint_found) {
            endpoint_found = (name == endpoint);
        }
        val["active"] = (name == endpoint);
        new_endpoints[name] = val;
    }

    if (!endpoint_found) {
        return false;
    }

    j["endpoints"] = new_endpoints;
    if (WriteConfigFile(std::move(j))) {
        HandleConfigFileUpdated();
        return true;
    }
    return false;
}

std::optional<wxString> Manager::GetActiveEndpoint() const
{
    auto active_endpoint = m_client_config.GetEndpoint();
    if (!active_endpoint) {
        return std::nullopt;
    }
    return active_endpoint->url_;
}

std::optional<llm::json> Manager::GetConfigAsJSON()
{
    const WriteOptions opts{.ignore_workspace = true};
    wxString config_content =
        FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts).value_or(kDefaultSettings);
    try {
        return llm::json::parse(config_content.ToStdString(wxConvUTF8));

    } catch (const std::exception& e) {
        clERROR() << "Failed to parse JSON file:" << kAssistantConfigFile << "." << e.what() << endl;
        return std::nullopt;
    }
}

void Manager::AddNewEndpoint(const llm::EndpointData& d)
{
    try {
        ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), );
        if (!j.contains("endpoints")) {
            j["endpoints"] = {};
        }

        auto endpoints = j["endpoints"];
        if (endpoints.contains(d.url)) {
            endpoints.erase(d.url);
        }

        llm::json new_endpoint;
        new_endpoint["active"] = GetActiveEndpoint().has_value() ? false : true;

        if (d.client_type == kClientTypeAnthropic) {
            llm::json http_headers;
            http_headers["x-api-key"] = d.api_key.value_or("<INSERT_API_KEY>");
            new_endpoint["http_headers"] = http_headers;

        } else if (d.client_type == kClientTypeOllama && d.api_key.has_value()) {
            // Ollama cloud
            llm::json http_headers;
            http_headers["Authorization"] = "Bearer " + d.api_key.value();
            new_endpoint["http_headers"] = http_headers;

        } else if (d.client_type == kClientTypeOllama) {
            // Ollama local
            llm::json http_headers;
            http_headers["Host"] = "127.0.0.1";
            new_endpoint["http_headers"] = http_headers;
        }

        new_endpoint["type"] = d.client_type;
        new_endpoint["model"] = d.model;
        if (d.context_size.has_value()) {
            new_endpoint["context_size"] = d.context_size.value();
        }

        if (d.max_tokens.has_value()) {
            new_endpoint["max_tokens"] = d.max_tokens.value();
        }

        j["endpoints"][d.url] = new_endpoint;
        if (WriteConfigFile(std::move(j))) {
            HandleConfigFileUpdated();
        }

    } catch (const std::exception& e) {
        clERROR() << "Failed to add new endpoint:" << e.what() << endl;
    }
}

void Manager::AddNewMcp(const llm::SSEMcp& d)
{
    try {
        ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), );
        if (!j.contains("mcp_servers")) {
            j["mcp_servers"] = {};
        }

        auto mcp_servers = j["mcp_servers"];
        if (mcp_servers.contains(d.name)) {
            mcp_servers.erase(d.name);
        }

        llm::json new_mcp;
        new_mcp["type"] = "sse";
        new_mcp["enabled"] = true;
        new_mcp["baseurl"] = d.base_url;
        new_mcp["endpoint"] = d.endpoint;
        new_mcp["headers"] = d.headers;
        new_mcp["auth_token"] = d.auth_token;

        j["mcp_servers"][d.name] = new_mcp;
        if (WriteConfigFile(std::move(j))) {
            HandleConfigFileUpdated();
        }

    } catch (const std::exception& e) {
        clERROR() << "Failed to add new SSE mcp server:" << e.what() << endl;
    }
}

void Manager::AddNewMcp(const llm::LocalMcp& d)
{
    try {
        ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), );
        if (!j.contains("mcp_servers")) {
            j["mcp_servers"] = {};
        }

        auto mcp_servers = j["mcp_servers"];
        if (mcp_servers.contains(d.name)) {
            mcp_servers.erase(d.name);
        }

        llm::json new_mcp;
        new_mcp["type"] = "stdio";
        new_mcp["enabled"] = true;
        new_mcp["command"] = d.command;
        new_mcp["env"] = d.env;

        j["mcp_servers"][d.name] = new_mcp;
        if (WriteConfigFile(std::move(j))) {
            HandleConfigFileUpdated();
        }

    } catch (const std::exception& e) {
        clERROR() << "Failed to add new local mcp server:" << e.what() << endl;
    }
}

bool Manager::WriteConfigFile(llm::json j)
{
    const WriteOptions opts{.ignore_workspace = true};
    if (!FileManager::WriteSettingsFileContent(kAssistantConfigFile, wxString::FromUTF8(j.dump(2)), opts)) {
        clERROR() << "Failed to write configuration file:" << kAssistantConfigFile << endl;
        return false;
    }
    return true;
}

/**
 * @brief Handles the file saved event and triggers configuration reload if the assistant config
 * file was saved.
 *
 * This function is called when a file is saved in the editor. It checks if the saved file is the
 * LLM assistant configuration file, and if so, triggers a configuration update by calling
 * HandleConfigFileUpdated(). If the saved file is not the config file, the function returns early.
 *
 * @param event the command event containing information about the file save operation
 */
void Manager::OnFileSaved(clCommandEvent& event)
{
    event.Skip();

    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    const WriteOptions opts{.converter = nullptr, .ignore_workspace = true};
    wxString llm_config_file = FileManager::GetSettingFileFullPath(kAssistantConfigFile, opts);

    wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
    if (filepath != llm_config_file) {
        return;
    }

    HandleConfigFileUpdated();
}

/**
 * @brief Handles configuration file update events by reloading the configuration and notifying
 * listeners.
 *
 * This method is called when the configuration file has been updated. It displays a busy cursor,
 * reloads the configuration from the LLM Manager, and if successful, creates and dispatches
 * a wxEVT_LLM_CONFIG_UPDATED event to notify other components of the configuration change.
 */
void Manager::HandleConfigFileUpdated()
{
    // Reload configuration
    wxBusyCursor bc{};
    if (!llm::Manager::GetInstance().ReloadConfig(std::nullopt, false)) {
        return;
    }

    clLLMEvent event_config_updates{wxEVT_LLM_CONFIG_UPDATED};
    event_config_updates.SetEventObject(this);
    AddPendingEvent(event_config_updates);
}

/**
 * @brief Creates or opens the assistant configuration file, ensuring it is valid and up-to-date.
 *
 * This function attempts to locate and validate the global assistant configuration file.
 * If the file doesn't exist, is invalid, or has an outdated version, it creates a backup
 * (if an old file exists) and generates a new configuration file with default settings.
 * The function performs version checking and ensures the configuration matches the expected format.
 *
 * @return clStatusOr<wxString> On success, returns the full path to the configuration file.
 *                              On failure, returns a status error with an appropriate message.
 */
clStatusOr<wxString> Manager::CreateOrOpenConfig()
{
    const WriteOptions opts{.ignore_workspace = true};
    wxString global_config_path = FileManager::GetSettingFileFullPath(kAssistantConfigFile, opts);
    wxString backup_file_path = global_config_path + ".old";
    bool valid_file{false};
    try {
        auto conf = assistant::Config::FromFile(global_config_path.ToStdString(wxConvUTF8));
        valid_file = conf.has_value();
    } catch (const std::exception& e) {
        valid_file = false;
    }

    if (!valid_file) {
        // Backup old file
        if (wxFileName::FileExists(global_config_path)) {
            wxLogNull nl;
            ::wxCopyFile(global_config_path, backup_file_path);
        }

        // Invalid file: could not parse it or missing - create a default file.
        if (!FileUtils::WriteFileContent(global_config_path, kDefaultSettings)) {
            wxString messgage;
            messgage << _("Failed to create configuration file:\n") << global_config_path;
            ::wxMessageBox(messgage, "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
            return StatusIOError(messgage);
        }
        return global_config_path;
    }

    // A valid file was found - check its version.
    ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), StatusOther("Failed to parse file as JSON"));

    if (!j.contains(kConfigVersionProperty) || !j[kConfigVersionProperty].is_number_float() ||
        (j[kConfigVersionProperty].get<float>() != kConfigVersion)) {
        // Backup old file
        if (wxFileName::FileExists(global_config_path)) {
            wxLogNull nl;
            ::wxCopyFile(global_config_path, backup_file_path);
        }

        // An old version - create a default file.
        if (!FileUtils::WriteFileContent(global_config_path, kDefaultSettings)) {
            wxString messgage;
            messgage << _("Failed to create configuration file:\n") << global_config_path;
            ::wxMessageBox(messgage, "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
            return StatusIOError(messgage);
        }
    }
    return global_config_path;
}

void Manager::OpenSettingsFileInEditor()
{
    auto res = CreateOrOpenConfig();
    if (!res.ok()) {
        clERROR() << res.error_message() << endl;
        return;
    }

    clGetManager()->OpenFile(res.value());
}

void Manager::EnableAllFunctions(bool b)
{
    if (!m_client) {
        clWARNING() << "Failed to call EnableAllFunctions(" << b << "). Null client" << endl;
        return;
    }
    clDEBUG() << "Successfully" << (b ? "enabled" : "disabled") << "all functions" << endl;
    m_client->GetFunctionTable().EnableAll(b);
}

void Manager::EnableFunctionByName(const wxString& name, bool b)
{
    if (!m_client) {
        clWARNING() << "Failed to call EnableFunctionByName(" << name << ", " << b << "). Null client" << endl;
        return;
    }
    m_client->GetFunctionTable().EnableFunction(name.ToStdString(wxConvUTF8), b);
}

void Manager::ShowChatWindow(const wxString& prompt) { m_chatAI->ShowChatWindow(prompt); }

void Manager::ShowTextGenerationDialog(const wxString& prompt,
                                       std::shared_ptr<TextGenerationPreviewFrame> preview_frame,
                                       std::optional<assistant::ChatOptions> chat_options,
                                       std::function<void()> completion_callback)
{
    assistant::ChatOptions opts{assistant::ChatOptions::kNoTools};
    assistant::AddFlagSet(opts, assistant::ChatOptions::kNoHistory);

    if (chat_options.has_value()) {
        opts = std::move(chat_options.value());
    }

    preview_frame->Show();
    preview_frame->StartProgress(_("Working..."));

    auto collector = new llm::ResponseCollector();
    collector->SetStateChangingCB([preview_frame](llm::ChatState state) {
        if (!wxThread::IsMain()) {
            clWARNING() << "StateChangingCB called for non main thread!" << endl;
            return;
        }
        switch (state) {
        case llm::ChatState::kThinking:
            preview_frame->UpdateProgress(_("Thinking..."));
            break;
        case llm::ChatState::kWorking:
            preview_frame->UpdateProgress(_("Working..."));
            break;
        case llm::ChatState::kReady:
            preview_frame->StopProgress(_("Ready"));
            break;
        }
    });

    collector->SetStreamCallback([preview_frame, completion_callback = std::move(completion_callback)](
                                     const std::string& message, llm::StreamCallbackReason reason) {
        if (llm::IsFlagSet(reason, llm::StreamCallbackReason::kCancelled)) {
            if (completion_callback) {
                completion_callback();
            }
            return;
        }

        bool is_done = llm::IsFlagSet(reason, llm::StreamCallbackReason::kDone);

        preview_frame->AppendText(wxString::FromUTF8(message));
        if (is_done && !preview_frame->IsShown()) {
            preview_frame->Show();
        }

        if (is_done && completion_callback) {
            completion_callback();
        }
    });

    Chat(collector, prompt, nullptr, opts);
}

const std::vector<wxString>& Manager::GetAvailablePlaceHolders() const { return kPlaceHolders; }

clStatusOr<UserAnswer> llm::Manager::PromptUserYesNoTrustQuestion(const wxString& text,
                                                                  int timeout_secs,
                                                                  const wxString& code_block,
                                                                  const wxString& code_block_lang)
{
    if (::wxIsMainThread()) {
        return StatusOther("Function must not be called from the main thread");
    }

    bool expected{false};
    if (!m_waitingForAnswer.compare_exchange_strong(expected, true)) {
        return StatusResourceBusy("CodeLite is already pending response from the user");
    }

    // Ensure m_waitingForAnswer is set to "false" when we leave this function.
    AtomiBoolLocker locker{m_waitingForAnswer, false};

    // GUI manipulations must be done on the main thread.
    auto func = [text, code_block, code_block_lang, this]() {
        if (!code_block.empty()) {
            wxString prompt;
            prompt << "```" << code_block_lang << "\n" << code_block << "\n```\n";
            GetChatWindowContainer()->AppendTextAndStyle(prompt);
        }
        GetChatWindowContainer()->GetChatWindow()->ShowYesNoTrustBar(text);
    };
    EventNotifier::Get()->RunOnMain<void>(func);

    // Now wait for the answer
    UserAnswer answer{UserAnswer::kNo};
    auto result = m_answerChannel.ReceiveTimeout(timeout_secs * 1000, answer);
    switch (result) {
    case wxMSGQUEUE_NO_ERROR:
        // Use clicked something
        return answer;
    case wxMSGQUEUE_TIMEOUT: {
        clLLMEvent prompt_error{wxEVT_LLM_USER_REPLY_TIMEOUT};
        AddPendingEvent(prompt_error);
        return StatusTimeout(_("User did not reply in a timely manner. Try again later."));
    }
    default:
    case wxMSGQUEUE_MISC_ERROR: {
        clLLMEvent prompt_error{wxEVT_LLM_USER_REPLY_ERROR};
        AddPendingEvent(prompt_error);
        return StatusOther(_("Failed to read response from user"));
    }
    }
}

void llm::Manager::PostAnswer(UserAnswer answer) { m_answerChannel.Post(answer); }
} // namespace llm
