#pragma once

#include "cl_command_event.h"
#include "ssh_account_info.h"

#include <optional>
#include <wx/panel.h>

class TerminalHost;
class wxTerminalViewCtrl;

/// A terminal that runs an agent. The terminal lives in this panel, so its progress bar can be shown above it:
///
///   AgentTerminal (wxPanel)
///     | wxBoxSizer (vertical)
///         | TerminalHost
///
/// Destroy the panel to destroy the terminal (this also ends the process that runs in it).
class AgentTerminal : public wxPanel
{
public:
    /// Open a new terminal that runs `shellCommand`, on the host of `sshAccount` if there is one.
    /// `GetTerminal()` returns nullptr if the terminal could not be opened.
    AgentTerminal(wxWindow* parent, const std::optional<SSHAccountInfo>& sshAccount, const wxString& shellCommand);
    ~AgentTerminal() override;

    wxTerminalViewCtrl* GetTerminal() const;

private:
    void OnTerminalThemeChanged(clCommandEvent& event);

    TerminalHost* m_terminal{nullptr};
};
