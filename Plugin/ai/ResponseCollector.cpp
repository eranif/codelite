#include "ai/ResponseCollector.hpp"

#include "assistant/helpers.hpp"

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

    StreamCallbackReason reason{StreamCallbackReason::kNone};
    switch (m_state) {
    case ChatState::kWorking:
        assistant::AddFlagSet(reason, StreamCallbackReason::kToken);
        m_stream_callback(event.GetResponseRaw(), reason);
        break;
    case ChatState::kThinking:
        if (IsWantThinkingTokens()) {
            assistant::AddFlagSet(reason, StreamCallbackReason::kToken);
            assistant::AddFlagSet(reason, StreamCallbackReason::kThinking);
            m_stream_callback(event.GetResponseRaw(), reason);
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
    m_request_cancelled = event.IsRequestCancelled();

    StreamCallbackReason reason{StreamCallbackReason::kDone};
    if (m_ended_with_error) {
        assistant::AddFlagSet(reason, StreamCallbackReason::kError);
    }
    if (m_request_cancelled) {
        assistant::AddFlagSet(reason, StreamCallbackReason::kCancelled);
    }

    if (m_state_callback) {
        m_state_callback(m_state);
    }

    if (m_stream_callback) {
        m_stream_callback(event.GetResponseRaw(), reason);
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
