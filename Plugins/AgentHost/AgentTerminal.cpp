#include "AgentTerminal.hpp"

#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#include <wx/sizer.h>

AgentTerminal::AgentTerminal(wxWindow* parent,
                             const std::optional<SSHAccountInfo>& sshAccount,
                             const wxString& shellCommand)
    : wxPanel(parent)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    auto* manager = clGetManager()->GetTerminalManager();
    // The host is the parent of the terminal control: the control adds its progress bar to the sizer of the host,
    // above itself
    m_terminal = manager->NewTerminalHost(this, shellCommand, wxEmptyString, sshAccount);
    if (m_terminal != nullptr) {
        GetSizer()->Add(m_terminal, wxSizerFlags(1).Expand());
    }
    Layout();
}

wxTerminalViewCtrl* AgentTerminal::GetTerminal() const
{
    CHECK_PTR_RET_NULL(m_terminal);
    return m_terminal->GetCtrl();
}
