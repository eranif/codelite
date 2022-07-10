#ifndef DAPMODULEVIEW_H
#define DAPMODULEVIEW_H

#include "DAPTerminalCtrlView.h"
#include "UI.h"
#include "clTerminalViewCtrl.hpp"
#include "dap/dap.hpp"

class DAPModuleView : public DAPTerminalCtrlView
{
    clTerminalViewCtrl* m_ctrl = nullptr;

protected:
    void OnMenu(wxDataViewEvent& event);

public:
    DAPModuleView(wxWindow* parent, clModuleLogger& log);
    virtual ~DAPModuleView();

    void AddModuleEvent(dap::ModuleEvent* event);
};
#endif // DAPMODULEVIEW_H
