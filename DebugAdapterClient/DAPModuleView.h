#ifndef DAPMODULEVIEW_H
#define DAPMODULEVIEW_H

#include "DAPOutputView.h"
#include "UI.h"
#include "clTerminalViewCtrl.hpp"
#include "dap/dap.hpp"

class DAPModuleView : public DAPOutputView
{
    clTerminalViewCtrl* m_ctrl = nullptr;

protected:
    void OnMenu(wxDataViewEvent& event);

public:
    DAPModuleView(wxWindow* parent);
    virtual ~DAPModuleView();

    void AddModuleEvent(dap::ModuleEvent* event);
};
#endif // DAPMODULEVIEW_H
