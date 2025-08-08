#ifndef STYLESSHEETLISTVIEW_H
#define STYLESSHEETLISTVIEW_H

#include <wx/propgrid/propgrid.h>

class wxcWidget;
class StylesSheetListView
{
    wxcWidget* m_wxcWidget;
    wxFlagsProperty* m_controlFlags;
    wxFlagsProperty* m_windowFlags;

public:
    StylesSheetListView();
    virtual ~StylesSheetListView();

    void Changed(wxPropertyGrid* pg, wxPropertyGridEvent& e);
    void Construct(wxPropertyGrid* pg, wxcWidget* wb);
    void Clear(wxPropertyGrid* pg);
};

#endif // STYLESSHEETLISTVIEW_H
