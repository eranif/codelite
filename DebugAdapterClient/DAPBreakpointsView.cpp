#include "DAPBreakpointsView.h"

#include "DebugAdapterClient.hpp"
#include "UIBreakpoint.hpp"
#include "dap/dap.hpp"

#include <unordered_map>

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
    // keep the previous breakpoints
    std::unordered_map<int, dap::Breakpoint> old_breakpoints;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        auto cd = GetItemData(m_dvListCtrl->RowToItem(i));
        if(!cd || cd->m_breapoint.id <= 0) {
            continue;
        }
        old_breakpoints.insert({ cd->m_breapoint.id, cd->m_breapoint });
    }

    m_dvListCtrl->Begin();
    m_dvListCtrl->DeleteAllItems();

    for(auto bp : breakpoints.get_breakpoints()) {

        wxString& path = bp.source.path;
        if(path.empty()) {
            // use the path from the previous time we saw it
            if(old_breakpoints.count(bp.id)) {
                path = old_breakpoints[bp.id].source.path;
            }
        }

        // still empty?
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
        cols.push_back(bp.verified ? wxString(wxT("\u2713")) : wxString(wxT("\u2715")));
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

void DAPBreakpointsView::OnBreakpointsContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("dap-new-function-breakpoint"), _("New function breakppoint"));
    menu.Bind(
        wxEVT_MENU,
        [&](wxCommandEvent& e) {
            wxUnusedVar(e);
            wxString funcname = clGetTextFromUser(_("Set breakpoint in function"), _("Function name"));
            if(funcname.empty()) {
                return;
            }

            dap::FunctionBreakpoint new_bp;
            new_bp.name = funcname;
            auto iter = std::find_if(m_functionBreakpoints.begin(), m_functionBreakpoints.end(),
                                     [&funcname](const dap::FunctionBreakpoint& bp) { return bp.name == funcname; });
            if(iter != m_functionBreakpoints.end()) {
                return;
            }
            m_functionBreakpoints.push_back(new_bp);
            m_plugin->GetClient().SetFunctionBreakpoints(m_functionBreakpoints);
        },
        XRCID("dap-new-function-breakpoint"));
    m_dvListCtrl->PopupMenu(&menu);
}
