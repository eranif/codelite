#include "AgentHostSettgingsDlg.hpp"

#include "cl_config.h"

AgentHostSettgingsDlg::AgentHostSettgingsDlg(wxWindow* parent)
    : AgentHostSettgingsBaseDlg(parent)
{
    m_textCtrlClaudeExec->SetValue(clConfig::Get().Read(kAgentHostCaudeCodeExecutable, wxString{}));
}

AgentHostSettgingsDlg::~AgentHostSettgingsDlg() {}
