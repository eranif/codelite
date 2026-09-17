#ifndef DAPCONSOLEOUTPUT_HPP
#define DAPCONSOLEOUTPUT_HPP

#include "DAPTerminalCtrlView.h"

class DAPConsoleOutput : public DAPTerminalCtrlView
{
public:
    DAPConsoleOutput(wxWindow* parent);
    ~DAPConsoleOutput() override = default;

    void AddOutputEvent(dap::OutputEvent* event);
};

#endif // DAPCONSOLEOUTPUT_HPP
