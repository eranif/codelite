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

void DAPBreakpointsView::SetBreakpoint(const dap::Breakpoint& breakpoint)
{
    m_breakpoints.erase(breakpoint);
    m_breakpoints.insert(breakpoint);
    RefreshView();
}

void DAPBreakpointsView::SetBreakpoints(const std::vector<dap::Breakpoint>& breakpoint)
{
    for(const auto& bp : breakpoint) {
        if(bp.source.name.empty() && bp.source.path.empty()) {
            continue;
        }
        // remove old entry that matches this breakpoint
        m_breakpoints.erase(bp);
        // insert the new instance
        m_breakpoints.insert(bp);
    }
    RefreshView();
}

void DAPBreakpointsView::RefreshView()
{
    m_dvListCtrl->Begin();
    m_dvListCtrl->DeleteAllItems();

    for(auto& bp : m_breakpoints) {
        wxVector<wxVariant> cols;
        cols.reserve(m_dvListCtrl->GetHeader()->GetCount());
        cols.push_back(wxString() << bp.id);
        cols.push_back(bp.source.path);
        cols.push_back(wxString() << bp.line);
        cols.push_back(bp.verified ? wxString("Yes") : wxString("No"));
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)&bp);
    }
    m_dvListCtrl->Commit();
}

void DAPBreakpointsView::Clear()
{
    m_breakpoints.clear();
    m_dvListCtrl->DeleteAllItems();
}