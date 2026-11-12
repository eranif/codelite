#pragma once

#include "codelite_exports.h"
#include "imanager.h"

#include <cstddef>
#include <functional>
#include <vector>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/timer.h>

/// How TabAttentionIcon (and TabStatusIcon) reach the tabs of a book. Books do not share the same API for the tab
/// icons, so this describes what is needed. Use From() to get one for a book.
struct clTabIconBook {
    std::function<int(const wxWindow*)> findPage;        ///< The index of the page, or wxNOT_FOUND
    std::function<wxBitmap(int)> getBitmap;              ///< The icon of the tab
    std::function<void(int, const wxBitmap&)> setBitmap; ///< Set the icon of the tab
    std::function<bool(const wxWindow*)> isCurrentPage;  ///< Is this the selected page?

    bool IsOk() const { return findPage && getBitmap && setBitmap && isCurrentPage; }

    /// Get the description of `book` (a MainNotebook, a wxAuiNotebook, ...). The book must outlive the result.
    /// A book that cannot show a bitmap that is built at run time (clGenericNotebook, its tabs use ids of a fixed
    /// list of bitmaps) gives an object that is not ok: no icon is shown on its tabs.
    template <typename Book>
    static clTabIconBook From(Book* book)
    {
        clTabIconBook result;
        if constexpr (requires(Book& b, const wxBitmap& bmp) { b.SetPageBitmap(size_t{0}, bmp); }) {
            result.findPage = [book](const wxWindow* page) { return book->FindPage(page); };
            result.getBitmap = [book](int index) { return wxBitmap(book->GetPageBitmap(static_cast<size_t>(index))); };
            result.setBitmap = [book](int index, const wxBitmap& bmp) {
                book->SetPageBitmap(static_cast<size_t>(index), bmp);
            };
            result.isCurrentPage = [book](const wxWindow* page) { return book->GetCurrentPage() == page; };
        }
        return result;
    }
};

/// Puts an icon on the tab of `page` to tell the user that the page needs them, or to show a state (a spinner). The
/// icon can be animated: it is made of frames that are shown one after the other.
/// The icon is removed, and the icon that was on the tab before is restored, when a window inside the page gets the
/// keyboard focus (unless `dismissOnFocus` is false), or when this object is destroyed. If the page is destroyed
/// first, this object does nothing any more.
class WXDLLIMPEXP_SDK TabAttentionIcon : public wxEvtHandler
{
public:
    /// How long each frame is shown, unless the caller asks for another time
    static constexpr int kDefaultFrameIntervalMs = 100;

    /// @param frames The bitmaps to show, in order. They are shown in a loop. With one bitmap there is no animation.
    ///               Nothing is shown if the list has no valid bitmap.
    /// @param dismissOnFocus Remove the icon when the page gets the focus. Use false for an icon that shows a state
    ///                       (a spinner) and not a request to the user.
    /// @param frameIntervalMs How long each frame is shown
    /// @param restoreIcon The icon to put back on the tab when this icon is removed, if the caller knows it. Without
    ///                    it, the icon is read from the tab. That bitmap is not scaled for the DPI of the screen (see
    ///                    MatchBitmapSize()), so it is made the size of the frames
    TabAttentionIcon(clTabIconBook book,
                     wxWindow* page,
                     const std::vector<wxBitmap>& frames,
                     bool dismissOnFocus = true,
                     int frameIntervalMs = kDefaultFrameIntervalMs,
                     const wxBitmap& restoreIcon = wxNullBitmap);
    ~TabAttentionIcon() override;

    /// Is the icon on the tab?
    bool IsShown() const { return m_shown; }

    /// Remove the icon and restore the previous one. Does nothing if the icon is not shown.
    void Dismiss();

private:
    void OnChildFocus(wxChildFocusEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnPageDestroyed(wxWindowDestroyEvent& event);
    /// Show the frame number `index` on the tab
    void ShowFrame(size_t index);

    clTabIconBook m_book;
    wxWindow* m_page{nullptr}; // nullptr after the page was destroyed
    std::vector<wxBitmap> m_frames;
    size_t m_currentFrame{0};
    wxTimer m_timer;
    wxBitmap m_saved;
    bool m_shown{false};
    bool m_dismissOnFocus{true};
    int m_frameIntervalMs{kDefaultFrameIntervalMs};
};
