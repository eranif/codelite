#ifndef DAPCONSOLEOUTPUT_HPP
#define DAPCONSOLEOUTPUT_HPP

#include "DAPOutputView.h"

class DAPConsoleOutput : public DAPOutputView
{
public:
    DAPConsoleOutput(wxWindow* parent);
    virtual ~DAPConsoleOutput();

    void AddOutputEvent(dap::OutputEvent* event);
};

#endif // DAPCONSOLEOUTPUT_HPP
