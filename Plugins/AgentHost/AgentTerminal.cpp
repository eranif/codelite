#include "AgentTerminal.hpp"

#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "terminal_view.h"
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

    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_THEME_CHANGED, &AgentTerminal::OnTerminalThemeChanged, this);
    EventNotifier::Get()->Bind(
        wxEVT_BUILTIN_TERMINAL_SETTINGS_CHANGED, &AgentTerminal::OnTerminalSettingsChanged, this);
}

AgentTerminal::~AgentTerminal()
{
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_THEME_CHANGED, &AgentTerminal::OnTerminalThemeChanged, this);
    EventNotifier::Get()->Unbind(
        wxEVT_BUILTIN_TERMINAL_SETTINGS_CHANGED, &AgentTerminal::OnTerminalSettingsChanged, this);
}

wxTerminalViewCtrl* AgentTerminal::GetTerminal() const
{
    CHECK_PTR_RET_NULL(m_terminal);
    return m_terminal->GetCtrl();
}

void AgentTerminal::OnTerminalThemeChanged(clCommandEvent& event)
{
    event.Skip();
    auto newTheme = clGetManager()->GetTerminalManager()->GetActiveTheme();
    if (GetTerminal())
        GetTerminal()->SetTheme(newTheme);
}

void AgentTerminal::OnTerminalSettingsChanged(clCommandEvent& event)
{
    event.Skip();
    if (GetTerminal()) {
        GetTerminal()->ShowProgressBar(TerminalSettings::Load().m_showProgressBar);
    }
}
