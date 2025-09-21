#pragma once

#include "ChatAIConfig.hpp"
#include "ChatAIEvents.hpp"

#include <wx/event.h>
#include <wx/string.h>

/// Define the interface for all LLM clients.
class LLMClientBase : public wxEvtHandler
{
public:
    enum class TaskKind {
        kChat,
        kReloadConfig,
        kListModels,
        kShutdown,
    };

    struct Task {
        TaskKind kind{TaskKind::kChat};
        wxString content; // depending on the task this can be the prompt or the configuration content.
        wxString model;   // the model to use
        wxEvtHandler* owner{nullptr};
    };

    LLMClientBase() = default;
    virtual ~LLMClientBase() = default;

    ChatAIConfig& GetConfig() { return m_config; }
    const ChatAIConfig& GetConfig() const { return m_config; }

    /// API.

    /// Start a chat. The response is returned in the form of an event.
    virtual void Send(wxString prompt, wxString model = wxEmptyString) = 0;

    /// Similar to Send() above, but the events are sent to "owner"
    virtual void Send(wxEvtHandler* owner, wxString prompt, wxString model = wxEmptyString) = 0;
    virtual void Interrupt() = 0;
    virtual bool IsBusy() const = 0;
    virtual void GetModels() const = 0;
    virtual void Clear() = 0;
    virtual void ReloadConfig(const wxString& configContent) = 0;
    virtual void SetLogSink(std::function<void(LLMLogLevel, std::string)> log_sink) = 0;

protected:
    ChatAIConfig m_config;
};
