#include "LLAMCli.hpp"

wxDEFINE_EVENT(wxEVT_LLAMACLI_OUTUPT, clCommandEvent);

LLAMCli::LLAMCli() { ReloadSettings(); }

LLAMCli::~LLAMCli() {}

void LLAMCli::ReloadSettings()
{
    ChatAIConfig config;
    config.Load();

    m_llamaCli = config.GetLlamaCli();
    m_activeModel = config.GetSelectedModel();
}

bool LLAMCli::IsOk() const
{
    return m_activeModel != nullptr && ::wxFileExists(m_llamaCli) && ::wxFileExists(m_activeModel->m_modelFile);
}
