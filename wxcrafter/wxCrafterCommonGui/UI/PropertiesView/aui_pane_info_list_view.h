#pragma once

#include "wxc_widget.h"

#include <wx/propgrid/propgrid.h>

class AuiPaneInfoListView
{
    wxcWidget* m_wxcWidget;

public:
    AuiPaneInfoListView();
    virtual ~AuiPaneInfoListView() = default;
    void Changed(wxPropertyGrid* pg, wxPropertyGridEvent& e);
    void Construct(wxPropertyGrid* pg, wxcWidget* wb);
};
