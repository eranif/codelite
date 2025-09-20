#include "LLMManager.hpp"

#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"

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

std::optional<uint64_t> LLMManager::Chat(const wxString& prompt, Callback cb)
{
    if (!IsAvailable()) {
        return std::nullopt;
    }

    auto id = id_generator.fetch_add(1);

    clCommandEvent event_chat{wxEVT_LLM_REQUEST};
    event_chat.SetString(prompt);
    event_chat.SetEventObject(this);
    EventNotifier::Get()->AddPendingEvent(event_chat);
    m_requetstQueue.push_back({id, std::move(cb)});
    return id;
}

void LLMManager::OnTimer(wxTimerEvent& e)
{
    clCommandEvent event_llm_is_available{wxEVT_LLM_IS_AVAILABLE};
    EventNotifier::Get()->ProcessEvent(event_llm_is_available);
    m_isAvailable = event_llm_is_available.IsAnswer();
}

void LLMManager::OnResponse(clCommandEvent& event)
{
    bool completed = event.GetEventType() == wxEVT_LLM_RESPONSE_COMPLETED;
    if (m_requetstQueue.empty()) {
        clWARNING() << "Received LLM response, but no callback is available";
        return;
    }

    (m_requetstQueue.front().second)(event.GetStringRaw(), completed, false);
    if (completed) {
        m_requetstQueue.erase(m_requetstQueue.begin());
    }
}

void LLMManager::OnResponseError(clCommandEvent& event)
{
    if (m_requetstQueue.empty()) {
        clWARNING() << "Received LLM response error, but no callback is available";
        return;
    }

    (m_requetstQueue.front().second)(event.GetStringRaw(), true, true);
    m_requetstQueue.erase(m_requetstQueue.begin());
}
