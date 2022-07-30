#ifndef DAPBREAKPOINTSVIEW_H
#define DAPBREAKPOINTSVIEW_H

#include "SessionBreakpoints.hpp"
#include "UI.h"
#include "dap/dap.hpp"

#include <unordered_set>
class DebugAdapterClient;

struct BreakpointClientData {
    dap::Breakpoint m_breapoint;
    BreakpointClientData(const dap::Breakpoint& breakpoint)
        : m_breapoint(breakpoint)
    {
    }
    ~BreakpointClientData() {}
};

class DAPBreakpointsView : public DAPBreakpointsViewBase
{
    DebugAdapterClient* m_plugin = nullptr;
    std::vector<dap::FunctionBreakpoint> m_functionBreakpoints;

private:
    BreakpointClientData* GetItemData(const wxDataViewItem& item);

protected:
    virtual void OnBreakpointsContextMenu(wxDataViewEvent& event);
    void OnBreakpointActivated(wxDataViewEvent& item);
    void OnNewFunctionBreakpoint(wxCommandEvent& event);
    void OnNewSourceBreakpoint(wxCommandEvent& event);

public:
    DAPBreakpointsView(wxWindow* parent, DebugAdapterClient* plugin);
    virtual ~DAPBreakpointsView();

    /**
     * @brief initialise the view by syncing the data with the store
     */
    void RefreshView(const SessionBreakpoints& breakpoints);
};

#endif // DAPBREAKPOINTSVIEW_H
