#include "DAPMainView.h"

#include "globals.h"

DAPMainView::DAPMainView(wxWindow* parent, dap::Client* client)
    : DAPMainViewBase(parent)
    , m_client(client)
{
    m_treeThreads->SetTreeStyle(m_treeThreads->GetTreeStyle() | wxTR_HIDE_ROOT);
    m_treeThreads->SetShowHeader(true);
    m_treeThreads->AddHeader(_("ID"));     // The thread/frame ID
    m_treeThreads->AddHeader(_("Name"));   // The thread/frame name
    m_treeThreads->AddHeader(_("Source")); // The frame source (function or file)
    m_treeThreads->AddHeader(_("Line"));   // The frame line
    m_treeThreads->AddRoot(_("Threads"));
}

DAPMainView::~DAPMainView() {}

void DAPMainView::UpdateThreads(int activeThreadId, dap::ThreadsResponse* response)
{
    wxUnusedVar(activeThreadId);

    // threads are the top level
    if(!response || !response->success) {
        return;
    }

    // build a map that matches thread-id -> wxTreeItemId
    std::unordered_map<int, wxTreeItemId> M;
    wxTreeItemId root = m_treeThreads->GetRootItem();
    wxTreeItemIdValue cookie;
    auto curitem = m_treeThreads->GetFirstChild(root, cookie);
    while(curitem.IsOk()) {
        M.insert({ ::wxStringToInt(m_treeThreads->GetItemText(curitem, 0), -1), curitem });
        curitem = m_treeThreads->GetNextChild(root, cookie);
    }

    m_treeThreads->Begin();
    for(const auto& thread : response->threads) {
        wxTreeItemId item;
        if(M.count(thread.id) == 0) {
            // new thread, add it
            item = m_treeThreads->AppendItem(root, wxString() << thread.id);
            // add a dummy child, so will get the ">" button
            m_treeThreads->AppendItem(item, "<dummy>");
        } else {
            item = M[thread.id];
            M.erase(thread.id); // remove it
        }
        m_treeThreads->SetItemText(item, thread.name, 1);
    }

    // remove any item left in the Map
    for(const auto& vt : M) {
        m_treeThreads->Delete(vt.second);
    }
    M.clear();
    m_treeThreads->Commit();
}

wxTreeItemId DAPMainView::FindThreadNode(int threadId)
{
    wxTreeItemId root = m_treeThreads->GetRootItem();
    wxTreeItemIdValue cookie;
    auto curitem = m_treeThreads->GetFirstChild(root, cookie);
    while(curitem.IsOk()) {
        int cur_thread_id = ::wxStringToInt(m_treeThreads->GetItemText(curitem, 0), -1);
        if(cur_thread_id == threadId) {
            return curitem;
        }
        curitem = m_treeThreads->GetNextChild(root, cookie);
    }
    return wxTreeItemId(nullptr);
}

void DAPMainView::UpdateFrames(int threadId, dap::StackTraceResponse* response)
{
    // locate the row
    m_treeThreads->Begin();
    wxTreeItemId parent = FindThreadNode(threadId);
    if(!parent.IsOk()) {
        return;
    }

    if(m_treeThreads->ItemHasChildren(parent)) {
        m_treeThreads->DeleteChildren(parent);
    }

    // append the stack frame
    for(const auto& frame : response->stackFrames) {
        wxTreeItemId frame_item = m_treeThreads->AppendItem(parent, wxString() << frame.id);
        m_treeThreads->SetItemText(frame_item, frame.name, 1);

        wxString source;
        if(!frame.source.name.empty()) {
            source = frame.source.name;
        } else {
            source = frame.source.path;
        }
        m_treeThreads->SetItemText(frame_item, source, 2);
        m_treeThreads->SetItemText(frame_item, wxString() << frame.line, 3);
    }
    m_treeThreads->Expand(parent);
    m_treeThreads->Commit();
}
