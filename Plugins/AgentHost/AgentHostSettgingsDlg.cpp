#include "AgentHostSettgingsDlg.hpp"

#include "cl_config.h"

AgentHostSettgingsDlg::AgentHostSettgingsDlg(wxWindow* parent)
    : AgentHostSettgingsBaseDlg(parent)
{
    m_textCtrlClaudeExec->SetValue(clConfig::Get().Read(kAgentHostClaudeCodeExecutable, wxString{}));
    m_textCtrlClaudeCodeReviewArgs->SetValue(clConfig::Get().Read(kAgentHostClaudeCodeReviewArgs, wxString{}));
    m_textCtrlKiroReviewArgs->SetValue(clConfig::Get().Read(kAgentHostKiroCliReviewArgs, wxString{}));
}

AgentHostSettgingsDlg::~AgentHostSettgingsDlg() {}
