#include "DAPMainView.h"

#include "DebugAdapterClient.hpp"
#include "globals.h"
#include "macros.h"

#include <unordered_set>

namespace
{
void DeleteFrameItemData(wxUIntPtr cd)
{
    if (cd == 0) {
        return;
    }
    auto d = reinterpret_cast<FrameInfo*>(cd);
    delete d;
}

void DeleteThreadItemData(wxUIntPtr cd)
{
    if (cd == 0) {
        return;
    }
    auto d = reinterpret_cast<ThreadInfo*>(cd);
    delete d;
}

template <typename ListCtrl>
void DeleteAllItems(ListCtrl* list, std::function<void(wxUIntPtr)> deleter)
{
    for (size_t i = 0; i < list->GetItemCount(); ++i) {
        deleter(list->GetItemData(list->RowToItem((int)i)));
    }
    list->DeleteAllItems();
}
} // namespace

DAPMainView::DAPMainView(wxWindow* parent, DebugAdapterClient* plugin, clModuleLogger& log)
    : DAPMainViewBase(parent)
    , m_plugin(plugin)
    , LOG(log)
{
    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &DAPMainView::OnTimerCheckCanInteract, this);

    m_variablesTree->SetTreeStyle(m_variablesTree->GetTreeStyle() | wxTR_HIDE_ROOT);
    m_variablesTree->SetShowHeader(true);
    m_variablesTree->AddHeader(_("Name"));
    m_variablesTree->AddHeader(_("Value"));
    m_variablesTree->AddHeader(_("Type"));
    m_variablesTree->AddRoot(_("Scopes"));
    m_variablesTree->Bind(wxEVT_TREE_ITEM_EXPANDING, &DAPMainView::OnScopeItemExpanding, this);
    m_variablesTree->Bind(wxEVT_TREE_ITEM_MENU, &DAPMainView::OnVariablesMenu, this);
    m_timer->Start(250);

    wxClientDC dc(this);
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    auto width = dc.GetTextExtent("1234567890 X Thread Name").GetWidth();
    m_splitterThreadsFrames->SetSashPosition(width);

    m_outputPane = new DAPOutputPane(m_splitterPageBottom, LOG);
    m_splitterPageBottom->GetSizer()->Add(m_outputPane, 1, wxEXPAND);
}

DAPMainView::~DAPMainView()
{
    Unbind(wxEVT_TIMER, &DAPMainView::OnTimerCheckCanInteract, this);
    m_timer->Stop();
    wxDELETE(m_timer);
}

void DAPMainView::UpdateThreads(int activeThreadId, dap::ThreadsResponse* response)
{
    // threads are the top level
    if (!response || !response->success) {
        return;
    }

    DeleteAllItems(m_dvListCtrlThreadId, DeleteThreadItemData);
    wxDataViewItem active_item{ nullptr };

    for (const auto& thread : response->threads) {
        wxVector<wxVariant> cols;
        cols.push_back(wxString::Format("%d", thread.id));
        cols.push_back(thread.id == activeThreadId ? wxT("\u2714") : wxString());
        cols.push_back(thread.name);

        // new thread, add it
        m_dvListCtrlThreadId->AppendItem(cols, (wxUIntPtr)(new ThreadInfo(thread)));
        wxDataViewItem item = m_dvListCtrlThreadId->RowToItem(m_dvListCtrlThreadId->GetItemCount() - 1);

        if (thread.id == activeThreadId) {
            active_item = item;
        }
    }

    if (active_item.IsOk()) {
        // this will also trigger a frame request
        m_dvListCtrlThreadId->Select(active_item);
    }
}

void DAPMainView::DoUpdateFrames(int threadId, const std::vector<dap::StackFrame>& frames)
{
    // locate the row
    auto thread_item = FindThread(threadId);
    if (!thread_item.IsOk()) {
        return;
    }

    ThreadInfo* thread_info = reinterpret_cast<ThreadInfo*>(m_dvListCtrlThreadId->GetItemData(thread_item));
    thread_info->frames = frames;

    if (m_dvListCtrlThreadId->GetSelection() == thread_item) {
        // this thread is also the selected one, update the backtrace UI
        DeleteAllItems(m_dvListCtrlFrames, DeleteFrameItemData);
        for (const auto& frame : frames) {
            wxString source;
            if (!frame.source.path.empty()) {
                source = frame.source.path;
            } else {
                source = frame.source.name;
            }
            wxVector<wxVariant> cols;
            cols.push_back(wxString::Format("%d", frame.id));
            cols.push_back(frame.name);
            cols.push_back(wxString::Format("%d", frame.line));
            cols.push_back(source);
            m_dvListCtrlFrames->AppendItem(cols, (wxUIntPtr)(new FrameInfo(frame)));
        }
    }
}

void DAPMainView::UpdateFrames(int threadId, dap::StackTraceResponse* response)
{
    if (!response || !response->success) {
        return;
    }
    DoUpdateFrames(threadId, response->stackFrames);

    if (!m_getFramesRequests.empty()) {
        size_t& count = m_getFramesRequests.front();

        if (count > 0) {
            count--;
        }

        if (count == 0) {
            m_getFramesRequests.erase(m_getFramesRequests.begin());
            DoCopyAllThreadsBacktrace();
        }
    }
}

void DAPMainView::UpdateScopes(int frameId, dap::ScopesResponse* response)
{
    LOG_DEBUG(LOG) << "Current frame id:" << m_scopesFrameId << endl;
    LOG_DEBUG(LOG) << "updating scopes for frame:" << frameId << endl;
    bool changed_frame = m_scopesFrameId != frameId;
    m_scopesFrameId = frameId;

    m_variablesTree->Begin();
    if (changed_frame) {
        m_variablesTree->DeleteChildren(m_variablesTree->GetRootItem());
    }

    // go over the tree and collect the current names
    std::unordered_map<wxString, wxTreeItemId> current_scopes;
    wxTreeItemIdValue cookie;
    wxTreeItemId root = m_variablesTree->GetRootItem();
    auto curitem = m_variablesTree->GetFirstChild(root, cookie);
    while (curitem.IsOk()) {
        current_scopes.insert({ m_variablesTree->GetItemText(curitem), curitem });
        curitem = m_variablesTree->GetNextChild(root, cookie);
    }

    for (const auto& scope : response->scopes) {
        if (current_scopes.count(scope.name) == 0) {
            // new scope
            auto item = m_variablesTree->AppendItem(root, scope.name);
            m_variablesTree->SetItemData(item, new VariableClientData(scope.variablesReference, wxEmptyString));
            if (scope.variablesReference > 0) {
                // it has children
                m_variablesTree->AppendItem(item, "<dummy>");

                wxString scope_name = scope.name.Lower();
                if (scope_name == "locals" || scope_name == "local") {
                    m_variablesTree->Expand(item);
                    m_variablesTree->DeleteChildren(item);
                }
            }
        } else {
            // remove it
            current_scopes.erase(scope.name);
        }
    }

    // remove all items that are still in the map
    for (const auto& vt : current_scopes) {
        m_variablesTree->Delete(vt.second);
    }
    m_variablesTree->Commit();

    // check which top level entry is expanded and update it
    curitem = m_variablesTree->GetFirstChild(root, cookie);
    std::unordered_set<int> items_to_refresh;
    while (curitem.IsOk()) {
        if (m_variablesTree->IsExpanded(curitem)) {
            items_to_refresh.insert(GetVariableId(curitem));
        }
        curitem = m_variablesTree->GetNextChild(root, cookie);
    }

    for (int refId : items_to_refresh) {
        if (refId != wxNOT_FOUND) {
            m_plugin->GetClient().GetChildrenVariables(refId);
        }
    }
}

void DAPMainView::UpdateVariables(int parentRef, dap::VariablesResponse* response)
{
    // go over the tree and collect the current names
    wxTreeItemId parent_item = FindVariableNode(parentRef);
    CHECK_ITEM_RET(parent_item);

    m_variablesTree->Begin();
    m_variablesTree->DeleteChildren(parent_item);

    for (const auto& variable : response->variables) {
        auto child = m_variablesTree->AppendItem(parent_item, variable.name);

        wxString value = variable.value;
        if (value.length() > 200) {
            value = value.Mid(0, 200);
            value << "... [truncated]";
        }

        m_variablesTree->SetItemText(child, value, 1);
        m_variablesTree->SetItemText(child, variable.type, 2);
        m_variablesTree->SetItemData(child, new VariableClientData(variable.variablesReference, variable.value));
        if (variable.variablesReference > 0) {
            // has children
            m_variablesTree->AppendItem(child, "<dummy>");
        }
    }
    m_variablesTree->Commit();
}

void DAPMainView::OnScopeItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId item = event.GetItem();
    CHECK_ITEM_RET(item);

    m_variablesTree->Begin();
    if (m_variablesTree->ItemHasChildren(item)) {
        m_variablesTree->DeleteChildren(item);
        m_variablesTree->AppendItem(item, _("Loading..."));
    }
    m_variablesTree->Commit();
    m_plugin->GetClient().GetChildrenVariables(GetVariableId(event.GetItem()));
}

wxDataViewItem DAPMainView::FindThread(int threadId) const
{
    for (size_t i = 0; i < m_dvListCtrlThreadId->GetItemCount(); ++i) {
        auto item = m_dvListCtrlThreadId->RowToItem(i);
        auto ti = reinterpret_cast<ThreadInfo*>(m_dvListCtrlThreadId->GetItemData(item));
        if (ti->thread_id() == threadId) {
            return item;
        }
    }
    return wxDataViewItem(nullptr);
}

int DAPMainView::GetVariableId(const wxTreeItemId& item)
{
    auto cd = GetVariableClientData(item);
    if (!cd) {
        return wxNOT_FOUND;
    }
    return cd->reference;
}

// O(n)
wxTreeItemId DAPMainView::FindVariableNode(int refId)
{
    wxTreeItemId parent = m_variablesTree->GetRootItem();
    std::vector<wxTreeItemId> Q;
    // scan in dfs manner
    Q.push_back(parent);
    while (!Q.empty()) {
        wxTreeItemId item = Q.back();
        Q.pop_back();

        int curid = GetVariableId(item);
        if (curid == refId) {
            return item;
        }

        if (m_variablesTree->HasChildren(item)) {
            // direct access to this item children goes through clRowEntry (the internal impl)
            clRowEntry* as_entry = reinterpret_cast<clRowEntry*>(item.GetID());
            const auto& children = as_entry->GetChildren();
            Q.reserve(Q.size() + children.size());
            for (auto child : children) {
                Q.emplace_back(wxTreeItemId(child));
            }
        }
    }
    return wxTreeItemId(nullptr);
}

ThreadInfo* DAPMainView::GetThreadInfo(const wxDataViewItem& item)
{
    CHECK_ITEM_RET_NULL(item);
    ThreadInfo* cd = reinterpret_cast<ThreadInfo*>(m_dvListCtrlThreadId->GetItemData(item));
    return cd;
}

VariableClientData* DAPMainView::GetVariableClientData(const wxTreeItemId& item)
{
    CHECK_ITEM_RET_NULL(item);
    VariableClientData* cd = dynamic_cast<VariableClientData*>(m_variablesTree->GetItemData(item));
    return cd;
}

void DAPMainView::SetDisabled(bool b)
{
    m_variablesTree->SetDisabled(b);
    m_variablesTree->Refresh();

    m_dvListCtrlThreadId->Enable(!b);
    m_dvListCtrlFrames->Enable(!b);
}

// we only need to check one view
bool DAPMainView::IsDisabled() const { return m_variablesTree->IsDisabled(); }

void DAPMainView::OnTimerCheckCanInteract(wxTimerEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_plugin->GetClient().IsConnected());

    if (!m_plugin->GetClient().CanInteract()) {
        if (!IsDisabled()) {
            SetDisabled(true);
        }
    } else {
        if (IsDisabled()) {
            SetDisabled(false);
        }
    }
}

void DAPMainView::OnVariablesMenu(wxTreeEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    auto item = event.GetItem();

    auto cd = GetVariableClientData(item);
    CHECK_PTR_RET(cd);

    wxMenu menu;
    menu.Append(XRCID("dap_copy_var_value"), _("Copy"));
    menu.Bind(
        wxEVT_MENU,
        [cd](wxCommandEvent& e) {
            wxUnusedVar(e);
            ::CopyToClipboard(cd->value);
        },
        XRCID("dap_copy_var_value"));
    m_variablesTree->PopupMenu(&menu);
}

void DAPMainView::OnThreadsListMenu(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    auto item = event.GetItem();

    auto thread_info = GetThreadInfo(item);
    CHECK_PTR_RET(thread_info);

    wxMenu menu;
    menu.Append(XRCID("copy_all_threads_backtrace"), _("Copy all"));
    menu.Append(XRCID("copy_current_thread_backtrace"), _("Copy this thread backtrace"));
    menu.Bind(
        wxEVT_MENU,
        [this, thread_info](wxCommandEvent& e) {
            wxUnusedVar(e);
            wxString bt;
            ::CopyToClipboard(thread_info->GetBacktrace());
        },
        XRCID("copy_current_thread_backtrace"));

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            // Make sure that all the threads have their backtrace loaded
            std::vector<int> threads_need_frames;
            for (size_t i = 0; i < m_dvListCtrlThreadId->GetItemCount(); ++i) {
                auto item = m_dvListCtrlThreadId->RowToItem(i);
                auto thread_info = GetThreadInfo(item);
                if (!thread_info->has_frames()) {
                    threads_need_frames.push_back(thread_info->thread_id());
                }
            }

            if (threads_need_frames.empty()) {
                DoCopyAllThreadsBacktrace();
            } else {
                // request frames for each missing thread and then perform the copy
                m_getFramesRequests.push_back(threads_need_frames.size());
                for (int thread_id : threads_need_frames) {
                    m_plugin->GetClient().GetFrames(thread_id);
                }
            }
        },
        XRCID("copy_all_threads_backtrace"));
    m_dvListCtrlThreadId->PopupMenu(&menu);
}

void DAPMainView::Clear()
{
    m_variablesTree->DeleteAllItems();
    DeleteAllItems(m_dvListCtrlThreadId, DeleteThreadItemData);
    DeleteAllItems(m_dvListCtrlFrames, DeleteFrameItemData);
}

FrameInfo* DAPMainView::GetFrameInfo(const wxDataViewItem& item)
{
    CHECK_ITEM_RET_NULL(item);
    auto cd = reinterpret_cast<FrameInfo*>(m_dvListCtrlFrames->GetItemData(item));
    return cd;
}

void DAPMainView::OnFrameChanged(wxDataViewEvent& event)
{
    auto cd = GetFrameInfo(event.GetItem());
    if (cd == nullptr) {
        return;
    }
    m_plugin->GetClient().GetScopes(cd->frame.id);

    // open the file corresponded to the frame ID
    m_plugin->LoadFile(cd->frame.source, cd->frame.line - 1);
}

void DAPMainView::OnThreadIdChanged(wxDataViewEvent& event)
{
    event.Skip();
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);

    auto thread_info = GetThreadInfo(item);
    CHECK_PTR_RET(thread_info);
    m_plugin->GetClient().GetFrames(thread_info->thread_id());
}

void DAPMainView::DoCopyAllThreadsBacktrace()
{
    wxString backtraces;
    for (size_t i = 0; i < m_dvListCtrlThreadId->GetItemCount(); ++i) {
        auto item = m_dvListCtrlThreadId->RowToItem(i);
        auto thread_info = GetThreadInfo(item);
        backtraces << thread_info->GetBacktrace();
    }

    backtraces.Trim() << "\n";
    ::CopyToClipboard(backtraces);
    clGetManager()->SetStatusMessage(_("Backtrace copied to clipboard!"));
}
