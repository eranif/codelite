#include "DAPOutputView.h"

#include "macros.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

DAPOutputView::DAPOutputView(wxWindow* parent)
    : DAPOutputViewBase(parent)
{
    m_ctrl = new clTerminalViewCtrl(this);
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
}

DAPOutputView::~DAPOutputView() {}

void DAPOutputView::AddOutputEvent(dap::OutputEvent* event)
{
    CHECK_PTR_RET(event);
    wxArrayString lines = wxStringTokenize(event->output, "\n", wxTOKEN_STRTOK);
    auto& builder = m_ctrl->GetBuilder(true);
    for(auto& line : lines) {
        line.Trim();
        builder.Clear();
        if(event->category == "console" || event->category == "stdout") {
            builder.Add(line, eAsciiColours::NORMAL_TEXT);
        } else if(event->category == "stderr") {
            builder.Add(line, eAsciiColours::RED);
        } else if(event->category == "telemetry") {
            builder.Add(line, eAsciiColours::GRAY);
        } else if(event->category == "important") {
            builder.Add(line, eAsciiColours::YELLOW, true);
        } else {
            builder.Add(line, eAsciiColours::NORMAL_TEXT);
        }

        m_ctrl->AddLine(line, false);
    }
}
