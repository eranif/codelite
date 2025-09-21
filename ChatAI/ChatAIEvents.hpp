#pragma once

#include "cl_command_event.h"
#include "ollama/ollama.hpp"

#include <wx/event.h>

enum class LLMLogLevel {
    kTrace,
    kDebug,
    kInfo,
    kWarning,
    kError,
};

enum class LLMEventReason {
    /// The current reason completed successfully.
    kDone,
    /// More data to come.
    kPartialResult,
    /// A non recoverable error.
    kFatalError,
    /// Log messages - NOTICE
    kLogNotice,
    /// Log messages - DEBUG
    kLogDebug,
    /// Request cancelled by the user.
    kCancelled,
};

// --------------------------------------------------------------
// Ollama event
// --------------------------------------------------------------
class LLMEvent : public clCommandEvent
{
public:
    LLMEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    LLMEvent(const LLMEvent&) = default;
    LLMEvent& operator=(const LLMEvent&) = delete;
    ~LLMEvent() override = default;
    wxEvent* Clone() const override { return new LLMEvent(*this); }

    inline LLMEventReason GetReason() const { return m_reason; }
    inline void SetReason(LLMEventReason reason) { m_reason = reason; }

    inline void SetOutput(std::string message) { this->m_message = std::move(message); }
    inline const std::string& GetOutput() const { return m_message; }

    inline void SetModels(const wxArrayString& models) { this->m_models = models; }
    inline const wxArrayString& GetModels() const { return m_models; }

    inline void SetLogLevel(const LLMLogLevel& logLevel) { m_logLevel = logLevel; }
    inline const LLMLogLevel& GetLogLevel() const { return m_logLevel; }

    inline bool IsThinking() const { return m_thinking; }
    inline void SetThinking(bool b) { m_thinking = b; }

    inline void RunCallback()
    {
        if (m_callback) {
            m_callback();
        }
    }

    inline void SetCallback(std::function<void()> cb) { m_callback = std::move(cb); }

private:
    LLMEventReason m_reason{LLMEventReason::kDone};
    std::string m_message;
    wxArrayString m_models;
    LLMLogLevel m_logLevel{LLMLogLevel::kInfo};
    std::function<void()> m_callback{nullptr};
    bool m_thinking{false};
};

using OllamaEventFunction = void (wxEvtHandler::*)(LLMEvent&);
#define OllamaEventHandler(func) wxEVENT_HANDLER_CAST(OllamaEventFunction, func)

wxDECLARE_EVENT(wxEVT_OLLAMA_THINKING, LLMEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_CHAT_STARTED, LLMEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_CHAT_DONE, LLMEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_CHAT_OUTPUT, LLMEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_LIST_MODELS, LLMEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_LOG, LLMEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_RUN_TOOL, LLMEvent);
