#include "clThemedSplitterWindow.h"

#include "clSystemSettings.h"
#include "drawingutils.h"
#include "event_notifier.h"

#include <wx/dcbuffer.h>

clThemedSplitterWindow::clThemedSplitterWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                               long style, const wxString& name)
{
    Create(parent, id, pos, size, style);
}

clThemedSplitterWindow::~clThemedSplitterWindow() {}

bool clThemedSplitterWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                                    const wxString& name)
{
    style = wxSP_LIVE_UPDATE;
#ifdef __WXMSW__
    if(wxSystemSettings::GetAppearance().IsDark()) {
        style |= wxSP_BORDER;
    } else {
        style |= wxBORDER_THEME;
    }
#endif
    return wxSplitterWindow::Create(parent, id, pos, size, style, name);
}
