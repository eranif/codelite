#include "TabAttentionIcon.hpp"

TabAttentionIcon::TabAttentionIcon(MainNotebook* book, wxWindow* page, const wxBitmap& icon)
    : m_book(book)
    , m_page(page)
{
    const int where = m_book->FindPage(m_page);
    if (where == wxNOT_FOUND) {
        return;
    }
    m_saved = m_book->GetPageBitmap(where);
    m_book->SetPageBitmap(where, icon);
    m_shown = true;

    // The focus event of a child window travels up to its parents
    m_page->Bind(wxEVT_CHILD_FOCUS, &TabAttentionIcon::OnChildFocus, this);
}

TabAttentionIcon::~TabAttentionIcon() { Dismiss(); }

void TabAttentionIcon::Dismiss()
{
    if (!m_shown) {
        return;
    }
    m_shown = false;
    m_page->Unbind(wxEVT_CHILD_FOCUS, &TabAttentionIcon::OnChildFocus, this);

    // The page may be gone from the book already (the tab was closed)
    const int where = m_book->FindPage(m_page);
    if (where != wxNOT_FOUND) {
        m_book->SetPageBitmap(where, m_saved);
    }
}

void TabAttentionIcon::OnChildFocus(wxChildFocusEvent& event)
{
    event.Skip();
    Dismiss();
}
