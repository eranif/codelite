#pragma once

#include "AgentHostUI.hpp"
#include "cl_command_event.h"
#include "ssh_account_info.h"

#include <optional>

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
    void OnFocus(wxFocusEvent& event) override;
    void OnBookPageChanged(wxBookCtrlEvent& event);
    void OnThemeChanged(clCommandEvent& event);
    void OnTerminalLink(clCommandEvent& event);
    void OnTerminalBell(clCommandEvent& event);
    void OnTerminalTerminated(clCommandEvent& event);
    void OnTerminalTitleChanged(clCommandEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    /// Open `text` as a URL, folder, file or symbol (same as clicking a link in the terminal)
    void OpenText(const wxString& text);
    void RestartAgentHost();
    std::optional<wxString> BuildSystemPrompt(const AgentInfo& info);

private:
    wxBookCtrlBase* m_book{nullptr};
    wxTerminalViewCtrl* m_terminal{nullptr};
    AgentInfo m_agentInfo;
};
