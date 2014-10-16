#ifndef TERMINALEMULATORFRAME_H
#define TERMINALEMULATORFRAME_H

#include "TerminalEmulatorUIBase.h"
#include "TerminalEmulatorUI.h"

class WXDLLIMPEXP_CL TerminalEmulatorFrame : public TerminalEmulatorFrameBase
{
    TerminalEmulatorUI* m_terminal;
public:
    TerminalEmulatorFrame(wxWindow* parent);
    virtual ~TerminalEmulatorFrame();
    TerminalEmulatorUI* GetTerminalUI() { return m_terminal; }
};
#endif // TERMINALEMULATORFRAME_H
