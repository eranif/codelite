#include "duplicateTLWdlg.h"

DuplicateTLWDlg::DuplicateTLWDlg(wxWindow* parent)
    : DuplicateTLWBaseDlg(parent)
{
}

DuplicateTLWDlg::~DuplicateTLWDlg() {}

void DuplicateTLWDlg::OnInheritedNameFocus(wxFocusEvent& event)
{
    event.Skip();

    if(!m_textInheritedName->IsEmpty() || m_textBasename->IsEmpty()) {
        // There's nothing to copy, or we already have an entry
        return;
    }

    wxString suggestion = m_textBasename->GetValue();
    if(suggestion.Replace("Base", "") || suggestion.Replace("base", "")) {
        m_textInheritedName->ChangeValue(suggestion);
    }
}

void DuplicateTLWDlg::OnFilenameFocus(wxFocusEvent& event)
{
    event.Skip();

    if(!m_textFilename->IsEmpty() || m_textInheritedName->IsEmpty()) {
        // There's nothing to copy, or we already have an entry
        return;
    }

    wxString suggestion = m_textInheritedName->GetValue();
    if(suggestion.Lower() != suggestion) { m_textFilename->ChangeValue(suggestion.Lower()); }
}
