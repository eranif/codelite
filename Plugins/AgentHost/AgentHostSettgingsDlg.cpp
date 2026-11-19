#include "AgentHostSettgingsDlg.hpp"

#include "AgentHostLaunchPage.hpp"
#include "cl_config.h"
#include "macros.h"

namespace
{
const wxString kPageClaude = _("claude");
const wxString kPageOpenCode = _("opencode");
const wxString kPageKiroCLI = _("kirocli");
} // namespace

AgentHostSettgingsDlg::AgentHostSettgingsDlg(wxWindow* parent)
    : AgentHostSettgingsBaseDlg(parent)
{
    AddPage(kPageClaude, kAgentHostClaudeCodeExecutable, kAgentHostClaudeCodeArgs, kAgentHostClaudeCodeReviewArgs);
    AddPage(kPageOpenCode, kAgentHostOpenCodeExecutable, kAgentHostOpenCodeArgs, kAgentHostOpenCodeReviewArgs);
    AddPage(kPageKiroCLI, kAgentHostKiroCliExecutable, kAgentHostKiroCliArgs, kAgentHostKiroCliReviewArgs);
    GetSizer()->Fit(this);
}

void AgentHostSettgingsDlg::AddPage(const wxString& name,
                                    const wxString& configExeName,
                                    const wxString& configExeArgs,
                                    const wxString& configReviewArgs)
{
    wxString exe = clConfig::Get().Read(configExeName, wxString{});
    wxString exeArgs = clConfig::Get().Read(configExeArgs, wxString{});
    wxString reviewArgs = clConfig::Get().Read(configReviewArgs, wxString{});

    auto* page = new AgentHostLaunchPage(m_notebook);
    page->SetExecutable(exe);
    page->SetExecutableArgs(exeArgs);
    page->SetReviewArgs(reviewArgs);
    m_notebook->AddPage(page, name, m_notebook->GetPageCount() == 0);
}

void AgentHostSettgingsDlg::SavePage(const wxString& name,
                                     const wxString& configExeName,
                                     const wxString& configExeArgs,
                                     const wxString& configReviewArgs)
{
    auto* page = FindPage(name);
    CHECK_PTR_RET(page);

    clConfig::Get().Write(configExeName, page->GetExecutable());
    clConfig::Get().Write(configExeArgs, page->GetExecutableArgs());
    clConfig::Get().Write(configReviewArgs, page->GetReviewArgs());
}

std::optional<AgentLaunchSpec> AgentHostSettgingsDlg::GetClaudeCodeLaunchSpec() const
{
    return GetLaunchSpecFor(kPageClaude);
}
std::optional<AgentLaunchSpec> AgentHostSettgingsDlg::GetKiroCliLaunchSpec() const
{
    return GetLaunchSpecFor(kPageKiroCLI);
}
std::optional<AgentLaunchSpec> AgentHostSettgingsDlg::GetOpenCodeLaunchSpec() const
{
    return GetLaunchSpecFor(kPageOpenCode);
}

AgentHostLaunchPage* AgentHostSettgingsDlg::FindPage(const wxString& title) const
{
    for (size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
        if (m_notebook->GetPageText(i) == title) {
            return static_cast<AgentHostLaunchPage*>(m_notebook->GetPage(i));
        }
    }
    return nullptr;
}

std::optional<AgentLaunchSpec> AgentHostSettgingsDlg::GetLaunchSpecFor(const wxString& pageName) const
{
    auto* page = FindPage(pageName);
    if (page == nullptr)
        return std::nullopt;

    return AgentLaunchSpec{
        .exe = Trimmed(page->GetExecutable()),
        .args = Trimmed(page->GetExecutableArgs()),
        .reviewArgs = Trimmed(page->GetReviewArgs()),
    };
}

void AgentHostSettgingsDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    SavePage(kPageClaude, kAgentHostClaudeCodeExecutable, kAgentHostClaudeCodeArgs, kAgentHostClaudeCodeReviewArgs);
    SavePage(kPageOpenCode, kAgentHostOpenCodeExecutable, kAgentHostOpenCodeArgs, kAgentHostOpenCodeReviewArgs);
    SavePage(kPageKiroCLI, kAgentHostKiroCliExecutable, kAgentHostKiroCliArgs, kAgentHostKiroCliReviewArgs);
}
