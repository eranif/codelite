#pragma once

#include "ai/Common.hpp"
#include "ai/LLMEvents.hpp"

#include <functional>
#include <wx/event.h>

namespace llm
{
class WXDLLIMPEXP_SDK ResponseCollector : public wxEvtHandler
{
public:
    ResponseCollector(wxEvtHandler* owner);
    ~ResponseCollector() override;

    inline const std::string& GetResponse() const { return m_response; }
    inline void SetStateChangingCB(std::function<void(ChatState)> cb) { m_state_callback = std::move(cb); }
    void Reset();
    inline void SetFinaliseCallback(std::function<void(ResponseCollector*)> cb) { m_finalise_callback = std::move(cb); }

    inline bool IsEndedWithError() const { return m_ended_with_error; }

protected:
    void OnOutput(clLLMEvent& evnet);
    void OnOutputDone(clLLMEvent& evnet);
    void OnThinkStarted(clLLMEvent& evnet);
    void OnThinkEnded(clLLMEvent& evnet);
    void OnChatStarted(clLLMEvent& evnet);
    void UnbindEvents();

private:
    void InvokeFinaliseCallback()
    {
        if (m_finalise_callback == nullptr) {
            return;
        }
        m_finalise_callback(this);
    }

    wxEvtHandler* m_owner{nullptr};
    std::string m_response;
    ChatState m_state{ChatState::kReady};
    bool m_ended_with_error{false};
    bool m_events_bound{false};
    std::function<void(ChatState)> m_state_callback{nullptr};
    std::function<void(ResponseCollector*)> m_finalise_callback{nullptr};
};

} // namespace llm