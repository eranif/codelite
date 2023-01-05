#include "DAPBreakpointsView.h"

#include "DebugAdapterClient.hpp"
#include "UIBreakpoint.hpp"
#include "dap/dap.hpp"

#include <algorithm>
#include <unordered_map>
#include <wx/msgdlg.h>

DAPBreakpointsView::DAPBreakpointsView(wxWindow* parent, DebugAdapterClient* plugin, clModuleLogger& log)
    : DAPBreakpointsViewBase(parent)
    , m_plugin(plugin)
    , LOG(log)
{
    m_dvListCtrl->SetSortFunction(
        [](const clRowEntry* a, const clRowEntry* b) { return a->GetLabel().CmpNoCase(b->GetLabel()); });
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &DAPBreakpointsView::OnBreakpointActivated, this);
    clBitmapList* bitmaps = new clBitmapList;
    m_toolbar->AddTool(XRCID("dap-new-source-breakpoint"), _("New source breakpoint"), bitmaps->Add("file_new"));
    m_toolbar->AddTool(XRCID("dap-new-function-breakpoint"), _("New function breakpoint"), bitmaps->Add("json"));
    m_toolbar->AddTool(XRCID("dap-delete-all-breakpoints"), _("Delete All"), bitmaps->Add("clear"));
    m_toolbar->AssignBitmaps(bitmaps);
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL, &DAPBreakpointsView::OnNewFunctionBreakpoint, this,
                    XRCID("dap-new-function-breakpoint"));
    m_toolbar->Bind(wxEVT_TOOL, &DAPBreakpointsView::OnNewSourceBreakpoint, this, XRCID("dap-new-source-breakpoint"));
    m_toolbar->Bind(wxEVT_TOOL, &DAPBreakpointsView::OnDeleteAllBreakpoints, this, XRCID("dap-delete-all-breakpoints"));

    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Enable(m_plugin->GetClient().CanInteract()); },
        XRCID("dap-new-function-breakpoint"));

    m_toolbar->Bind(
        wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Enable(m_plugin->GetClient().CanInteract()); },
        XRCID("dap-new-source-breakpoint"));

    m_toolbar->Bind(
        wxEVT_UPDATE_UI,
        [&](wxUpdateUIEvent& e) { e.Enable(m_plugin->GetClient().CanInteract() && !m_dvListCtrl->IsEmpty()); },
        XRCID("dap-delete-all-breakpoints"));
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

        // the order is:
        // #,?,File,Line,Fullpath
        wxVector<wxVariant> cols;
        cols.reserve(m_dvListCtrl->GetHeader()->GetCount());
        cols.push_back(wxString() << bp.id);
        cols.push_back(bp.verified ? wxString(wxT("\u2713")) : wxString(wxT("\u2715")));
        cols.push_back(wxFileName(path).GetFullName());
        cols.push_back(wxString() << bp.line);
        cols.push_back(path); // the longest field, is shown last
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
    menu.Bind(wxEVT_MENU, &DAPBreakpointsView::OnNewFunctionBreakpoint, this, XRCID("dap-new-function-breakpoint"));
    m_dvListCtrl->PopupMenu(&menu);
}

void DAPBreakpointsView::OnNewFunctionBreakpoint(wxCommandEvent& event)
{
    wxUnusedVar(event);
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
}

void DAPBreakpointsView::OnNewSourceBreakpoint(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString location = clGetTextFromUser(_("Set breakpoint in source file"), _("Location (source:line)"));
    if(location.empty()) {
        return;
    }

    wxString source;
    long line_numner;
    source = location.BeforeFirst(':');
    if(!location.AfterLast(':').ToCLong(&line_numner)) {
        wxMessageBox(_("Invalid line number"), "CodeLite", wxOK | wxICON_ERROR | wxOK_DEFAULT);
        return;
    }

    // get all breakpoints for the requested source file
    std::vector<dap::SourceBreakpoint> source_breakpoints;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        auto cd = GetItemData(m_dvListCtrl->RowToItem(i));
        if(!cd) {
            continue;
        }
        if(cd->m_breapoint.source.path == source) {
            source_breakpoints.push_back({ cd->m_breapoint.line, "" });
        }
    }
    source_breakpoints.push_back({ static_cast<int>(line_numner), "" });
    m_plugin->GetClient().SetBreakpointsFile(source, source_breakpoints);
}

void DAPBreakpointsView::OnDeleteAllBreakpoints(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // collect all source file
    std::unordered_set<wxString> paths;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        auto cd = GetItemData(m_dvListCtrl->RowToItem(i));
        if(!cd) {
            continue;
        }
        LOG_DEBUG(LOG) << "Will delete breakpoint:" << cd->m_breapoint.source.sourceReference << ","
                       << cd->m_breapoint.source.path << endl;
        if(cd->m_breapoint.source.path.empty()) {
            continue;
        }

        paths.insert(cd->m_breapoint.source.path);
    }

    for(const wxString& path : paths) {
        LOG_DEBUG(LOG) << "Deleting breakpoints with path:" << path << endl;
        m_plugin->GetClient().SetBreakpointsFile(path, {});
    }
}
