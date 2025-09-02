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

    inline void SetMessage(std::string message) { this->m_message = std::move(message); }
    inline const std::string& GetMessage() const { return m_message; }

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
    void Send(const wxString& prompt);
    void Interrupt();
    bool IsRunning() const { return m_ollama.IsRunning(); }
    bool IsProcessingRequest() const { return m_processingRequest; }

private:
    ChatAIConfig m_config;
    ollama::Manager& m_ollama;
    wxString m_model;
    bool m_processingRequest{ false };
};

wxDECLARE_EVENT(wxEVT_OLLAMA_THINKING, clCommandEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_READY, clCommandEvent);
wxDECLARE_EVENT(wxEVT_OLLAMA_OUTPUT, clCommandEvent);
