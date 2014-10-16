#include "TerminalEmulatorFrame.h"

TerminalEmulatorFrame::TerminalEmulatorFrame(wxWindow* parent)
    : TerminalEmulatorFrameBase(parent)
{
    m_terminal = new TerminalEmulatorUI(this);
    GetSizer()->Add(m_terminal, 1, wxEXPAND|wxALL);
    
}

TerminalEmulatorFrame::~TerminalEmulatorFrame()
{
}

