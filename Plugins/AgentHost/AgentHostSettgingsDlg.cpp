#include "AgentHostSettgingsDlg.hpp"

#include "cl_config.h"

AgentHostSettgingsDlg::AgentHostSettgingsDlg(wxWindow* parent)
    : AgentHostSettgingsBaseDlg(parent)
{
    m_filePickerClaudeCode->SetPath(clConfig::Get().Read(kAgentHostClaudeCodeExecutable, wxString{}));
    m_filePickerKiroCLI->SetPath(clConfig::Get().Read(kAgentHostKiroCliExecutable, wxString{}));
    m_filePickerOpenCode->SetPath(clConfig::Get().Read(kAgentHostOpenCodeExecutable, wxString{}));
    m_textCtrlClaudeCodeReviewArgs->SetValue(clConfig::Get().Read(kAgentHostClaudeCodeReviewArgs, wxString{}));
    m_textCtrlKiroReviewArgs->SetValue(clConfig::Get().Read(kAgentHostKiroCliReviewArgs, wxString{}));
    m_textCtrlOpenCodeReviewArgs->SetValue(clConfig::Get().Read(kAgentHostOpenCodeReviewArgs, wxString{}));

    GetSizer()->Fit(this);
}

