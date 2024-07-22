#pragma once

#include "ChatAIConfig.hpp"
#include "cl_command_event.h"

class LLAMCli : public wxEvtHandler
{
public:
    LLAMCli();
    virtual ~LLAMCli();
    bool IsOk() const;

    ChatAIConfig& GetConfig() { return m_config; }
    void Send(const wxString& prompt);
    void Stop();
    bool IsRunning() const { return m_process != nullptr; }

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);

private:
    ChatAIConfig m_config;
    IProcess* m_process = nullptr;
};

wxDECLARE_EVENT(wxEVT_LLAMACLI_STARTED, clCommandEvent);
wxDECLARE_EVENT(wxEVT_LLAMACLI_STDOUT, clCommandEvent);
wxDECLARE_EVENT(wxEVT_LLAMACLI_STDERR, clCommandEvent);
wxDECLARE_EVENT(wxEVT_LLAMACLI_TERMINATED, clCommandEvent);