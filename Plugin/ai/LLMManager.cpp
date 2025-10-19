#include "LLMManager.hpp"

#include "FileManager.hpp"
#include "JSON.h"
#include "assistant/assistant.hpp"
#include "assistant/claude_client.hpp"
#include "assistant/ollama_client.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"

#include <wx/choicdlg.h>
#include <wx/msgdlg.h>

namespace llm
{
namespace
{

wxString TruncateText(const wxString& text, size_t size = 100)
{
    if (text.size() >= size) {
        return text.Mid(0, size - 3) << "...";
    }
    return text;
}

/// If Running on the main thread, construct a busy cursor.
std::unique_ptr<wxBusyCursor> CreateBusyCursor()
{
    std::unique_ptr<wxBusyCursor> bc;
    if (wxThread::IsMain()) {
        return std::make_unique<wxBusyCursor>();
    }
    return nullptr;
}

struct TaskDropper {
    llm::ThreadTask& task;
    TaskDropper(llm::ThreadTask& t) : task{t} {}
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
} // namespace

// ==---------------------
// Client
// ==---------------------

Manager& Manager::GetInstance()
{
    static Manager instance;
    return instance;
}

Manager::Manager()
{
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &Manager::OnFileSaved, this);
    Start();
}

Manager::~Manager()
{
    Stop();
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &Manager::OnFileSaved, this);
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
                            NotifyDoneWithError(owner, "\n\n** Request cancelled by the user. **\n\n");
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
                            clDEBUG() << message << endl;
                            break;
                        case assistant::Reason::kCancelled: {
                            NotifyDoneWithError(owner, "\n\n** Request cancelled by caller. **\n\n");
                            abort_loop = true;
                            return false;
                        } break;
                        case assistant::Reason::kLogDebug:
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
    m_queue.Post(std::move(task));
}

void Manager::Chat(wxEvtHandler* owner,
                   const wxArrayString& prompts,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    // Post 1 job with multiple prompts.
    ThreadTask task{.options = options, .owner = owner, .cancellation_token = cancel_token};
    for (const auto& prompt : prompts) { task.prompt_array.push_back(prompt.ToStdString(wxConvUTF8)); }
    PostTask(std::move(task));
}

void Manager::Chat(ResponseCollector* collector,
                   const wxArrayString& prompts,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    // Post 1 job with multiple prompts.
    ThreadTask task{.options = options, .cancellation_token = cancel_token, .collector = collector};
    for (const auto& prompt : prompts) { task.prompt_array.push_back(prompt.ToStdString(wxConvUTF8)); }
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
                clDEBUG() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            case assistant::LogLevel::kDebug:
                clDEBUG() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            case assistant::LogLevel::kTrace:
                clDEBUG1() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            }
        });
    });

    WriteOptions opts{.force_global = true};
    wxString config_content =
        FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts).value_or(kDefaultSettings);
    assistant::Config config =
        assistant::Config::FromContent(config_content.ToStdString(wxConvUTF8)).value_or(m_default_config);
    return config;
}

void Manager::Restart()
{
    Stop();
    Start();
}

void Manager::Stop()
{
    auto bc = CreateBusyCursor();

    if (m_client) {
        m_client->Interrupt();
        CleanupAfterWorkerExit();
        m_client.reset();
    }
    std::scoped_lock lk{m_models_mutex};
    m_models.clear();
}

void Manager::Start()
{
    auto bc = CreateBusyCursor();

    m_client_config = MakeConfig();

    // MakeClient that accepts a Config object can not fail.
    m_client = assistant::MakeClient(m_client_config).value_or(nullptr);
    CHECK_PTR_RET(m_client);

    // Initialise the function table.
    PopulateBuiltInFunctions(m_client->GetFunctionTable());
    m_client->GetFunctionTable().Merge(GetPluginFunctionTable());

    // Add external MCP tools.
    m_client->GetFunctionTable().ReloadMCPServers(&m_client_config);
    LoadModels(nullptr);

    // Start the worker thread
    m_worker_thread = std::make_unique<std::thread>([this]() { WorkerMain(); });
}

void Manager::LoadModels(wxEvtHandler* owner)
{
    WriteOptions opts{.force_global = true};
    wxString config_content =
        FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts).value_or(kDefaultSettings);
    auto client = assistant::MakeClient(config_content.ToStdString(wxConvUTF8));
    if (!client.has_value()) {
        clERROR() << "Could not construct a client for fetching model list." << endl;
        return;
    }

    std::thread t([owner, client = std::move(client.value())]() {
        auto models = client->List();
        wxArrayString m;
        m.reserve(models.size());
        for (const auto& model : models) { m.Add(wxString::FromUTF8(model)); }
        llm::Manager::GetInstance().SetModels(m);

        // Notify
        if (owner) {
            clLLMEvent models_loaded_event{wxEVT_LLM_MODELS_LOADED};
            models_loaded_event.SetStrings(m);
            owner->AddPendingEvent(models_loaded_event);
        }
    });
    t.detach();
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
        const WriteOptions opts{.force_global = true};
        content = FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts).value_or(kDefaultSettings);
    }

    auto conf = assistant::Config::FromContent(content.ToStdString(wxConvUTF8));
    if (conf.has_value()) {
        m_client_config = std::move(conf.value());
        // Now we can replace the client instance
        m_client = assistant::MakeClient(m_client_config).value_or(nullptr);
        if (!m_client) {
            clERROR() << "Could not create new LLM client!" << endl;
            return false;
        }

        // Start the new client
        Start();
        return true;
    }
    return false;
}

void Manager::ClearHistory()
{
    CHECK_PTR_RET(m_client);
    m_client->ClearHistoryMessages();
}

void Manager::SetModels(const wxArrayString& models)
{
    std::scoped_lock lk{m_models_mutex};
    m_models = models;
}

wxArrayString Manager::GetModels() const
{
    std::scoped_lock lk{m_models_mutex};
    return m_models;
}

wxArrayString Manager::ListEndpoints()
{
    const auto& endpoints = m_client_config.GetEndpoints();
    wxArrayString result;
    result.reserve(endpoints.size());
    for (auto ep : endpoints) { result.Add(ep->url_); }
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

std::optional<wxString> Manager::GetActiveEndpoint()
{
    auto active_endpoint = m_client_config.GetEndpoint();
    if (!active_endpoint) {
        return std::nullopt;
    }
    return active_endpoint->url_;
}

std::optional<llm::json> Manager::GetConfigAsJSON()
{
    const WriteOptions opts{.force_global = true};
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
        if (!endpoints.contains(d.url)) {
            endpoints.erase(d.url);
        }

        llm::json new_endpoint;
        new_endpoint["active"] = GetActiveEndpoint().has_value() ? false : true;

        if (d.provider == "anthropic") {
            llm::json http_headers;
            http_headers["x-api-key"] = d.api_key.value_or("<INSERT_API_KEY>");
            new_endpoint["http_headers"] = http_headers;
        } else if (d.provider == "ollama") {
            llm::json http_headers;
            http_headers["Host"] = "127.0.0.1";
            new_endpoint["http_headers"] = http_headers;
        }

        new_endpoint["type"] = d.provider;
        new_endpoint["model"] = d.model;
        new_endpoint["context_size"] = d.context_size.value_or(4 * 1024);

        j["endpoints"][d.url] = new_endpoint;
        if (WriteConfigFile(std::move(j))) {
            HandleConfigFileUpdated();
        }

    } catch (const std::exception& e) {
        clERROR() << "Failed to add new endpoint:" << e.what() << endl;
    }
}

bool Manager::WriteConfigFile(llm::json j)
{
    const WriteOptions opts{.force_global = true};
    if (!FileManager::WriteSettingsFileContent(kAssistantConfigFile, wxString::FromUTF8(j.dump(2)), opts)) {
        clERROR() << "Failed to write configuration file:" << kAssistantConfigFile << endl;
        return false;
    }
    return true;
}

void Manager::OnFileSaved(clCommandEvent& event)
{
    event.Skip();

    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    const WriteOptions opts{.converter = nullptr, .force_global = true};
    wxString llm_config_file = FileManager::GetSettingFileFullPath(kAssistantConfigFile, opts);

    wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
    if (filepath != llm_config_file) {
        return;
    }

    HandleConfigFileUpdated();
}

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

} // namespace llm
