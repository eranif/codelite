#pragma once

#include "DAPTerminalCtrlView.h"
#include "clTerminalViewCtrl.hpp"
#include "dap/dap.hpp"

class DAPModuleView : public DAPTerminalCtrlView
{
    clTerminalViewCtrl* m_ctrl = nullptr;

protected:
    void OnMenu(wxDataViewEvent& event);

public:
    DAPModuleView(wxWindow* parent);
    ~DAPModuleView() override = default;

    void AddModuleEvent(dap::ModuleEvent* event);
};
