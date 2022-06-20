#ifndef DAPMAINVIEW_H
#define DAPMAINVIEW_H

#include "UI.h"
#include "dap/Client.hpp"
#include "dap/DAPEvent.hpp"
#include "dap/dap.hpp"

#include <unordered_set>

class DAPMainView : public DAPMainViewBase
{
    dap::Client* m_client = nullptr;

protected:
    wxTreeItemId FindThreadNode(int threadId);
    int GetThreadId(const wxTreeItemId& item);

    void OnItemExpanding(wxTreeEvent& event);

public:
    DAPMainView(wxWindow* parent, dap::Client* client);
    virtual ~DAPMainView();

    void UpdateThreads(int activeThreadId, dap::ThreadsResponse* response);
    void UpdateFrames(int threadId, dap::StackTraceResponse* response);
    /**
     * @brief return list of thread-id that are expanded in the UI
     */
    std::unordered_set<int> GetExpandedThreads();
};
#endif // DAPMAINVIEW_H
