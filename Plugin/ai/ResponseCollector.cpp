#include "ai/ResponseCollector.hpp"

namespace llm
{
ResponseCollector::ResponseCollector(wxEvtHandler* owner)
    : m_owner(owner)
{
    m_owner->Bind(wxEVT_LLM_OUTPUT, &ResponseCollector::OnOutput, this);
    m_owner->Bind(wxEVT_LLM_OUTPUT_DONE, &ResponseCollector::OnOutputDone, this);
    m_owner->Bind(wxEVT_LLM_THINK_SATRTED, &ResponseCollector::OnThinkStarted, this);
    m_owner->Bind(wxEVT_LLM_THINK_ENDED, &ResponseCollector::OnThinkEnded, this);
    m_owner->Bind(wxEVT_LLM_CHAT_STARTED, &ResponseCollector::OnChatStarted, this);
    m_events_bound = true;
}

ResponseCollector::~ResponseCollector() { UnbindEvents(); }

void ResponseCollector::OnOutput(clLLMEvent& event)
{
    event.Skip();
    if (m_state == ChatState::kWorking) {
        m_response.append(event.GetResponseRaw());
    }
}

void ResponseCollector::OnOutputDone(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kReady;
    m_ended_with_error = event.IsError();
    UnbindEvents();

    if (m_state_callback) {
        m_state_callback(m_state);
    }

    if (m_finalise_callback) {
        m_finalise_callback(this);
    }
}

void ResponseCollector::OnThinkStarted(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kThinking;
    if (m_state_callback) {
        m_state_callback(m_state);
    }
}

void ResponseCollector::OnThinkEnded(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kWorking;
    if (m_state_callback) {
        m_state_callback(m_state);
    }
}

void ResponseCollector::OnChatStarted(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kWorking;
    if (m_state_callback) {
        m_state_callback(m_state);
    }
    m_response.clear();
}

void ResponseCollector::Reset()
{
    m_state = ChatState::kReady;
    m_response.clear();
    m_state_callback = nullptr;
    m_finalise_callback = nullptr;
    m_ended_with_error = false;
}

void ResponseCollector::UnbindEvents()
{
    if (!m_events_bound) {
        return;
    }

    m_events_bound = false;
    m_owner->Unbind(wxEVT_LLM_OUTPUT, &ResponseCollector::OnOutput, this);
    m_owner->Unbind(wxEVT_LLM_OUTPUT_DONE, &ResponseCollector::OnOutputDone, this);
    m_owner->Unbind(wxEVT_LLM_THINK_SATRTED, &ResponseCollector::OnThinkStarted, this);
    m_owner->Unbind(wxEVT_LLM_THINK_ENDED, &ResponseCollector::OnThinkEnded, this);
    m_owner->Unbind(wxEVT_LLM_CHAT_STARTED, &ResponseCollector::OnChatStarted, this);
}
} // namespace llm
