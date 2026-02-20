#pragma once

#include "ai/Common.hpp"
#include "ai/LLMEvents.hpp"

#include <functional>
#include <wx/event.h>

namespace llm
{

enum class StreamCallbackReason {
    kNone = 0,
    kDone = (1 << 1),
    kThinking = (2 << 1),
    kCancelled = (3 << 1),
    kError = (4 << 1),
    kToken = (5 << 1),
};

class WXDLLIMPEXP_SDK ResponseCollector : public wxEvtHandler
{
public:
    ResponseCollector();
    ~ResponseCollector() override;

    inline void SetStateChangingCB(std::function<void(ChatState)> cb) { m_state_callback = std::move(cb); }
    void Reset();

    /// The stream callbacl: message, is_done, is_thinking
    inline void SetStreamCallback(std::function<void(const std::string&, StreamCallbackReason)> cb)
    {
        m_stream_callback = std::move(cb);
    }
    inline bool IsEndedWithError() const { return m_ended_with_error; }

    inline void SetWantThinkingTokens(bool wantThinkingTokens) { this->m_wantThinkingTokens = wantThinkingTokens; }
    inline bool IsWantThinkingTokens() const { return m_wantThinkingTokens; }

protected:
    void OnOutput(clLLMEvent& evnet);
    void OnOutputDone(clLLMEvent& evnet);
    void OnThinkStarted(clLLMEvent& evnet);
    void OnThinkEnded(clLLMEvent& evnet);
    void OnChatStarted(clLLMEvent& evnet);
    void UnbindEvents();

private:
    ChatState m_state{ChatState::kReady};
    bool m_ended_with_error{false};
    bool m_request_cancelled{false};
    std::function<void(ChatState)> m_state_callback{nullptr};
    std::function<void(const std::string&, StreamCallbackReason)> m_stream_callback{nullptr};
    bool m_wantThinkingTokens{false};
};

} // namespace llm