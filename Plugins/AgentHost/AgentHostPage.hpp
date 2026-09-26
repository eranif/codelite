#pragma once

#include "AgentHostUI.hpp"
#include "cl_command_event.h"
#include "ssh_account_info.h"

class wxTerminalViewCtrl;

enum class AgentType {
    kClaudeCode = 0,
    kKiroCli = 1,
};

struct AgentInfo {
    AgentType agent_type;
    wxString executable;
    wxString workingDirectory;
    std::optional<SSHAccountInfo> sshAccount;
};

class AgentHostPage : public AgentHostPageBase
{
public:
    AgentHostPage(wxBookCtrlBase* parent);
    ~AgentHostPage() override;

    wxTerminalViewCtrl* GetTerminal() { return m_terminal; }
    void StartAgentHost(const AgentInfo& info);

protected:
    void OnThemeChanged(clCommandEvent& event);
    void OnTerminalLink(clCommandEvent& event);
    void OnTerminalBell(clCommandEvent& event);
    void OnTerminalTerminated(clCommandEvent& event);
    void OnTerminalTitleChanged(clCommandEvent& event);

private:
    wxTerminalViewCtrl* m_terminal{nullptr};
};
