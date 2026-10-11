#pragma once

#include "AgentHostUI.hpp"

inline const wxString kAgentHostClaudeCodeExecutable = "claude-code/executable";
inline const wxString kAgentHostKiroCliExecutable = "kiro-cli/executable";
// Extra command line arguments for the review buddy's reviewer agent (for example, a custom agent)
inline const wxString kAgentHostClaudeCodeReviewArgs = "claude-code/review-args";
inline const wxString kAgentHostKiroCliReviewArgs = "kiro-cli/review-args";

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

    wxString GetClaudeCodeReviewArgs() const { return Trimmed(m_textCtrlClaudeCodeReviewArgs->GetValue()); }
    wxString GetKiroCliReviewArgs() const { return Trimmed(m_textCtrlKiroReviewArgs->GetValue()); }

private:
    static wxString Trimmed(wxString value)
    {
        value.Trim().Trim(false);
        return value;
    }
};
