#ifndef DAPMAINVIEW_H
#define DAPMAINVIEW_H

#include "UI.h"
#include "clModuleLogger.hpp"
#include "dap/Client.hpp"
#include "dap/DAPEvent.hpp"
#include "dap/dap.hpp"

#include <unordered_set>
#include <wx/timer.h>

class wxTimer;
class DebugAdapterClient;

// variable items client data
struct VariableClientData : public wxTreeItemData {
    int reference = wxNOT_FOUND;
    wxString value; // untrucated value
    VariableClientData(int ref, const wxString& v)
        : reference(ref)
        , value(v)
    {
    }
    virtual ~VariableClientData() {}
};

enum class FrameOrThread {
    THREAD,
    FRAME,
};

// every entry in the threads tree has this item data
struct FrameOrThreadClientData : public wxTreeItemData {
    FrameOrThread type;
    dap::StackFrame frame_info;
    dap::Thread thread_info;
    bool loaded = false;

    FrameOrThreadClientData(const dap::StackFrame& frameInfo)
        : type(FrameOrThread::FRAME)
        , frame_info(frameInfo)
    {
    }

    FrameOrThreadClientData(const dap::Thread& threadInfo)
        : type(FrameOrThread::THREAD)
        , thread_info(threadInfo)
    {
    }

    virtual ~FrameOrThreadClientData() {}

    bool IsFrame() const { return type == FrameOrThread::FRAME; }
    bool IsThread() const { return type == FrameOrThread::THREAD; }

    int GetId() const
    {
        if(IsFrame()) {
            return frame_info.id;
        } else {
            return thread_info.id;
        }
    }
};

/// --------------------------------------------------------------------

class DAPMainView : public DAPMainViewBase
{
    DebugAdapterClient* m_plugin = nullptr;
    wxTimer* m_timer = nullptr;
    // the variables displayed in the view are owned by this frame Id
    int m_scopesFrameId = wxNOT_FOUND;

    clModuleLogger& LOG;

protected:
    wxTreeItemId FindThreadNode(int threadId);
    wxTreeItemId FindVariableNode(int refId);

    FrameOrThreadClientData* GetFrameClientData(const wxTreeItemId& item);
    VariableClientData* GetVariableClientData(const wxTreeItemId& item);

    void OnTimerCheckCanInteract(wxTimerEvent& event);
    int GetThreadId(const wxTreeItemId& item);
    int GetVariableId(const wxTreeItemId& item);
    int GetFrameId(const wxTreeItemId& item);

    void DoThreadExpanding(const wxTreeItemId& item);
    bool DoCopyBacktrace(const wxTreeItemId& item, wxString* content);
    void OnThreadItemExpanding(wxTreeEvent& event);
    void OnFrameItemSelected(wxTreeEvent& event);
    void OnThreadsListMenu(wxTreeEvent& event);
    void OnVariablesMenu(wxTreeEvent& event);
    void OnScopeItemExpanding(wxTreeEvent& event);
    void Clear();

public:
    DAPMainView(wxWindow* parent, DebugAdapterClient* plugin, clModuleLogger& log);
    virtual ~DAPMainView();

    void UpdateThreads(int activeThreadId, dap::ThreadsResponse* response);
    void UpdateFrames(int threadId, dap::StackTraceResponse* response);
    void UpdateScopes(int frameId, dap::ScopesResponse* response);
    void UpdateVariables(int parentRef, dap::VariablesResponse* response);
    /**
     * @brief return list of thread-id that are expanded in the UI
     */
    std::unordered_set<int> GetExpandedThreads();

    void SetDisabled(bool b);
    bool IsDisabled() const;

    int GetCurrentFrameId() const { return m_scopesFrameId; }
};
#endif // DAPMAINVIEW_H
