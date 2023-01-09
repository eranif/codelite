#include "DAPMainView.h"

#include "DebugAdapterClient.hpp"
#include "globals.h"
#include "macros.h"

DAPMainView::DAPMainView(wxWindow* parent, DebugAdapterClient* plugin, clModuleLogger& log)
    : DAPMainViewBase(parent)
    , m_plugin(plugin)
    , LOG(log)
{
    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &DAPMainView::OnTimerCheckCanInteract, this);

    m_threadsTree->SetTreeStyle(m_threadsTree->GetTreeStyle() | wxTR_HIDE_ROOT);
    m_threadsTree->SetShowHeader(true);
    m_threadsTree->AddHeader(_("ID"));     // The thread/frame ID
    m_threadsTree->AddHeader(_("Name"));   // The frame source (function or file)
    m_threadsTree->AddHeader(_("Line"));   // The frame line
    m_threadsTree->AddHeader(_("Source")); // The thread/frame name
    m_threadsTree->AddRoot(_("Threads"));
    m_threadsTree->Bind(wxEVT_TREE_ITEM_EXPANDING, &DAPMainView::OnThreadItemExpanding, this);
    m_threadsTree->Bind(wxEVT_TREE_SEL_CHANGED, &DAPMainView::OnFrameItemSelected, this);
    m_threadsTree->Bind(wxEVT_TREE_ITEM_MENU, &DAPMainView::OnThreadsListMenu, this);

    m_variablesTree->SetTreeStyle(m_variablesTree->GetTreeStyle() | wxTR_HIDE_ROOT);
    m_variablesTree->SetShowHeader(true);
    m_variablesTree->AddHeader(_("Name"));
    m_variablesTree->AddHeader(_("Value"));
    m_variablesTree->AddHeader(_("Type"));
    m_variablesTree->AddRoot(_("Scopes"));
    m_variablesTree->Bind(wxEVT_TREE_ITEM_EXPANDING, &DAPMainView::OnScopeItemExpanding, this);
    m_variablesTree->Bind(wxEVT_TREE_ITEM_MENU, &DAPMainView::OnVariablesMenu, this);
    m_timer->Start(250);
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
    if(!response || !response->success) {
        return;
    }

    // build a map that matches thread-id -> wxTreeItemId
    wxTreeItemId root = m_threadsTree->GetRootItem();

    m_threadsTree->Begin();
    m_threadsTree->DeleteChildren(root);

    for(const auto& thread : response->threads) {
        // new thread, add it
        wxTreeItemId item =
            m_threadsTree->AppendItem(root, wxString() << thread.id, -1, -1, new FrameOrThreadClientData(thread));

        // add a dummy child, so will get the ">" button
        m_threadsTree->AppendItem(item, "<dummy>");
        if(thread.id == activeThreadId) {
            m_threadsTree->SetItemBold(item, true, 0);
            m_threadsTree->SetItemBold(item, true, 1);
        }
        m_threadsTree->SetItemText(item, thread.name, 1);
    }
    m_threadsTree->Commit();
}

void DAPMainView::UpdateFrames(int threadId, dap::StackTraceResponse* response)
{
    // locate the row
    m_threadsTree->Begin();
    wxTreeItemId parent = FindThreadNode(threadId);
    if(!parent.IsOk()) {
        return;
    }

    if(m_threadsTree->ItemHasChildren(parent)) {
        m_threadsTree->DeleteChildren(parent);
    }

    // mark the parent as loaded
    auto cd = GetFrameClientData(parent);
    if(cd) {
        cd->loaded = true;
    }

    // append the stack frame
    for(const auto& frame : response->stackFrames) {
        wxTreeItemId frame_item =
            m_threadsTree->AppendItem(parent, wxString() << frame.id, -1, -1, new FrameOrThreadClientData(frame));

        wxString source;
        if(!frame.source.path.empty()) {
            source = frame.source.path;
        } else {
            source = frame.source.name;
        }

        m_threadsTree->SetItemText(frame_item, source, 1);
        m_threadsTree->SetItemText(frame_item, wxString() << frame.line, 2);
        m_threadsTree->SetItemText(frame_item, frame.name, 3);
    }
    m_threadsTree->Commit();
    m_threadsTree->Expand(parent);
}

void DAPMainView::UpdateScopes(int frameId, dap::ScopesResponse* response)
{
    LOG_DEBUG(LOG) << "Current frame id:" << m_scopesFrameId << endl;
    LOG_DEBUG(LOG) << "updating scopes for frame:" << frameId << endl;
    bool changed_frame = m_scopesFrameId != frameId;
    m_scopesFrameId = frameId;

    m_variablesTree->Begin();
    if(changed_frame) {
        m_variablesTree->DeleteChildren(m_variablesTree->GetRootItem());
    }

    // go over the tree and collect the current names
    std::unordered_map<wxString, wxTreeItemId> current_scopes;
    wxTreeItemIdValue cookie;
    wxTreeItemId root = m_variablesTree->GetRootItem();
    auto curitem = m_variablesTree->GetFirstChild(root, cookie);
    while(curitem.IsOk()) {
        current_scopes.insert({ m_variablesTree->GetItemText(curitem), curitem });
        curitem = m_variablesTree->GetNextChild(root, cookie);
    }

    for(const auto& scope : response->scopes) {
        if(current_scopes.count(scope.name) == 0) {
            // new scope
            auto item = m_variablesTree->AppendItem(root, scope.name);
            m_variablesTree->SetItemData(item, new VariableClientData(scope.variablesReference, wxEmptyString));
            if(scope.variablesReference > 0) {
                // it has children
                m_variablesTree->AppendItem(item, "<dummy>");

                wxString scope_name = scope.name.Lower();
                if(scope_name == "locals" || scope_name == "local") {
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
    for(const auto& vt : current_scopes) {
        m_variablesTree->Delete(vt.second);
    }
    m_variablesTree->Commit();

    // check which top level entry is expanded and update it
    curitem = m_variablesTree->GetFirstChild(root, cookie);
    std::unordered_set<int> items_to_refresh;
    while(curitem.IsOk()) {
        if(m_variablesTree->IsExpanded(curitem)) {
            items_to_refresh.insert(GetVariableId(curitem));
        }
        curitem = m_variablesTree->GetNextChild(root, cookie);
    }

    for(int refId : items_to_refresh) {
        if(refId != wxNOT_FOUND) {
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

    for(const auto& variable : response->variables) {
        auto child = m_variablesTree->AppendItem(parent_item, variable.name);

        wxString value = variable.value;
        if(value.length() > 200) {
            value = value.Mid(0, 200);
            value << "... [truncated]";
        }

        m_variablesTree->SetItemText(child, value, 1);
        m_variablesTree->SetItemText(child, variable.type, 2);
        m_variablesTree->SetItemData(child, new VariableClientData(variable.variablesReference, variable.value));
        if(variable.variablesReference > 0) {
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

    wxTreeItemIdValue cookie;
    m_variablesTree->Begin();
    if(m_variablesTree->ItemHasChildren(item)) {
        m_variablesTree->DeleteChildren(item);
        m_variablesTree->AppendItem(item, _("Loading..."));
    }
    m_variablesTree->Commit();
    m_plugin->GetClient().GetChildrenVariables(GetVariableId(event.GetItem()));
}

wxTreeItemId DAPMainView::FindThreadNode(int threadId)
{
    wxTreeItemId root = m_threadsTree->GetRootItem();
    wxTreeItemIdValue cookie;
    auto curitem = m_threadsTree->GetFirstChild(root, cookie);
    while(curitem.IsOk()) {
        int cur_thread_id = ::wxStringToInt(m_threadsTree->GetItemText(curitem, 0), -1);
        if(cur_thread_id == threadId) {
            return curitem;
        }
        curitem = m_threadsTree->GetNextChild(root, cookie);
    }
    return wxTreeItemId(nullptr);
}

void DAPMainView::OnFrameItemSelected(wxTreeEvent& event)
{
    auto cd = GetFrameClientData(event.GetItem());
    if(!cd || !cd->IsFrame() || cd->GetId() == wxNOT_FOUND) {
        return;
    }
    m_plugin->GetClient().GetScopes(cd->GetId());

    // open the file correspondend to the frame ID
    m_plugin->LoadFile(cd->frame_info.source, cd->frame_info.line - 1);
}

void DAPMainView::DoThreadExpanding(const wxTreeItemId& item)
{
    CHECK_ITEM_RET(item);

    auto cd = GetFrameClientData(item);
    CHECK_PTR_RET(cd);
    CHECK_COND_RET(cd->IsThread());

    // if this item already loaded, avoid calling the debugger
    if(cd->loaded) {
        return;
    }

    if(m_threadsTree->ItemHasChildren(item)) {
        m_threadsTree->DeleteChildren(item);
        m_threadsTree->AppendItem(item, _("Loading..."));
    }
    m_plugin->GetClient().GetFrames(cd->GetId());
}

void DAPMainView::OnThreadItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    DoThreadExpanding(event.GetItem());
}

int DAPMainView::GetVariableId(const wxTreeItemId& item)
{
    auto cd = GetVariableClientData(item);
    if(!cd) {
        return wxNOT_FOUND;
    }
    return cd->reference;
}

int DAPMainView::GetThreadId(const wxTreeItemId& item)
{
    auto cd = GetFrameClientData(item);
    if(!cd) {
        return wxNOT_FOUND;
    }

    if(cd->IsThread()) {
        return cd->GetId();
    }
    return wxNOT_FOUND;
}

int DAPMainView::GetFrameId(const wxTreeItemId& item)
{
    auto cd = GetFrameClientData(item);
    if(!cd) {
        return wxNOT_FOUND;
    }

    if(cd->IsFrame()) {
        return cd->GetId();
    }
    return wxNOT_FOUND;
}

std::unordered_set<int> DAPMainView::GetExpandedThreads()
{
    std::unordered_set<int> result = { m_plugin->GetClient().GetActiveThreadId() };
    wxTreeItemIdValue cookie;
    wxTreeItemId root = m_threadsTree->GetRootItem();
    auto curitem = m_threadsTree->GetFirstChild(root, cookie);
    while(curitem.IsOk()) {
        if(m_threadsTree->IsExpanded(curitem)) {
            int cur_thread_id = GetThreadId(curitem);
            if(cur_thread_id != wxNOT_FOUND) {
                result.insert(cur_thread_id);
            }
        }
        curitem = m_threadsTree->GetNextChild(root, cookie);
    }
    return result;
}

// O(n)
wxTreeItemId DAPMainView::FindVariableNode(int refId)
{
    wxTreeItemId parent = m_variablesTree->GetRootItem();
    std::vector<wxTreeItemId> Q;
    // scan in dfs manner
    Q.push_back(parent);
    while(!Q.empty()) {
        wxTreeItemId item = Q.back();
        Q.pop_back();

        int curid = GetVariableId(item);
        if(curid == refId) {
            return item;
        }

        if(m_variablesTree->HasChildren(item)) {
            // direct access to this item children goes through clRowEntry (the internal impl)
            clRowEntry* as_entry = reinterpret_cast<clRowEntry*>(item.GetID());
            const auto& children = as_entry->GetChildren();
            Q.reserve(Q.size() + children.size());
            for(auto child : children) {
                Q.emplace_back(wxTreeItemId(child));
            }
        }
    }
    return wxTreeItemId(nullptr);
}

void DAPMainView::Clear()
{
    m_variablesTree->DeleteAllItems();
    m_threadsTree->DeleteAllItems();
}

FrameOrThreadClientData* DAPMainView::GetFrameClientData(const wxTreeItemId& item)
{
    CHECK_ITEM_RET_NULL(item);
    FrameOrThreadClientData* cd = dynamic_cast<FrameOrThreadClientData*>(m_threadsTree->GetItemData(item));
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

    m_threadsTree->SetDisabled(b);
    m_threadsTree->Refresh();
}

// we only need to check on view
bool DAPMainView::IsDisabled() const { return m_variablesTree->IsDisabled(); }

void DAPMainView::OnTimerCheckCanInteract(wxTimerEvent& event)
{
    event.Skip();
    CHECK_PTR_RET(m_plugin->GetClient().IsConnected());

    if(!m_plugin->GetClient().CanInteract()) {
        if(!IsDisabled()) {
            SetDisabled(true);
        }
    } else {
        if(IsDisabled()) {
            SetDisabled(false);
        }
    }
}

bool DAPMainView::DoCopyBacktrace(const wxTreeItemId& item, wxString* content)
{
    auto cd = GetFrameClientData(item);
    CHECK_PTR_RET_FALSE(cd);
    auto thread_item = item;
    if(cd->IsFrame()) {
        thread_item = m_threadsTree->GetItemParent(item);
    }
    cd = GetFrameClientData(thread_item);
    CHECK_PTR_RET_FALSE(cd);
    CHECK_COND_RET_FALSE(cd->IsThread());

    wxString backtrace;

    backtrace << cd->thread_info.id << " " << cd->thread_info.name << "\n";
    wxTreeItemIdValue cookie;
    auto child = m_variablesTree->GetFirstChild(thread_item, cookie);
    while(child.IsOk()) {
        cd = GetFrameClientData(child);
        if(!cd || !cd->IsFrame()) {
            return false;
        }
        for(size_t i = 0; i < 4; ++i) {
            backtrace << m_variablesTree->GetItemText(child, i) << ",";
        }
        backtrace.RemoveLast(); // the trailing comma
        backtrace << "\n";
        child = m_variablesTree->GetNextChild(thread_item, cookie);
    }
    content->swap(backtrace);
    return true;
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

void DAPMainView::OnThreadsListMenu(wxTreeEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    auto item = event.GetItem();

    auto cd = GetFrameClientData(item);
    CHECK_PTR_RET(cd);

    wxMenu menu;
    menu.Append(XRCID("expand_all_threads"), _("Expand all threads"));
    menu.AppendSeparator();
    menu.Append(XRCID("copy_all_threads_backtrace"), _("Copy all"));
    if(cd->IsFrame()) {
        menu.Append(XRCID("copy_current_threads_backtrace"), _("Copy this thread backtrace"));
        menu.Bind(
            wxEVT_MENU,
            [this, item](wxCommandEvent& e) {
                wxUnusedVar(e);
                wxString bt;
                DoCopyBacktrace(item, &bt);
                ::CopyToClipboard(bt);
            },
            XRCID("copy_current_threads_backtrace"));
    }

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            wxUnusedVar(e);
            wxTreeItemIdValue cookie;
            auto root = m_threadsTree->GetRootItem();
            auto child = m_threadsTree->GetFirstChild(root, cookie);
            m_threadsTree->Begin();
            while(child.IsOk()) {
                DoThreadExpanding(child);
                child = m_threadsTree->GetNextChild(root, cookie);
            }
            m_threadsTree->Commit();
        },
        XRCID("expand_all_threads"));
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& e) {
            wxUnusedVar(e);
            wxArrayString backtraces;
            wxTreeItemIdValue cookie;
            auto root = m_threadsTree->GetRootItem();
            auto child = m_threadsTree->GetFirstChild(root, cookie);
            while(child.IsOk()) {
                if(m_threadsTree->IsExpanded(child)) {
                    backtraces.push_back({});
                    auto& bt = backtraces.back();
                    DoCopyBacktrace(child, &bt);
                }
                child = m_threadsTree->GetNextChild(root, cookie);
            }

            wxString str;
            for(auto& bt : backtraces) {
                bt.Trim();
                str << bt << "\n\n";
            }

            str.RemoveLast();
            ::CopyToClipboard(str);
        },
        XRCID("copy_all_threads_backtrace"));
    m_threadsTree->PopupMenu(&menu);
}
