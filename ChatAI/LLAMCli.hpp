#pragma once

#include "ChatAIConfig.hpp"
#include "cl_command_event.h"

class LLAMCli
{
public:
    LLAMCli();
    virtual ~LLAMCli();
    void ReloadSettings();
    bool IsOk() const;

private:
    wxString m_llamaCli;
    std::shared_ptr<ChatAIConfig::Model> m_activeModel;
};

wxDECLARE_EVENT(wxEVT_LLAMACLI_OUTUPT, clCommandEvent);