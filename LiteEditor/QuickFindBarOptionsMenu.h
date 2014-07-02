#ifndef QUICKFINDBAROPTIONSMENU_H
#define QUICKFINDBAROPTIONSMENU_H
#include "quickfindbarbase.h"

class QuickFindBar;
class QuickFindBarOptionsMenu : public QuickFindBarOptionsMenuBase
{
    QuickFindBar* m_bar;
public:
    QuickFindBarOptionsMenu( wxWindow* parent, QuickFindBar* qfb );
    virtual ~QuickFindBarOptionsMenu();

    wxCheckBox* GetCheckBoxCase() {
        return m_checkBoxCase;
    }
    wxCheckBox* GetCheckBoxHighlight() {
        return m_checkBoxHighlight;
    }
    wxCheckBox* GetCheckBoxMultipleSelections() {
        return m_checkBoxMultipleSelections;
    }
    wxCheckBox* GetCheckBoxRegex() {
        return m_checkBoxRegex;
    }
    wxCheckBox* GetCheckBoxWildcard() {
        return m_checkBoxWildcard;
    }
    wxCheckBox* GetCheckBoxWord() {
        return m_checkBoxWord;
    }

protected:
    virtual void OnCheckBoxRegex( wxCommandEvent& event );
    virtual void OnCheckWild( wxCommandEvent& event );
    virtual void OnHighlightMatches( wxCommandEvent& event );
};
#endif // QUICKFINDBAROPTIONSMENU_H
