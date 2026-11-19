#pragma once

#include "AgentHostUI.hpp"

#include <optional>

inline const wxString kAgentHostClaudeCodeExecutable = "claude-code/executable";
inline const wxString kAgentHostClaudeCodeArgs = "claude-code/args";
inline const wxString kAgentHostClaudeCodeReviewArgs = "claude-code/review-args";

inline const wxString kAgentHostKiroCliExecutable = "kiro-cli/executable";
inline const wxString kAgentHostKiroCliArgs = "kiro-cli/args";
inline const wxString kAgentHostKiroCliReviewArgs = "kiro-cli/review-args";

inline const wxString kAgentHostOpenCodeExecutable = "opencode/executable";
inline const wxString kAgentHostOpenCodeArgs = "opencode/args";
inline const wxString kAgentHostOpenCodeReviewArgs = "opencode/review-args";

class AgentHostLaunchPage;
struct AgentLaunchSpec {
    wxString exe;
    wxString args;
    wxString reviewArgs;
};

class AgentHostSettgingsDlg : public AgentHostSettgingsBaseDlg
{
public:
    AgentHostSettgingsDlg(wxWindow* parent);
    ~AgentHostSettgingsDlg() override = default;

    std::optional<AgentLaunchSpec> GetClaudeCodeLaunchSpec() const;
    std::optional<AgentLaunchSpec> GetKiroCliLaunchSpec() const;
    std::optional<AgentLaunchSpec> GetOpenCodeLaunchSpec() const;

private:
    static wxString Trimmed(wxString value)
    {
        value.Trim().Trim(false);
        return value;
    }

    void AddPage(const wxString& name,
                 const wxString& configExeName,
                 const wxString& configExeArgs,
                 const wxString& configReviewArgs);
    void SavePage(const wxString& name,
                  const wxString& configExeName,
                  const wxString& configExeArgs,
                  const wxString& configReviewArgs);
    AgentHostLaunchPage* FindPage(const wxString& title) const;
    std::optional<AgentLaunchSpec> GetLaunchSpecFor(const wxString& pageName) const;

protected:
    void OnOK(wxCommandEvent& event) override;
};
