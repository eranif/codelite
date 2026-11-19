#include "AgentHostLaunchPage.hpp"

AgentHostLaunchPage::AgentHostLaunchPage(wxWindow* parent)
    : AgentHostLaunchPageBase(parent)
{
}

void AgentHostLaunchPage::SetExecutable(const wxString& exe) { m_filePickerExecutable->SetPath(exe); }
void AgentHostLaunchPage::SetExecutableArgs(const wxString& args) { m_textCtrlArguments->SetValue(args); }
void AgentHostLaunchPage::SetReviewArgs(const wxString& reviewArgs)
{
    m_textCtrlReviewBuddyArguments->SetValue(reviewArgs);
}

wxString AgentHostLaunchPage::GetExecutable() const { return m_filePickerExecutable->GetPath(); }
wxString AgentHostLaunchPage::GetExecutableArgs() const
{
    wxString args = m_textCtrlArguments->GetValue();
    args.Trim().Trim(false);
    return args;
}

wxString AgentHostLaunchPage::GetReviewArgs() const
{
    wxString args = m_textCtrlReviewBuddyArguments->GetValue();
    args.Trim().Trim(false);
    return args;
}
