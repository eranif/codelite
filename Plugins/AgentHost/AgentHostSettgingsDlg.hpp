#pragma once

#include "AgentHostUI.hpp"

inline const wxString kAgentHostClaudeCodeExecutable = "claude-code/executable";
inline const wxString kAgentHostKiroCliExecutable = "kiro-cli/executable";
inline const wxString kAgentHostOpenCodeExecutable = "opencode/executable";
// Extra command line arguments for the review buddy's reviewer agent (for example, a custom agent)
inline const wxString kAgentHostClaudeCodeReviewArgs = "claude-code/review-args";
inline const wxString kAgentHostKiroCliReviewArgs = "kiro-cli/review-args";
inline const wxString kAgentHostOpenCodeReviewArgs = "opencode/review-args";

class AgentHostSettgingsDlg : public AgentHostSettgingsBaseDlg
{
public:
    AgentHostSettgingsDlg(wxWindow* parent);
    ~AgentHostSettgingsDlg() override = default;

    wxString GetClaudeCodeExecutable() const { return Trimmed(m_filePickerClaudeCode->GetPath()); }
    wxString GetKiroCliExecutable() const { return Trimmed(m_filePickerKiroCLI->GetPath()); }
    wxString GetOpenCodeExecutable() const { return Trimmed(m_filePickerOpenCode->GetPath()); }
    wxString GetClaudeCodeReviewArgs() const { return Trimmed(m_textCtrlClaudeCodeReviewArgs->GetValue()); }
    wxString GetKiroCliReviewArgs() const { return Trimmed(m_textCtrlKiroReviewArgs->GetValue()); }
    wxString GetOpenCodeReviewArgs() const { return Trimmed(m_textCtrlOpenCodeReviewArgs->GetValue()); }

private:
    static wxString Trimmed(wxString value)
    {
        value.Trim().Trim(false);
        return value;
    }
};
