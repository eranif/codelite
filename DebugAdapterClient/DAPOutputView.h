#ifndef DAPOUTPUTVIEW_H
#define DAPOUTPUTVIEW_H

#include "UI.h"
#include "clTerminalViewCtrl.hpp"
#include "dap/dap.hpp"

class DAPOutputView : public DAPOutputViewBase
{
protected:
    clTerminalViewCtrl* m_ctrl = nullptr;

protected:
    void OnMenu(wxDataViewEvent& event);

public:
    DAPOutputView(wxWindow* parent);
    virtual ~DAPOutputView();
};
#endif // DAPOUTPUTVIEW_H
