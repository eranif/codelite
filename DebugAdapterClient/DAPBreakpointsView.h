#ifndef DAPBREAKPOINTSVIEW_H
#define DAPBREAKPOINTSVIEW_H

#include "UI.h"
#include "dap/dap.hpp"

#include <unordered_set>

class DAPBreakpointsView : public DAPBreakpointsViewBase
{
    std::unordered_set<dap::Breakpoint> m_breakpoints;

protected:
    void RefreshView();

public:
    DAPBreakpointsView(wxWindow* parent);
    virtual ~DAPBreakpointsView();

    /**
     * @brief set or update a breakpoint
     */
    void SetBreakpoint(const dap::Breakpoint& breakpoint);

    /**
     * @brief set or update list of breakpoints
     */
    void SetBreakpoints(const std::vector<dap::Breakpoint>& breakpoint);\

    /**
     * @brief remove all the breakpoints from the ui
     */
    void Clear();
};

#endif // DAPBREAKPOINTSVIEW_H
