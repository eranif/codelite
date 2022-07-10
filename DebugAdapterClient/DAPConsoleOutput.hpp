#ifndef DAPCONSOLEOUTPUT_HPP
#define DAPCONSOLEOUTPUT_HPP

#include "DAPTerminalCtrlView.h"

class DAPConsoleOutput : public DAPTerminalCtrlView
{
public:
    DAPConsoleOutput(wxWindow* parent, clModuleLogger& log);
    virtual ~DAPConsoleOutput();

    void AddOutputEvent(dap::OutputEvent* event);
};

#endif // DAPCONSOLEOUTPUT_HPP
