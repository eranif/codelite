#include "DAPModuleView.h"

DAPModuleView::DAPModuleView(wxWindow* parent)
    : DAPOutputView(parent)
{
}

DAPModuleView::~DAPModuleView() {}

void DAPModuleView::AddModuleEvent(dap::ModuleEvent* event)
{
    CHECK_PTR_RET(event);
    auto& builder = m_ctrl->GetBuilder(true);
    wxString line;

    line << event->module.id << " " << event->module.name;
    builder.Add(line, eAsciiColours::NORMAL_TEXT);

    if(event->reason == "new") {
        builder.Add(" NEW", eAsciiColours::GREEN);
    } else if(event->reason == "changed") {
        builder.Add(" CHANGED", eAsciiColours::YELLOW);
    } else if(event->reason == "removed") {
        builder.Add(" REMOVED", eAsciiColours::GRAY);
    } else {
        builder.Add(" " + event->reason.Upper(), eAsciiColours::NORMAL_TEXT);
    }

    builder.Add(" " + event->module.path, eAsciiColours::NORMAL_TEXT);
    m_ctrl->AddLine(builder.GetString(), false);
}
