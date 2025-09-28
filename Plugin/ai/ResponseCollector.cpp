#include "ai/ResponseCollector.hpp"

namespace llm
{
ResponseCollector::ResponseCollector()
{
    Bind(wxEVT_LLM_OUTPUT, &ResponseCollector::OnOutput, this);
    Bind(wxEVT_LLM_OUTPUT_DONE, &ResponseCollector::OnOutputDone, this);
    Bind(wxEVT_LLM_THINK_SATRTED, &ResponseCollector::OnThinkStarted, this);
    Bind(wxEVT_LLM_THINK_ENDED, &ResponseCollector::OnThinkEnded, this);
    Bind(wxEVT_LLM_CHAT_STARTED, &ResponseCollector::OnChatStarted, this);
}

ResponseCollector::~ResponseCollector()
{
    Unbind(wxEVT_LLM_OUTPUT, &ResponseCollector::OnOutput, this);
    Unbind(wxEVT_LLM_OUTPUT_DONE, &ResponseCollector::OnOutputDone, this);
    Unbind(wxEVT_LLM_THINK_SATRTED, &ResponseCollector::OnThinkStarted, this);
    Unbind(wxEVT_LLM_THINK_ENDED, &ResponseCollector::OnThinkEnded, this);
    Unbind(wxEVT_LLM_CHAT_STARTED, &ResponseCollector::OnChatStarted, this);
}

void ResponseCollector::OnOutput(clLLMEvent& event)
{
    event.Skip();
    if (!m_stream_callback) {
        return;
    }
    switch (m_state) {
    case ChatState::kWorking:
        m_stream_callback(event.GetResponseRaw(), false, false);
        break;
    case ChatState::kThinking:
        if (IsWantThinkingTokens()) {
            m_stream_callback(event.GetResponseRaw(), false, true);
        }
        break;
    case ChatState::kReady:
        break;
    }
}

void ResponseCollector::OnOutputDone(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kReady;
    m_ended_with_error = event.IsError();

    if (m_state_callback) {
        m_state_callback(m_state);
    }

    if (m_stream_callback) {
        m_stream_callback(event.GetResponseRaw(), true, false);
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
}

void ResponseCollector::Reset()
{
    m_state = ChatState::kReady;
    m_state_callback = nullptr;
    m_stream_callback = nullptr;
    m_ended_with_error = false;
}

} // namespace llm
