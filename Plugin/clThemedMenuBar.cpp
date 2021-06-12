#include "clThemedMenuBar.hpp"

#if !wxUSE_NATIVE_MENUBAR
clThemedMenuBar::clThemedMenuBar() {}

clThemedMenuBar::clThemedMenuBar(wxWindow* parent, size_t n, wxMenu* menus[], const wxString titles[], long style)
    : clMenuBar(parent, n, menus, titles, style)
{
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clThemedMenuBar::OnThemeChanged, this);
}

clThemedMenuBar::~clThemedMenuBar()
{
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clThemedMenuBar::OnThemeChanged, this);
}

void clThemedMenuBar::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    cl::ApplyTheme<clThemedMenuBar>(this);
}
#endif
