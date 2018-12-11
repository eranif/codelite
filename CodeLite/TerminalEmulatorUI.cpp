#include "TerminalEmulatorUI.h"
#if wxUSE_GUI
TerminalEmulatorUI::TerminalEmulatorUI(wxWindow* parent)
    : TerminalEmulatorUIBase(parent)
    , m_terminal(NULL)
{
}

TerminalEmulatorUI::~TerminalEmulatorUI() {}

void TerminalEmulatorUI::OnSendCommand(wxCommandEvent& event) {}

void TerminalEmulatorUI::SetTerminal(TerminalEmulator* terminal)
{
    if(m_terminal) {
        DoUnBindTerminal(m_terminal);
    }
    this->m_terminal = terminal;
    if(m_terminal) {
        DoBindTerminal(m_terminal);
    }
}

void TerminalEmulatorUI::OnProcessExit(clCommandEvent& e)
{
    e.Skip();
    if(m_terminal) {
        DoUnBindTerminal(m_terminal);
        m_terminal = NULL;
    }
}

void TerminalEmulatorUI::OnProcessOutput(clCommandEvent& e)
{
    e.Skip();
    m_stc->SetReadOnly(false);
    m_stc->AppendText(e.GetString());
    m_stc->SetReadOnly(true);
    int lastPos = m_stc->GetLastPosition();
    m_stc->SetCurrentPos(lastPos);
    m_stc->SetSelectionStart(lastPos);
    m_stc->SetSelectionEnd(lastPos);
    m_stc->ScrollToEnd();
}

void TerminalEmulatorUI::DoBindTerminal(TerminalEmulator* terminal)
{
    if(terminal) {
        terminal->Bind(wxEVT_TERMINAL_COMMAND_EXIT, &TerminalEmulatorUI::OnProcessExit, this);
        terminal->Bind(wxEVT_TERMINAL_COMMAND_OUTPUT, &TerminalEmulatorUI::OnProcessOutput, this);
    }
}

void TerminalEmulatorUI::DoUnBindTerminal(TerminalEmulator* terminal)
{
    if(terminal) {
        terminal->Unbind(wxEVT_TERMINAL_COMMAND_EXIT, &TerminalEmulatorUI::OnProcessExit, this);
        terminal->Unbind(wxEVT_TERMINAL_COMMAND_OUTPUT, &TerminalEmulatorUI::OnProcessOutput, this);
    }
}

void TerminalEmulatorUI::Clear()
{
    m_textCtrl->ChangeValue("");
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
}
#endif // LIBCODELITE_WITH_UI
