#ifndef DAPBREAKPOINTSVIEW_H
#define DAPBREAKPOINTSVIEW_H

#include "SessionBreakpoints.hpp"
#include "UI.h"
#include "dap/dap.hpp"

#include <unordered_set>

class DAPBreakpointsView : public DAPBreakpointsViewBase
{
public:
    DAPBreakpointsView(wxWindow* parent);
    virtual ~DAPBreakpointsView();

    /**
     * @brief initialise the view by syncing the data with the store
     */
    void RefreshView(const SessionBreakpoints& breakpoints);
};

#endif // DAPBREAKPOINTSVIEW_H
