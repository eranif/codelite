#ifndef TERMINALEMULATORUI_H
#define TERMINALEMULATORUI_H

#include "TerminalEmulatorUIBase.h"
#include "TerminalEmulator.h"

class WXDLLIMPEXP_CL TerminalEmulatorUI : public TerminalEmulatorUIBase
{
    TerminalEmulator* m_terminal;
private:
    void DoBindTerminal(TerminalEmulator* terminal);
    void DoUnBindTerminal(TerminalEmulator* terminal);
    
public:
    TerminalEmulatorUI(wxWindow* parent);
    virtual ~TerminalEmulatorUI();
    
    void Clear();
    void SetTerminal(TerminalEmulator* terminal) ;
    TerminalEmulator* GetTerminal() { return m_terminal; }
    wxStyledTextCtrl* GetTerminalOutputWindow() { return m_stc; }
    
protected:
    virtual void OnSendCommand(wxCommandEvent& event);
    void OnProcessExit(clCommandEvent& e);
    void OnProcessOutput(clCommandEvent& e);
};
#endif // TERMINALEMULATORUI_H
