#ifndef AUIPANEINFOLISTVIEW_H
#define AUIPANEINFOLISTVIEW_H

#include "wxc_widget.h"
#include <wx/propgrid/propgrid.h>

class AuiPaneInfoListView
{
    wxcWidget* m_wxcWidget;

public:
    AuiPaneInfoListView();
    virtual ~AuiPaneInfoListView();
    void Changed(wxPropertyGrid* pg, wxPropertyGridEvent& e);
    void Construct(wxPropertyGrid* pg, wxcWidget* wb);
};

#endif // AUIPANEINFOLISTVIEW_H
