#ifndef SIZERFLAGSLISTVIEW_H
#define SIZERFLAGSLISTVIEW_H

#include <wx/propgrid/propgrid.h> // Base class: wxPropertyGrid

class wxcWidget;
class wxFlagsProperty;

class SizerFlagsListView
{
    wxcWidget* m_wxcWidget;
    wxFlagsProperty* m_flags;

public:
    SizerFlagsListView();
    virtual ~SizerFlagsListView();

    void Changed(wxPropertyGrid* pg, wxPropertyGridEvent& e);
    void Construct(wxPropertyGrid* pg, wxcWidget* wb);

    void DoUpdateUI(wxPropertyGrid* pg, wxUpdateUIEvent& event);
};

#endif // SIZERFLAGSLISTVIEW_H
