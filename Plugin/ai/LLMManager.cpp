#include "LLMManager.hpp"

#include "file_logger.h"
#include "ollama/client.hpp"

#include <wx/msgdlg.h>

namespace llm
{
namespace
{

struct SharedState {
    /// used for detecting loops in the model.
    std::vector<std::string> last_tokens;
    size_t total_batch_count{1};
    size_t current_batch{1};
};
} // namespace

Manager& Manager::GetInstance()
{
    static Manager instance;
    return instance;
}

Manager::Manager() { Start(); }

Manager::~Manager() { Stop(); }

void Manager::WorkerMain()
{
    clDEBUG() << "LLM Worker thread started" << endl;
    while (!m_client->IsInterrupted()) {
        ThreadTask task;
        auto res = m_queue.ReceiveTimeout(50, task);
        if (res == wxMSGQUEUE_TIMEOUT) {
            continue;
        }

        // Process the the task
        bool saved_thinking_state{false};
        wxEvtHandler* owner = task.owner;

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
                [cancellation_token, &abort_loop, &shared_state, &saved_thinking_state, owner](
                    std::string message, ollama::Reason reason, bool thinking) -> bool {
                    // Cancelled by user?
                    if (cancellation_token && cancellation_token->IsCancelled()) {
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
                        clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
                        event.SetResponseRaw(message);
                        event.SetIsError(true);
                        owner->AddPendingEvent(event);
                        abort_loop = true;
                    } break;
                    case ollama::Reason::kDone: {
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

                            if (cancellation_token && !cancellation_token->Incr()) {
                                return false;
                            }
                        }
                    } break;
                    case ollama::Reason::kLogNotice:
                    case ollama::Reason::kCancelled:
                        clDEBUG() << message << endl;
                        break;
                    case ollama::Reason::kLogDebug:
                        clDEBUG1() << message << endl;
                        break;
                    case ollama::Reason::kPartialResult: {
                        clLLMEvent event{wxEVT_LLM_OUTPUT};
                        event.SetResponseRaw(message);
                        owner->AddPendingEvent(event);
                        if (cancellation_token && !cancellation_token->Incr()) {
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
    m_queue.Post(std::move(task));
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
    m_queue.Post(std::move(task));
}

void Manager::Stop()
{
    m_client->Interrupt();
    if (m_worker_thread) {
        m_worker_thread->join();
        m_worker_thread.reset();
    }

    // Pull all items from the queue and notify about "Stop"
    ThreadTask w;
    while (m_queue.ReceiveTimeout(1, w) == wxMSGQUEUE_NO_ERROR) {
        clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
        event.SetResponseRaw("LLM is going down");
        w.owner->AddPendingEvent(event);
    }
    m_queue.Clear();
    m_client.reset();

    {
        std::scoped_lock lk{m_models_mutex};
        m_activeModel.clear();
        m_models.clear();
    }
}

void Manager::Start()
{
    std::unordered_map<std::string, std::string> headers = {{"Host", "127.0.0.1"}};
    m_client = std::make_shared<ollama::Client>(std::string{"127.0.0.1:11434"}, headers);

    // Start the worker thread
    m_worker_thread = std::make_unique<std::thread>([this]() { WorkerMain(); });
}

wxArrayString Manager::LoadModels()
{
    auto models = m_client->List();
    wxArrayString m;
    m.reserve(models.size());
    for (const auto& model : models) {
        m.Add(wxString::FromUTF8(model));
    }

    std::scoped_lock lk{m_models_mutex};
    m_models.swap(m);
    return m_models;
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
        m_client->ApplyConfig(&conf.value());
        Restart();
        return true;
    }
    return false;
}
} // namespace llm
