#pragma once

#include "imanager.h"

#include <wx/bitmap.h>
#include <wx/event.h>

/// Puts an icon on the tab of `page` to tell the user that the page needs them. The icon is removed, and the icon that
/// was on the tab before is restored, when a window inside the page gets the keyboard focus, or when this object is
/// destroyed.
class TabAttentionIcon : public wxEvtHandler
{
public:
    TabAttentionIcon(MainNotebook* book, wxWindow* page, const wxBitmap& icon);
    ~TabAttentionIcon() override;

    /// Is the icon on the tab?
    bool IsShown() const { return m_shown; }

    /// Remove the icon and restore the previous one. Does nothing if the icon is not shown.
    void Dismiss();

private:
    void OnChildFocus(wxChildFocusEvent& event);

    MainNotebook* m_book{nullptr};
    wxWindow* m_page{nullptr};
    wxBitmap m_saved;
    bool m_shown{false};
};
