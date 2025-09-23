#include "LLMManager.hpp"

#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"

#include <atomic> // atomic_uint64_t

namespace
{
std::atomic_uint64_t id_generator{0};
}

LLMManager& LLMManager::GetInstance()
{
    static LLMManager instance;
    return instance;
}

LLMManager::LLMManager()
{
    m_timer.SetOwner(this);
    m_timer.Start(500);
    Bind(wxEVT_TIMER, &LLMManager::OnTimer, this, m_timer.GetId());
    Bind(wxEVT_LLM_RESPONSE, &LLMManager::OnResponse, this);
    Bind(wxEVT_LLM_RESPONSE_COMPLETED, &LLMManager::OnResponse, this);
    Bind(wxEVT_LLM_RESPONSE_ERROR, &LLMManager::OnResponseError, this);
}

LLMManager::~LLMManager()
{
    m_timer.Stop();
    Unbind(wxEVT_TIMER, &LLMManager::OnTimer, this, m_timer.GetId());
    Unbind(wxEVT_LLM_RESPONSE, &LLMManager::OnResponse, this);
    Unbind(wxEVT_LLM_RESPONSE_COMPLETED, &LLMManager::OnResponse, this);
    Unbind(wxEVT_LLM_RESPONSE_ERROR, &LLMManager::OnResponseError, this);
}

std::optional<uint64_t> LLMManager::Chat(const wxString& prompt, Callback cb, size_t options, const wxString& model)
{
    if (!IsAvailable()) {
        return std::nullopt;
    }

    auto id = id_generator.fetch_add(1);

    clLLMEvent event_chat{wxEVT_LLM_REQUEST};
    event_chat.SetPrompt(prompt);
    event_chat.SetEventObject(this);
    event_chat.SetEnableTools((options & ChatOptions::kNoTools) != 0);
    event_chat.SetModelName(model);
    EventNotifier::Get()->AddPendingEvent(event_chat);
    m_requetstQueue.push_back({id, std::move(cb)});
    return id;
}

void LLMManager::OnTimer(wxTimerEvent& e)
{
    clLLMEvent event_llm_is_available{wxEVT_LLM_IS_AVAILABLE};
    EventNotifier::Get()->ProcessEvent(event_llm_is_available);
    m_isAvailable = event_llm_is_available.IsAvailable();
}

void LLMManager::OnResponse(clLLMEvent& event)
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
    (m_requetstQueue.front().second)(event.GetResponseRaw(), flags);
    if (completed) {
        m_requetstQueue.erase(m_requetstQueue.begin());
    }
}

void LLMManager::OnResponseError(clLLMEvent& event)
{
    if (m_requetstQueue.empty()) {
        clWARNING() << "Received LLM response error, but no callback is available";
        return;
    }

    size_t flags{ResponseFlags::kError | ResponseFlags::kCompleted};
    (m_requetstQueue.front().second)(event.GetResponseRaw(), flags);
    m_requetstQueue.erase(m_requetstQueue.begin());
}
