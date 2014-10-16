#ifndef PHPTERMINAL_H
#define PHPTERMINAL_H

#include "TerminalEmulatorFrame.h" // Base class: TerminalEmulatorFrame

class PHPTerminal : public TerminalEmulatorFrame
{
public:
    PHPTerminal(wxWindow* parent);
    virtual ~PHPTerminal();

};

#endif // PHPTERMINAL_H
