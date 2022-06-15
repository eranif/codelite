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

void DAPMainView::UpdateThreads(const DAPEvent& event)
{
    // threads are the top level
    auto response = event.GetDapResponse()->As<dap::ThreadsResponse>();
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
