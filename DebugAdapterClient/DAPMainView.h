#ifndef DAPMAINVIEW_H
#define DAPMAINVIEW_H

#include "UI.h"
#include "clModuleLogger.hpp"
#include "dap/Client.hpp"
#include "dap/DAPEvent.hpp"
#include "dap/dap.hpp"

#include <unordered_set>

class DAPMainView : public DAPMainViewBase
{
    dap::Client* m_client = nullptr;

    // the variables displayed in the view are owned by this frame Id
    int m_scopesFrameId = wxNOT_FOUND;

    clModuleLogger& LOG;

protected:
    wxTreeItemId FindThreadNode(int threadId);
    wxTreeItemId FindVariableNode(int refId);
    int GetThreadId(const wxTreeItemId& item);
    int GetVariableId(const wxTreeItemId& item);

    void OnThreadItemExpanding(wxTreeEvent& event);
    void OnScopeItemExpanding(wxTreeEvent& event);
    void Clear();

public:
    DAPMainView(wxWindow* parent, dap::Client* client, clModuleLogger& log);
    virtual ~DAPMainView();

    void UpdateThreads(int activeThreadId, dap::ThreadsResponse* response);
    void UpdateFrames(int threadId, dap::StackTraceResponse* response);
    void UpdateScopes(int frameId, dap::ScopesResponse* response);
    void UpdateVariables(int parentRef, dap::VariablesResponse* response);
    /**
     * @brief return list of thread-id that are expanded in the UI
     */
    std::unordered_set<int> GetExpandedThreads();
};
#endif // DAPMAINVIEW_H
