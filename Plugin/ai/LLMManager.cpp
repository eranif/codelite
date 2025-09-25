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
{
    m_timer.SetOwner(this);
    m_timer.Start(500);
    Bind(wxEVT_TIMER, &Manager::OnTimer, this, m_timer.GetId());
    Bind(wxEVT_LLM_RESPONSE, &Manager::OnResponse, this);
    Bind(wxEVT_LLM_RESPONSE_COMPLETED, &Manager::OnResponse, this);
    Bind(wxEVT_LLM_RESPONSE_ERROR, &Manager::OnResponseError, this);
}

Manager::~Manager()
{
    m_timer.Stop();
    Unbind(wxEVT_TIMER, &Manager::OnTimer, this, m_timer.GetId());
    Unbind(wxEVT_LLM_RESPONSE, &Manager::OnResponse, this);
    Unbind(wxEVT_LLM_RESPONSE_COMPLETED, &Manager::OnResponse, this);
    Unbind(wxEVT_LLM_RESPONSE_ERROR, &Manager::OnResponseError, this);
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
                   ResponseCB response_cb,
                   size_t options,
                   const wxString& model)
{
    std::shared_ptr<ResponseCB> shared_cb = std::make_shared<ResponseCB>(response_cb);
    std::shared_ptr<SharedState> shared_state = std::make_shared<SharedState>();
    shared_state->total_batch_count = prompt_context_arr.size();

    for (const auto& prompt_context : prompt_context_arr) {
        wxString prompt = prompt_template;
        prompt.Replace("{{CONTEXT}}", prompt_context);
        Chat(
            prompt,
            [=](const std::string& message, size_t flags) {
                if (flags & llm::Manager::kCompleted) {
                    if (shared_state->total_batch_count == shared_state->current_batch) {
                        // this was the last token in the batch.
                        response_cb(message, llm::Manager::kCompleted);
                    } else {
                        shared_state->current_batch++;
                    }
                } else {
                    response_cb(message, flags);
                }
            },
            options,
            model);
    }
}

std::optional<uint64_t> Manager::Chat(const wxString& prompt, ResponseCB cb, size_t options, const wxString& model)
{
    if (!IsAvailable()) {
        return std::nullopt;
    }

    auto id = id_generator.fetch_add(1);

    clLLMEvent event_chat{wxEVT_LLM_REQUEST};
    event_chat.SetPrompt(prompt);
    event_chat.SetEventObject(this);
    bool enable_tools = ((options & ChatOptions::kNoTools) == 0);
    event_chat.SetEnableTools(enable_tools);
    event_chat.SetModelName(model);
    event_chat.SetClearHistory((options & ChatOptions::kClearHistory) != 0);
    EventNotifier::Get()->AddPendingEvent(event_chat);
    m_requetstQueue.push_back({id, std::move(cb)});
    return id;
}

void Manager::OnTimer(wxTimerEvent& e)
{
    clLLMEvent event_llm_is_available{wxEVT_LLM_IS_AVAILABLE};
    EventNotifier::Get()->ProcessEvent(event_llm_is_available);
    m_isAvailable = event_llm_is_available.IsAvailable();
}

void Manager::OnResponse(clLLMEvent& event)
{
    bool completed = event.GetEventType() == wxEVT_LLM_RESPONSE_COMPLETED;
    bool is_thinking = event.IsThinking();
    if (m_requetstQueue.empty()) {
        clWARNING() << "Received LLM response, but no callback is available";
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

void Manager::OnResponseError(clLLMEvent& event)
{
    if (m_requetstQueue.empty()) {
        clWARNING() << "Received LLM response error, but no callback is available";
        return;
    }

    size_t flags{ResponseFlags::kError | ResponseFlags::kCompleted};

    auto& entry = m_requetstQueue.front();
    if (entry.second != nullptr) {
        (entry.second)(event.GetResponseRaw(), flags);
    }
    m_requetstQueue.erase(m_requetstQueue.begin());
}

void Manager::Cancel(uint64_t request_id)
{
    auto iter = std::find_if(
        m_requetstQueue.begin(), m_requetstQueue.end(), [request_id](const auto& p) { return p.first == request_id; });
    if (iter == m_requetstQueue.end()) {
        return;
    }

    // We can't really remove the entry, but we can replace the callback with a null.
    iter->second = nullptr;
}

bool Manager::RegisterFunction(Function func)
{
    std::string name = func.m_name;
    return m_functions.insert({name, std::move(func)}).second;
}

void Manager::UnregisterFunction(const std::string& funcname)
{
    if (m_functions.count(funcname) != 0) {
        // Erase it from the local cache.
        m_functions.erase(funcname);
    }

    // In case someone already consumed it from the LLMManager, notify that this function should be unregistered.
    clLLMEvent unregister_event{wxEVT_LLM_UNREGISTER_FUNC};
    unregister_event.SetStringRaw(funcname);
    unregister_event.SetString(funcname);
    EventNotifier::Get()->ProcessEvent(unregister_event);
}

void Manager::ConsumeFunctions(std::function<void(Function)> cb)
{
    for (auto& [_, f] : m_functions) {
        cb(std::move(f));
    }
    m_functions.clear();
}
} // namespace llm