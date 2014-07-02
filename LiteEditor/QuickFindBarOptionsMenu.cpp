#include "QuickFindBarOptionsMenu.h"
#include "quickfindbar.h"

QuickFindBarOptionsMenu::QuickFindBarOptionsMenu(wxWindow* parent, QuickFindBar* qfb)
    : QuickFindBarOptionsMenuBase(parent)
    , m_bar(qfb)
{
}

QuickFindBarOptionsMenu::~QuickFindBarOptionsMenu()
{
}

void QuickFindBarOptionsMenu::OnCheckBoxRegex(wxCommandEvent& event)
{
    m_bar->CallAfter( &QuickFindBar::OnCheckBoxRegex, event );
}

void QuickFindBarOptionsMenu::OnCheckWild(wxCommandEvent& event)
{
    m_bar->CallAfter( &QuickFindBar::OnCheckWild, event );
}

void QuickFindBarOptionsMenu::OnHighlightMatches(wxCommandEvent& event)
{
    m_bar->CallAfter( &QuickFindBar::OnHighlightMatches, event );
}

void QuickFindBarOptionsMenu::OnDismiss()
{
    // We got dismissed - untoggle the button
    m_bar->CallAfter( &QuickFindBar::OnOptionsMenuDismissed );
}
