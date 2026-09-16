#pragma once

#include "ClaudeCodeUI.hpp"
#include "cl_command_event.h"
#include "ssh_account_info.h"

class wxTerminalViewCtrl;
class ClaudeCodePage : public ClaudeCodePageBase
{
public:
    ClaudeCodePage(wxWindow* parent, const wxString& workingDirectory, const std::optional<SSHAccountInfo>& sshAccount);
    ~ClaudeCodePage() override;

    inline wxTerminalViewCtrl* GetTerminal() { return m_terminal; }

private:
    void OnThemeChanged(clCommandEvent& event);
    wxTerminalViewCtrl* m_terminal{nullptr};
};
