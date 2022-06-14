#ifndef DAPMAINVIEW_H
#define DAPMAINVIEW_H

#include "UI.h"
#include "dap/Client.hpp"
#include "dap/DAPEvent.hpp"

class DAPMainView : public DAPMainViewBase
{
    dap::Client* m_client = nullptr;

public:
    DAPMainView(wxWindow* parent, dap::Client* client);
    virtual ~DAPMainView();

    void UpdateThreads(const DAPEvent& event);
};
#endif // DAPMAINVIEW_H
