#include "LLMManager.hpp"

#include "file_logger.h"
#include "ollama/client.hpp"

#include <wx/msgdlg.h>
namespace llm
{
namespace
{

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
    clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
    event.SetResponseRaw(message);
    event.SetIsError(true);
    owner->AddPendingEvent(event);
}
} // namespace

// ==---------------------
// Client
// ==---------------------

std::unique_ptr<ollama::Client> Client::NewClient()
{
    std::unique_ptr<ollama::Client> client = std::make_unique<ollama::Client>(GetUrl(), m_http_headers.get_value());
    return client;
}

Manager& Manager::GetInstance()
{
    static Manager instance;
    return instance;
}

Manager::Manager() { Start(); }

Manager::~Manager() { Stop(); }

void Manager::DeleteCollector(ResponseCollector* collector) { wxDELETE(collector); }

void Manager::WorkerMain()
{
    // Helper class to pass the collector to the main thread from the ThreadTask class
    // once it drops out of scope. This way we ensure that the collector will be destroyed
    // without pending events.
    clDEBUG() << "LLM Worker thread started" << endl;
    m_worker_thread_running.store(true);
    while (!m_client->IsInterrupted()) {
        m_worker_busy.store(false);
        ThreadTask task;
        auto res = m_queue.ReceiveTimeout(50, task);
        if (res == wxMSGQUEUE_TIMEOUT) {
            continue;
        }
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

            m_client->Chat(
                std::move(prompt),
                [this, cancellation_token, &abort_loop, &shared_state, &saved_thinking_state, owner](
                    std::string message, ollama::Reason reason, bool thinking) -> bool {
                    // Check various options that the chat was cancelled.
                    if (m_client->IsInterrupted() || (cancellation_token && cancellation_token->IsCancelled())) {
                        NotifyDoneWithError(owner, "\n\n** Request cancelled by caller. **\n\n");
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
                    case ollama::Reason::kFatalError: {
                        NotifyDoneWithError(owner, message);
                        abort_loop = true;
                        return false;
                    } break;
                    case ollama::Reason::kDone: {
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
                    case ollama::Reason::kLogNotice:
                        clDEBUG() << message << endl;
                        break;
                    case ollama::Reason::kCancelled: {
                        NotifyDoneWithError(owner, "\n\n** Request cancelled by caller. **\n\n");
                        abort_loop = true;
                        return false;
                    } break;
                    case ollama::Reason::kLogDebug:
                        clDEBUG1() << message << endl;
                        break;
                    case ollama::Reason::kPartialResult: {
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
                task.model,
                task.options);
        }
    }
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
                   const wxString& prompt_template,
                   const wxArrayString& prompt_context_arr,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options,
                   const wxString& model)
{
    // Post 1 job with multiple prompts.
    ThreadTask task{
        .model = model.ToStdString(wxConvUTF8), .options = options, .owner = owner, .cancellation_token = cancel_token};
    for (const auto& prompt_context : prompt_context_arr) {
        wxString prompt = prompt_template;
        prompt.Replace("{{CONTEXT}}", prompt_context);
        task.prompt_array.push_back(prompt.ToStdString(wxConvUTF8));
    }
    PostTask(std::move(task));
}

void Manager::Chat(ResponseCollector* collector,
                   const wxString& prompt_template,
                   const wxArrayString& prompt_context_arr,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options,
                   const wxString& model)
{
    // Post 1 job with multiple prompts.
    ThreadTask task{.model = model.ToStdString(wxConvUTF8),
                    .options = options,
                    .cancellation_token = cancel_token,
                    .collector = collector};
    for (const auto& prompt_context : prompt_context_arr) {
        wxString prompt = prompt_template;
        prompt.Replace("{{CONTEXT}}", prompt_context);
        task.prompt_array.push_back(prompt.ToStdString(wxConvUTF8));
    }
    PostTask(std::move(task));
}

void Manager::Chat(wxEvtHandler* owner,
                   const wxString& prompt,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options,
                   const wxString& model)
{
    ThreadTask task{.prompt_array = {prompt.ToStdString(wxConvUTF8)},
                    .model = model.ToStdString(wxConvUTF8),
                    .options = options,
                    .owner = owner,
                    .cancellation_token = cancel_token};
    PostTask(std::move(task));
}

void Manager::Chat(ResponseCollector* collector,
                   const wxString& prompt,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options,
                   const wxString& model)
{
    ThreadTask task{.prompt_array = {prompt.ToStdString(wxConvUTF8)},
                    .model = model.ToStdString(wxConvUTF8),
                    .options = options,
                    .cancellation_token = cancel_token,
                    .collector = collector};
    PostTask(std::move(task));
}

void Manager::Restart()
{
    Stop();
    Start();
}

void Manager::Stop()
{
    auto bc = CreateBusyCursor();

    m_client->Interrupt();
    CleanupAfterWorkerExit();
    m_client.reset();
    {
        std::scoped_lock lk{m_models_mutex};
        m_activeModel.clear();
        m_models.clear();
    }
}

void Manager::Start()
{
    auto bc = CreateBusyCursor();

    std::unordered_map<std::string, std::string> headers = {{"Host", "127.0.0.1"}};
    m_client = std::make_shared<llm::Client>(std::string{"127.0.0.1:11434"}, headers);
    const ollama::Config* config = m_client_config.has_value() ? &m_client_config.value() : nullptr;
    if (config) {
        m_client->ApplyConfig(config);
    }

    // Initialise the function table.
    PopulateBuiltInFunctions(m_client->GetFunctionTable());
    m_client->GetFunctionTable().Merge(GetPluginFunctionTable());
    if (config) {
        // Add external MCP tools.
        m_client->GetFunctionTable().ReloadMCPServers(config);
    }
    LoadModels(nullptr);

    // Start the worker thread
    m_worker_thread = std::make_unique<std::thread>([this]() { WorkerMain(); });
}

void Manager::LoadModels(wxEvtHandler* owner)
{
    auto client = m_client->NewClient();
    std::thread t([owner, client = std::move(client)]() {
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

bool Manager::ReloadConfig(const wxString& config_content, bool prompt)
{
    if (prompt && ::wxMessageBox(_("Reloading the configuration will restart "
                                   "the LLM client.\nContinue?"),
                                 "CodeLite",
                                 wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER) != wxYES) {
        return false;
    }

    auto conf = ollama::Config::FromContent(config_content.ToStdString(wxConvUTF8));
    if (conf.has_value()) {
        m_client_config = std::move(conf);
        m_client->ApplyConfig(&m_client_config.value());
        Restart();
        return true;
    }
    return false;
}

void Manager::ClearHistory()
{
    CHECK_PTR_RET(m_client);
    m_client->ClearHistoryMessages();
}
} // namespace llm
