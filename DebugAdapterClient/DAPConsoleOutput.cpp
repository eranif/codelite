#include "DAPConsoleOutput.hpp"

#include "globals.h"
#include "macros.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

DAPConsoleOutput::DAPConsoleOutput(wxWindow* parent, clModuleLogger& log)
    : DAPTerminalCtrlView(parent, log)
{
}

DAPConsoleOutput::~DAPConsoleOutput() {}

void DAPConsoleOutput::AddOutputEvent(dap::OutputEvent* event)
{
    CHECK_PTR_RET(event);
    wxArrayString lines = wxStringTokenize(event->output, "\n", wxTOKEN_STRTOK);
    ScrollToEnd();
    for(auto& line : lines) {
        line.Trim();
        AppendLine(line);
    }
    ScrollToEnd();
}
