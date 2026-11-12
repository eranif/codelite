#pragma once

#include "TabAttentionIcon.hpp"
#include "codelite_exports.h"
#include "terminal_event.h"

#include <memory>

/// Shows the state of the program that runs in a page (see wxTerminalProgramState) as an icon on the tab of the page:
///
/// - Working: a spinner (it stays when the page gets the focus, as it only shows a state).
/// - Blocked: a pulse that asks the user to come back (the program waits for them).
/// - Done: a check mark.
/// - Idle, Clear, Error: no icon. A Blocked or Done icon stays until the user comes back to the page.
///
/// The Blocked and Done icons are shown only if the page is not the selected one, and they are removed when a window
/// inside the page gets the focus. They can be turned off with SetAttentionEnabled().
/// The icon that was on the tab before is restored when an icon is removed, and when this object is destroyed. If the
/// page is destroyed first, this object does nothing any more.
class WXDLLIMPEXP_SDK TabStatusIcon
{
public:
    /// The spinner and the pulse have the same number of frames, and they use the same frame time
    static constexpr int kFrameCount = 16;
    static constexpr int kFrameIntervalMs = 50;

    TabStatusIcon(clTabIconBook book, wxWindow* page);
    ~TabStatusIcon();

    TabStatusIcon(const TabStatusIcon&) = delete;
    TabStatusIcon& operator=(const TabStatusIcon&) = delete;

    /// Show the icon of `state`. Only a change of state does something (a working program reports its progress again
    /// and again).
    void SetState(wxTerminalProgramState state);

    /// The last state that was set
    wxTerminalProgramState GetState() const { return m_state; }

    /// Allow the Blocked and Done icons. The spinner is not affected. The default is true.
    void SetAttentionEnabled(bool enabled) { m_attentionEnabled = enabled; }

    /// The icon that is on the tab when no status icon is shown. When it is set, it is put back as it is when a status
    /// icon is removed. Without it, the icon is read from the tab (it is resized to the size of the status icons, see
    /// TabAttentionIcon). It is used the next time that a status icon is shown, not by the one that is shown now.
    void SetBaseIcon(const wxBitmap& icon) { m_baseIcon = icon; }

    /// Has the page been destroyed? Then this object does nothing, and can be deleted.
    bool IsPageGone() const { return m_page == nullptr; }

private:
    void OnPageDestroyed(wxWindowDestroyEvent& event);
    void ShowSpinner();
    void ShowAttention(wxTerminalProgramState state);

    clTabIconBook m_book;
    wxWindow* m_page{nullptr}; // nullptr after the page was destroyed
    // The first member is a spinner, the second asks for the user. Their frames are loaded when they are shown, so they
    // always match the theme
    std::unique_ptr<TabAttentionIcon> m_spinner;
    std::unique_ptr<TabAttentionIcon> m_attention;
    wxBitmap m_baseIcon;
    wxTerminalProgramState m_state{wxTerminalProgramState::Clear};
    bool m_attentionEnabled{true};
};
