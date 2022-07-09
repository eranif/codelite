#include "DAPBreakpointsView.h"

#include "DebugAdapterClient.hpp"
#include "UIBreakpoint.hpp"
#include "dap/dap.hpp"

DAPBreakpointsView::DAPBreakpointsView(wxWindow* parent, DebugAdapterClient* plugin)
    : DAPBreakpointsViewBase(parent)
    , m_plugin(plugin)
{
    m_dvListCtrl->SetSortFunction(
        [](const clRowEntry* a, const clRowEntry* b) { return a->GetLabel().CmpNoCase(b->GetLabel()); });
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &DAPBreakpointsView::OnBreakpointActivated, this);
}

DAPBreakpointsView::~DAPBreakpointsView()
{
    m_dvListCtrl->Unbind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &DAPBreakpointsView::OnBreakpointActivated, this);
    m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) {
        BreakpointClientData* ptr = reinterpret_cast<BreakpointClientData*>(d);
        delete ptr;
    });
}

void DAPBreakpointsView::RefreshView(const SessionBreakpoints& breakpoints)
{
    m_dvListCtrl->Begin();
    m_dvListCtrl->DeleteAllItems();

    for(const auto& bp : breakpoints.get_breakpoints()) {

        wxString path = bp.source.path;
        if(path.empty()) {
            if(!bp.source.name.empty()) {
                path = bp.source.name;
            } else if(bp.source.sourceReference > 0) {
                path << "source ref: " << bp.source.sourceReference;
            }
        }

        wxVector<wxVariant> cols;
        cols.reserve(m_dvListCtrl->GetHeader()->GetCount());
        cols.push_back(wxString() << bp.id);
        cols.push_back(bp.verified ? wxString("YES") : wxString("NO"));
        cols.push_back(path);
        cols.push_back(wxString() << bp.line);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr) new BreakpointClientData(bp));
    }
    m_dvListCtrl->Commit();
}

BreakpointClientData* DAPBreakpointsView::GetItemData(const wxDataViewItem& item)
{
    BreakpointClientData* ptr = reinterpret_cast<BreakpointClientData*>(m_dvListCtrl->GetItemData(item));
    return ptr;
}

void DAPBreakpointsView::OnBreakpointActivated(wxDataViewEvent& event)
{
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);

    auto cd = GetItemData(item);
    CHECK_PTR_RET(cd);

    m_plugin->LoadFile(cd->m_breapoint.source, cd->m_breapoint.line - 1);
}
