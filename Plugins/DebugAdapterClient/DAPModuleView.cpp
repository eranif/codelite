#include "DAPModuleView.h"

#include "DapLogger.hpp"

DAPModuleView::DAPModuleView(wxWindow* parent)
    : DAPTerminalCtrlView(parent)
{
}

void DAPModuleView::AddModuleEvent(dap::ModuleEvent* event)
{
    DAP_DEBUG() << "Adding module event..." << endl;
    CHECK_PTR_RET(event);
    CHECK_PTR_RET(m_ctrl);

    wxString line;
    line << event->module.id << " " << event->module.name << " " << event->reason.Upper() << " " + event->module.path;
    ScrollToEnd();
    AppendLine(line);
    ScrollToEnd();
}
