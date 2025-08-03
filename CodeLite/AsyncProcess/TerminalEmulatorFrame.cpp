#include "TerminalEmulatorFrame.h"
#if wxUSE_GUI
TerminalEmulatorFrame::TerminalEmulatorFrame(wxWindow* parent)
    : TerminalEmulatorFrameBase(parent)
{
    m_terminal = new TerminalEmulatorUI(this);
    GetSizer()->Add(m_terminal, 1, wxEXPAND|wxALL);
    
}
#endif // LIBCODELITE_WITH_UI
