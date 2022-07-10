#include "DAPModuleView.h"

DAPModuleView::DAPModuleView(wxWindow* parent, clModuleLogger& log)
    : DAPTerminalCtrlView(parent, log)
{
}

DAPModuleView::~DAPModuleView() {}

void DAPModuleView::AddModuleEvent(dap::ModuleEvent* event)
{
    LOG_DEBUG(LOG) << "Adding module event..." << endl;
    CHECK_PTR_RET(event);
    CHECK_PTR_RET(m_ctrl);

    wxString line;
    line << event->module.id << " " << event->module.name << " " << event->reason.Upper() << " " + event->module.path;
    ScrollToEnd();
    AppendLine(line);
    ScrollToEnd();
}
