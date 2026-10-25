#include "TabAttentionIcon.hpp"

TabAttentionIcon::TabAttentionIcon(MainNotebook* book,
                                   wxWindow* page,
                                   const std::vector<wxBitmap>& frames,
                                   bool dismissOnFocus)
    : m_book(book)
    , m_page(page)
    , m_timer(this)
    , m_dismissOnFocus(dismissOnFocus)
{
    for (const wxBitmap& frame : frames) {
        if (frame.IsOk()) {
            m_frames.push_back(frame);
        }
    }
    if (m_frames.empty()) {
        return;
    }

    const int where = m_book->FindPage(m_page);
    if (where == wxNOT_FOUND) {
        return;
    }
    m_saved = m_book->GetPageBitmap(where);
    m_shown = true;
    ShowFrame(0);

    // The focus event of a child window travels up to its parents
    if (m_dismissOnFocus) {
        m_page->Bind(wxEVT_CHILD_FOCUS, &TabAttentionIcon::OnChildFocus, this);
    }

    if (m_frames.size() > 1) {
        Bind(wxEVT_TIMER, &TabAttentionIcon::OnTimer, this, m_timer.GetId());
        m_timer.Start(kFrameIntervalMs);
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
    if (m_dismissOnFocus) {
        m_page->Unbind(wxEVT_CHILD_FOCUS, &TabAttentionIcon::OnChildFocus, this);
    }

    // The page may be gone from the book already (the tab was closed)
    const int where = m_book->FindPage(m_page);
    if (where != wxNOT_FOUND) {
        m_book->SetPageBitmap(where, m_saved);
    }
}

void TabAttentionIcon::ShowFrame(size_t index)
{
    const int where = m_book->FindPage(m_page);
    if (where == wxNOT_FOUND) {
        return;
    }
    m_currentFrame = index % m_frames.size();
    m_book->SetPageBitmap(where, m_frames[m_currentFrame]);
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
