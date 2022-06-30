#ifndef DAPOUTPUTVIEW_H
#define DAPOUTPUTVIEW_H

#include "UI.h"
#include "clTerminalViewCtrl.hpp"
#include "dap/dap.hpp"

class DAPOutputView : public DAPOutputViewBase
{
    clTerminalViewCtrl* m_ctrl = nullptr;

public:
    DAPOutputView(wxWindow* parent);
    virtual ~DAPOutputView();

    void AddOutputEvent(dap::OutputEvent* event);
};
#endif // DAPOUTPUTVIEW_H
