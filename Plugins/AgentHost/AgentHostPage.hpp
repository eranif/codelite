#pragma once

#include "AgentHostUI.hpp"
#include "cl_command_event.h"
#include "ssh_account_info.h"

class wxTerminalViewCtrl;
class AgentHostPage : public AgentHostPageBase
{
public:
    AgentHostPage(wxBookCtrlBase* parent, const std::optional<SSHAccountInfo>& sshAccount);
    ~AgentHostPage() override;

    wxTerminalViewCtrl* GetTerminal() { return m_terminal; }
    void StartAgentHost(const wxString& claudeExecutable, const wxString& workingDirectory = wxEmptyString);

protected:
    void OnThemeChanged(clCommandEvent& event);
    void OnTerminalLink(clCommandEvent& event);
    void OnTerminalBell(clCommandEvent& event);
    void OnTerminalTerminated(clCommandEvent& event);
    void OnTerminalTitleChanged(clCommandEvent& event);

private:
    wxTerminalViewCtrl* m_terminal{nullptr};
};
