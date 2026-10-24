#pragma once

#include "imanager.h"

#include <vector>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/timer.h>

/// Puts an animated icon on the tab of `page` to tell the user that the page needs them. The icon is made of frames
/// that are shown one after the other (for example a dot that grows, so it pulses).
/// The icon is removed, and the icon that was on the tab before is restored, when a window inside the page gets the
/// keyboard focus, or when this object is destroyed.
class TabAttentionIcon : public wxEvtHandler
{
public:
    /// How long each frame is shown
    static constexpr int kFrameIntervalMs = 100;

    /// @param frames The bitmaps to show, in order. They are shown in a loop. With one bitmap there is no animation.
    ///               Nothing is shown if the list has no valid bitmap.
    TabAttentionIcon(MainNotebook* book, wxWindow* page, const std::vector<wxBitmap>& frames);
    ~TabAttentionIcon() override;

    /// Is the icon on the tab?
    bool IsShown() const { return m_shown; }

    /// Remove the icon and restore the previous one. Does nothing if the icon is not shown.
    void Dismiss();

private:
    void OnChildFocus(wxChildFocusEvent& event);
    void OnTimer(wxTimerEvent& event);
    /// Show the frame number `index` on the tab
    void ShowFrame(size_t index);

    MainNotebook* m_book{nullptr};
    wxWindow* m_page{nullptr};
    std::vector<wxBitmap> m_frames;
    size_t m_currentFrame{0};
    wxTimer m_timer;
    wxBitmap m_saved;
    bool m_shown{false};
};
