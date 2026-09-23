#pragma once

#include "ClaudeCodeUI.hpp"
#include "cl_command_event.h"
#include "ssh_account_info.h"

class wxTerminalViewCtrl;
class ClaudeCodePage : public ClaudeCodePageBase
{
public:
    ClaudeCodePage(wxBookCtrlBase* parent, const std::optional<SSHAccountInfo>& sshAccount);
    ~ClaudeCodePage() override;

    wxTerminalViewCtrl* GetTerminal() { return m_terminal; }
    void StartClaudeCode(const wxString& claudeExecutable, const wxString& workingDirectory = wxEmptyString);

protected:
    void OnThemeChanged(clCommandEvent& event);
    void OnTerminalLink(clCommandEvent& event);
    void OnTerminalTerminated(clCommandEvent& event);
    void OnTerminalTitleChanged(clCommandEvent& event);

private:
    wxTerminalViewCtrl* m_terminal{nullptr};
};
