#pragma once

#include "ClaudeCodeUI.hpp"
#include "ssh_account_info.h"

class wxTerminalViewCtrl;
class ClaudeCodePage : public ClaudeCodePageBase
{
public:
    ClaudeCodePage(wxWindow* parent, const wxString& workingDirectory, const std::optional<SSHAccountInfo>& sshAccount);
    ~ClaudeCodePage() override;

    inline wxTerminalViewCtrl* GetTerminal() { return m_terminal; }

private:
    wxTerminalViewCtrl* m_terminal{nullptr};
};
