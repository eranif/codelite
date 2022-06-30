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

private:
    BreakpointClientData* GetItemData(const wxDataViewItem& item);

protected:
    void OnBreakpointActivated(wxDataViewEvent& item);

public:
    DAPBreakpointsView(wxWindow* parent, DebugAdapterClient* plugin);
    virtual ~DAPBreakpointsView();

    /**
     * @brief initialise the view by syncing the data with the store
     */
    void RefreshView(const SessionBreakpoints& breakpoints);
};

#endif // DAPBREAKPOINTSVIEW_H
