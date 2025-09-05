#pragma once

#include "ChatAIConfig.hpp"
#include "cl_command_event.h"
#include "ollama/ollama.hpp"

// --------------------------------------------------------------
// Ollama event
// --------------------------------------------------------------
class OllamaEvent : public clCommandEvent
{
public:
    OllamaEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    OllamaEvent(const OllamaEvent&) = default;
    OllamaEvent& operator=(const OllamaEvent&) = delete;
    ~OllamaEvent() override = default;
    wxEvent* Clone() const override { return new OllamaEvent(*this); }

    inline ollama::Reason GetReason() const { return m_reason; }
    inline void SetReason(ollama::Reason reason) { m_reason = reason; }

    inline void SetOutput(std::string message) { this->m_message = std::move(message); }
    inline const std::string& GetOutput() const { return m_message; }

private:
    ollama::Reason m_reason{ ollama::Reason::kDone };
    std::string m_message;
};

using OllamaEventFunction = void (wxEvtHandler::*)(OllamaEvent&);
#define OllamaEventHandler(func) wxEVENT_HANDLER_CAST(OllamaEventFunction, func)

class OllamaClient : public wxEvtHandler
{
public:
    OllamaClient();
    virtual ~OllamaClient();
    bool IsOk() const;

    ChatAIConfig& GetConfig() { return m_config; }
    void Send(const wxString& prompt, const wxString& model = wxEmptyString);
    void Interrupt();
    bool IsRunning() const { return m_ollama.IsRunning(); }
    bool IsBusy() const { return m_processingRequest; }
    wxArrayString GetModels() const;
    void Clear();
    void ReloadConfig(const wxString& configContent);
    void SetLogSink(std::function<void(ollama::LogLevel, std::string)> log_sink);

private:
    ChatAIConfig m_config;
    ollama::Manager& m_ollama;
    bool m_processingRequest{ false };
};

wxDECLARE_EVENT(wxEVT_OLLAMA_THINKING, OllamaEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_CHAT_DONE, OllamaEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_OUTPUT, OllamaEvent);
