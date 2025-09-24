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
        kLoadPluginFunctions,
        kListModels,
        kShutdown,
    };

    enum ChatOptions {
        kDefault = 0,
        kNoTools = (1 << 0),
    };
    struct Task {
        TaskKind kind{TaskKind::kChat};
        wxString content; // depending on the task this can be the prompt or the configuration content.
        wxString model;   // the model to use
        wxEvtHandler* owner{nullptr};
        size_t options{0};
    };

    LLMClientBase() = default;
    virtual ~LLMClientBase() = default;

    ChatAIConfig& GetConfig() { return m_config; }
    const ChatAIConfig& GetConfig() const { return m_config; }

    /// API.

    /// Start a chat. The response is returned in the form of an event.
    virtual void Send(wxString prompt, wxString model = wxEmptyString, ChatOptions options = ChatOptions::kDefault) = 0;

    /// Similar to Send() above, but the events are sent to "owner"
    virtual void Send(wxEvtHandler* owner,
                      wxString prompt,
                      wxString model = wxEmptyString,
                      ChatOptions options = ChatOptions::kDefault) = 0;
    virtual void Interrupt() = 0;
    virtual bool IsBusy() = 0;
    virtual bool IsRunning() = 0;
    virtual void GetModels() = 0;
    virtual void Clear() = 0;
    virtual void ReloadConfig(const wxString& configContent) = 0;
    virtual void SetLogSink(std::function<void(LLMLogLevel, std::string)> log_sink) = 0;
    virtual void OnInitDone() {}

protected:
    ChatAIConfig m_config;
};
