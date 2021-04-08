#ifndef CLTHEMEDMENUBAR_HPP
#define CLTHEMEDMENUBAR_HPP

#include "clMenuBar.hpp"
#include "clThemedCtrl.hpp"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "event_notifier.h"

#ifdef __WXMSW__
class WXDLLIMPEXP_SDK clThemedMenuBar : public clMenuBar
{
public:
    clThemedMenuBar();
    clThemedMenuBar(wxWindow* parent, size_t n, wxMenu* menus[], const wxString titles[], long style = 0);
    ~clThemedMenuBar();

    void OnThemeChanged(wxCommandEvent& event);
};
#else
typedef clThemedMenuBar wxMenuBar;
typedef clMenuBar wxMenuBar;
#endif
#endif // CLTHEMEDMENUBAR_HPP
