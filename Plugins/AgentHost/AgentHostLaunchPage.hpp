#pragma once

#include "AgentHostUI.hpp"

class AgentHostLaunchPage : public AgentHostLaunchPageBase
{
public:
    AgentHostLaunchPage(wxWindow* parent);
    ~AgentHostLaunchPage() override = default;

    void SetExecutable(const wxString& exe);
    void SetExecutableArgs(const wxString& args);
    void SetReviewArgs(const wxString& reviewArgs);
    wxString GetExecutable() const;
    wxString GetExecutableArgs() const;
    wxString GetReviewArgs() const;
};
