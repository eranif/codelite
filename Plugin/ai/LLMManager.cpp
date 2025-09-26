#include "LLMManager.hpp"

#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"

#include <atomic> // atomic_uint64_t
namespace llm
{
namespace
{
std::atomic_uint64_t id_generator{0};
}

Manager& Manager::GetInstance()
{
    static Manager instance;
    return instance;
}

Manager::Manager()
    : m_client("http://127.0.0.1:11434", {{"Host", "127.0.0.1"}})
{
    Bind(wxEVT_LLM_RESPONSE, &Manager::OnResponse, this);
    Bind(wxEVT_LLM_RESPONSE_COMPLETED, &Manager::OnResponse, this);
    Bind(wxEVT_LLM_RESPONSE_ERROR, &Manager::OnResponseError, this);
    Bind(wxEVT_LLM_RESPONSE_ABORTED, &Manager::OnResponseAborted, this);
}

Manager::~Manager()
{
    Unbind(wxEVT_LLM_RESPONSE, &Manager::OnResponse, this);
    Unbind(wxEVT_LLM_RESPONSE_COMPLETED, &Manager::OnResponse, this);
    Unbind(wxEVT_LLM_RESPONSE_ERROR, &Manager::OnResponseError, this);
    Unbind(wxEVT_LLM_RESPONSE_ABORTED, &Manager::OnResponseAborted, this);
}

namespace
{
struct SharedState {
    /// used for detecting loops in the model.
    std::vector<std::string> last_tokens;
    size_t total_batch_count{1};
    size_t current_batch{1};
};
} // namespace

void Manager::Chat(const wxString& prompt_template,
                   const wxArrayString& prompt_context_arr,
                   OnResponseCallback response_cb,
                   size_t options,
                   const wxString& model)
{
    std::shared_ptr<OnResponseCallback> shared_cb = std::make_shared<OnResponseCallback>(response_cb);
    std::shared_ptr<SharedState> shared_state = std::make_shared<SharedState>();
    shared_state->total_batch_count = prompt_context_arr.size();

    for (const auto& prompt_context : prompt_context_arr) {
        wxString prompt = prompt_template;
        prompt.Replace("{{CONTEXT}}", prompt_context);
        Chat(
            prompt,
            [=](const std::string& message, llm::Reason reason, bool thinking) -> bool {
                if (reason == llm::Reason::kDone) {
                    if (shared_state->total_batch_count == shared_state->current_batch) {
                        // this was the last token in the batch.
                        return response_cb(message, reason, thinking);
                    } else {
                        shared_state->current_batch++;
                        return true;
                    }
                } else {
                    return response_cb(message, reason, thinking);
                }
            },
            options,
            model);
    }
}

void Manager::Chat(const wxString& prompt, OnResponseCallback cb, size_t options, const wxString& model)
{
    if (!IsAvailable()) {
        return;
    }

    m_client.Chat(prompt.ToStdString(wxConvUTF8),
                  std::move(cb),
                  model.ToStdString(wxConvUTF8),
                  static_cast<llm::ChatOptions>(options));
}

void Manager::OnResponse(clLLMEvent& event)
{
    bool completed = event.GetEventType() == wxEVT_LLM_RESPONSE_COMPLETED;
    bool is_thinking = event.IsThinking();
    if (m_requetstQueue.empty()) {
        clWARNING() << "Received LLM response, but no callback is available";
        RestartClient();
        return;
    }

    size_t flags{ResponseFlags::kNone};
    if (completed) {
        flags |= ResponseFlags::kCompleted;
    }
    if (is_thinking) {
        flags |= ResponseFlags::kThinking;
    }

    auto& entry = m_requetstQueue.front();
    if (entry.second != nullptr) {
        (entry.second)(event.GetResponseRaw(), flags);
    }

    if (completed) {
        m_requetstQueue.erase(m_requetstQueue.begin());
    }
}

void Manager::OnResponseAborted(clLLMEvent& event)
{
    event.Skip();

    clSYSTEM() << "LLM request cancelled." << endl;
    if (m_requetstQueue.empty()) {
        clWARNING() << "Received LLM response ABORT, but no callback is available";
        return;
    }

    size_t flags{ResponseFlags::kAborted | ResponseFlags::kCompleted};
    auto& entry = m_requetstQueue.front();
    if (entry.second != nullptr) {
        (entry.second)(event.GetResponseRaw(), flags);
    }
    m_requetstQueue.clear();
}

void Manager::RestartClient()
{
    clLLMEvent restart_event{wxEVT_LLM_RESTART};
    EventNotifier::Get()->ProcessEvent(restart_event);
}
} // namespace llm
