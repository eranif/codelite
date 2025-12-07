#ifndef DAPMAINVIEW_H
#define DAPMAINVIEW_H

#include "DAPOutputPane.hpp"
#include "UI.h"
#include "clModuleLogger.hpp"
#include "dap/Client.hpp"
#include "dap/DAPEvent.hpp"
#include "dap/dap.hpp"

#include <wx/timer.h>

class wxTimer;
class DebugAdapterClient;

// variable items client data
struct VariableClientData : public wxTreeItemData {
    int reference = wxNOT_FOUND;
    wxString value; // untruncated value
    VariableClientData(int ref, const wxString& v)
        : reference(ref)
        , value(v)
    {
    }
    virtual ~VariableClientData() = default;
};

struct ThreadInfo {
    dap::Thread thread_info;
    std::vector<dap::StackFrame> frames;

    ThreadInfo(const dap::Thread& t)
        : thread_info(t)
    {
    }
    bool has_frames() const { return !frames.empty(); }
    int thread_id() const { return thread_info.id; }

    /// Return this thread backtrace as a string
    wxString GetBacktrace() const
    {
        wxString backtrace;
        backtrace << "Thread: " << thread_info.id << ":" << thread_info.name << "\n";
        for (const auto& frame : frames) {
            wxString source;
            if (!frame.source.path.empty()) {
                source << frame.source.path << ":" << frame.line;
            } else {
                source = frame.source.name;
            }
            backtrace << frame.id << ", " << frame.name << ", " << source << "\n";
        }
        backtrace << "\n";
        return backtrace;
    }
};

struct FrameInfo {
    dap::StackFrame frame;
    FrameInfo(const dap::StackFrame& f)
        : frame(f)
    {
    }
};

/// --------------------------------------------------------------------

class DAPMainView : public DAPMainViewBase
{
public:
    DAPMainView(wxWindow* parent, DebugAdapterClient* plugin, clModuleLogger& log);
    virtual ~DAPMainView();

    void UpdateThreads(int activeThreadId, dap::ThreadsResponse* response);
    void UpdateFrames(int threadId, dap::StackTraceResponse* response);
    void DoUpdateFrames(int threadId, const std::vector<dap::StackFrame>& frames);
    void UpdateScopes(int frameId, dap::ScopesResponse* response);
    void UpdateVariables(int parentRef, dap::VariablesResponse* response);

    void SetDisabled(bool b);
    bool IsDisabled() const;
    void Clear();

    int GetCurrentFrameId() const { return m_scopesFrameId; }
    DAPOutputPane* GetOutputPane() const { return m_outputPane; }

protected:
    void OnFrameChanged(wxDataViewEvent& event) override;
    void OnThreadIdChanged(wxDataViewEvent& event) override;
    wxDataViewItem FindThread(int threadId) const;
    wxTreeItemId FindVariableNode(int refId);

    ThreadInfo* GetThreadInfo(const wxDataViewItem& item);
    FrameInfo* GetFrameInfo(const wxDataViewItem& item);
    VariableClientData* GetVariableClientData(const wxTreeItemId& item);

    void OnTimerCheckCanInteract(wxTimerEvent& event);
    int GetVariableId(const wxTreeItemId& item);

    void OnThreadsListMenu(wxDataViewEvent& event) override;
    void OnVariablesMenu(wxTreeEvent& event);
    void OnScopeItemExpanding(wxTreeEvent& event);
    void DoCopyAllThreadsBacktrace();

private:
    DebugAdapterClient* m_plugin = nullptr;
    wxTimer* m_timer = nullptr;
    // the variables displayed in the view are owned by this frame Id
    int m_scopesFrameId = wxNOT_FOUND;

    clModuleLogger& LOG;
    DAPOutputPane* m_outputPane = nullptr;
    std::vector<size_t> m_getFramesRequests;
};
#endif // DAPMAINVIEW_H
