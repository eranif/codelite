#pragma once

#include "DAPTerminalCtrlView.h"

class DAPConsoleOutput : public DAPTerminalCtrlView
{
public:
    DAPConsoleOutput(wxWindow* parent);
    ~DAPConsoleOutput() override = default;

    void AddOutputEvent(dap::OutputEvent* event);
};
