#include "TabAttentionIcon.hpp"

TabAttentionIcon::TabAttentionIcon(
    clTabIconBook book, wxWindow* page, const std::vector<wxBitmap>& frames, bool dismissOnFocus, int frameIntervalMs)
    : m_book(std::move(book))
    , m_page(page)
    , m_timer(this)
    , m_dismissOnFocus(dismissOnFocus)
    , m_frameIntervalMs(frameIntervalMs)
{
    if (m_page == nullptr || !m_book.IsOk()) {
        return;
    }

    for (const wxBitmap& frame : frames) {
        if (frame.IsOk()) {
            m_frames.push_back(frame);
        }
    }
    if (m_frames.empty()) {
        return;
    }

    const int where = m_book.findPage(m_page);
    if (where == wxNOT_FOUND) {
        return;
    }
    m_saved = m_book.getBitmap(where);
    m_shown = true;
    ShowFrame(0);

    // The focus event of a child window travels up to its parents
    if (m_dismissOnFocus) {
        m_page->Bind(wxEVT_CHILD_FOCUS, &TabAttentionIcon::OnChildFocus, this);
    }
    m_page->Bind(wxEVT_DESTROY, &TabAttentionIcon::OnPageDestroyed, this);

    if (m_frames.size() > 1) {
        Bind(wxEVT_TIMER, &TabAttentionIcon::OnTimer, this, m_timer.GetId());
        m_timer.Start(m_frameIntervalMs);
    }
}

TabAttentionIcon::~TabAttentionIcon() { Dismiss(); }

void TabAttentionIcon::Dismiss()
{
    if (!m_shown) {
        return;
    }
    m_shown = false;
    m_timer.Stop();

    // The page is gone: there is no tab to restore
    if (m_page == nullptr) {
        return;
    }

    if (m_dismissOnFocus) {
        m_page->Unbind(wxEVT_CHILD_FOCUS, &TabAttentionIcon::OnChildFocus, this);
    }
    m_page->Unbind(wxEVT_DESTROY, &TabAttentionIcon::OnPageDestroyed, this);

    // The page may be gone from the book already (the tab was closed)
    const int where = m_book.findPage(m_page);
    if (where != wxNOT_FOUND) {
        m_book.setBitmap(where, m_saved);
    }
}

void TabAttentionIcon::ShowFrame(size_t index)
{
    if (m_page == nullptr) {
        return;
    }
    const int where = m_book.findPage(m_page);
    if (where == wxNOT_FOUND) {
        return;
    }
    m_currentFrame = index % m_frames.size();
    m_book.setBitmap(where, m_frames[m_currentFrame]);
}

void TabAttentionIcon::OnTimer(wxTimerEvent& event)
{
    wxUnusedVar(event);
    if (m_shown) {
        ShowFrame(m_currentFrame + 1);
    }
}

void TabAttentionIcon::OnChildFocus(wxChildFocusEvent& event)
{
    event.Skip();
    Dismiss();
}

void TabAttentionIcon::OnPageDestroyed(wxWindowDestroyEvent& event)
{
    event.Skip();
    // The page (and its tab) is being destroyed. Do not touch it, or the book, any more
    m_timer.Stop();
    m_shown = false;
    m_page = nullptr;
}
