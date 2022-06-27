#include "DAPBreakpointsView.h"

#include "UIBreakpoint.hpp"
#include "dap/dap.hpp"

DAPBreakpointsView::DAPBreakpointsView(wxWindow* parent)
    : DAPBreakpointsViewBase(parent)
{
    m_dvListCtrl->SetSortFunction(
        [](const clRowEntry* a, const clRowEntry* b) { return a->GetLabel().CmpNoCase(b->GetLabel()); });
}

DAPBreakpointsView::~DAPBreakpointsView() {}

void DAPBreakpointsView::RefreshView(const SessionBreakpoints& breakpoints)
{
    m_dvListCtrl->Begin();
    m_dvListCtrl->DeleteAllItems();

    for(const auto& bp : breakpoints.get_breakpoints()) {
        wxVector<wxVariant> cols;
        cols.reserve(m_dvListCtrl->GetHeader()->GetCount());
        cols.push_back(wxString() << bp.id);
        cols.push_back(bp.source.path.empty() ? "<unknown>" : bp.source.path);
        cols.push_back(wxString() << bp.line);
        cols.push_back(bp.verified ? wxString("YES") : wxString("NO"));
        m_dvListCtrl->AppendItem(cols);
    }
    m_dvListCtrl->Commit();
}
