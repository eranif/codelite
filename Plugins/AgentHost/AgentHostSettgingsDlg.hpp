#pragma once

#include "AgentHostUI.hpp"

inline const wxString kAgentHostCaudeCodeExecutable = "agent-host/claude-code/executable";

class AgentHostSettgingsDlg : public AgentHostSettgingsBaseDlg
{
public:
    AgentHostSettgingsDlg(wxWindow* parent);
    ~AgentHostSettgingsDlg() override;

    wxString GetClaudeCodeExecutable() const
    {
        auto exe = m_textCtrlClaudeExec->GetValue();
        exe.Trim().Trim(false);
        return exe;
    }
};
