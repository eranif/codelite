#ifndef DAPOUTPUTVIEW_H
#define DAPOUTPUTVIEW_H

#include "UI.h"
#include "clTerminalViewCtrl.hpp"
#include "dap/dap.hpp"

class DAPTerminalCtrlView : public DAPOutputViewBase
{
protected:
    clTerminalViewCtrl* m_ctrl = nullptr;

protected:
    void OnMenu(wxDataViewEvent& event);

public:
    DAPTerminalCtrlView(wxWindow* parent);
    virtual ~DAPTerminalCtrlView();
};
#endif // DAPOUTPUTVIEW_H
