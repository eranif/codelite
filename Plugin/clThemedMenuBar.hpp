#ifndef CLTHEMEDMENUBAR_HPP
#define CLTHEMEDMENUBAR_HPP

#include "clMenuBar.hpp"
#if wxUSE_NATIVE_MENUBAR
typedef clMenuBar clThemedMenuBar;
#else
#include "clMenuBar.hpp"
#include "clThemedCtrl.hpp"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "event_notifier.h"

#include <wx/menu.h>

class WXDLLIMPEXP_SDK clThemedMenuBar : public clMenuBar
{
public:
    clThemedMenuBar();
    clThemedMenuBar(wxWindow* parent, size_t n, wxMenu* menus[], const wxString titles[], long style = 0);
    ~clThemedMenuBar();

    void OnThemeChanged(clCommandEvent& event);
};
#endif
#endif // CLTHEMEDMENUBAR_HPP
